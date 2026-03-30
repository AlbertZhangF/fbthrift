/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <folly/SocketAddress.h>
#include <folly/init/Init.h>
#include <folly/io/async/EventBase.h>
#include <folly/io/async/ScopedEventBaseThread.h>
#include <thrift/example/cpp2/server/EchoService.h>
#include <thrift/example/if/gen-cpp2/Echo.h>
#include <thrift/perf/cpp2/util/Util.h>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <thread>
#include <vector>

DEFINE_string(host, "::1", "EchoServer host");
DEFINE_int32(port, 7778, "EchoServer port");
DEFINE_string(
    transport,
    "header",
    "Transport to use: header, rsocket, http2, or inmemory");
DEFINE_int32(packet_size, 1024, "Fixed packet size in bytes");
DEFINE_int32(duration, 10, "Test duration in seconds");
DEFINE_int32(thread_num, 10, "Number of worker threads");

using example::chatroom::EchoAsyncClient;

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  folly::init(&argc, &argv);

  auto addr = folly::SocketAddress(FLAGS_host, FLAGS_port);

  std::string message(FLAGS_packet_size, 'A');
  std::atomic<uint64_t> successCount{0};
  std::atomic<uint64_t> failCount{0};
  std::atomic<bool> running{true};

  std::vector<std::thread> workers;
  workers.reserve(FLAGS_thread_num);

  auto startTime = std::chrono::steady_clock::now();

  for (int i = 0; i < FLAGS_thread_num; ++i) {
    workers.emplace_back([&]() {
      folly::EventBase evb;
      auto client = newClient<EchoAsyncClient>(&evb, addr, FLAGS_transport);
      std::string response;

      while (running) {
        try {
          client->sync_echo(response, message);
          successCount++;
        } catch (const std::exception& ex) {
          failCount++;
        }
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::seconds(FLAGS_duration));
  running = false;

  for (auto& t : workers) {
    t.join();
  }

  auto endTime = std::chrono::steady_clock::now();
  auto elapsedMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
          .count();
  double elapsedSeconds = elapsedMs / 1000.0;

  uint64_t totalRequests = successCount + failCount;
  double qps = totalRequests / elapsedSeconds;

  LOG(INFO) << "========== Benchmark Results ==========";
  LOG(INFO) << "Packet Size: " << FLAGS_packet_size << " bytes";
  LOG(INFO) << "Thread Num: " << FLAGS_thread_num;
  LOG(INFO) << "Duration: " << elapsedSeconds << " seconds";
  LOG(INFO) << "Total Requests: " << totalRequests;
  LOG(INFO) << "Success: " << successCount;
  LOG(INFO) << "Failed: " << failCount;
  LOG(INFO) << "QPS: " << std::fixed << std::setprecision(2) << qps;
  LOG(INFO) << "Throughput: " << std::fixed << std::setprecision(2)
            << (qps * FLAGS_packet_size / 1024 / 1024) << " MB/s";
  LOG(INFO) << "=======================================";

  return 0;
}