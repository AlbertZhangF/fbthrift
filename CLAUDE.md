# Facebook Thrift (FBThrift)

## 项目概述

Facebook Thrift 是一个高性能的序列化和 RPC（远程过程调用）框架，用于服务间通信。该项目是 Facebook 基于 Apache Thrift 演进而来的内部分支，于 2014 年 2 月重新开源。Facebook Thrift 在原有基础上进行了重大改进，特别是编译器从零开始重写，并实现了完全异步的 Thrift 服务器。

## 核心架构

### 三大核心组件

#### 1. 代码生成器 (Code Generator)
- 解析 `.thrift` IDL 文件
- 生成不同语言的数据结构和客户端/服务端存根代码
- 支持多种目标语言的代码生成

#### 2. 序列化框架 (Serialization Framework)
- 提供多种协议实现（Binary、Compact、JSON 等）
- 支持跨语言的序列化和反序列化
- 高性能的二进制协议实现

#### 3. RPC 框架 (RPC Framework)
- 客户端和服务端之间的消息帧处理
- 支持同步和异步调用模式
- 完全异步的 C++ 服务器实现（Cpp2）

## 目录结构

```
fbthrift/
├── thrift/
│   ├── annotation/          # 内置 Thrift 注解定义
│   ├── compiler/            # Thrift 编译器（thrift1）
│   │   ├── ast/            # 抽象语法树定义
│   │   ├── parse/          # 解析器实现
│   │   ├── gen/            # 代码生成器
│   │   └── sema/           # 语义分析
│   ├── lib/                # 各语言运行时库
│   │   ├── cpp/            # C++ 运行时库（第一代）
│   │   ├── cpp2/           # C++ 运行时库（第二代，异步）
│   │   ├── py/             # Python 运行时库
│   │   ├── py3/            # Python3 运行时库（Cython 实现）
│   │   ├── java/           # Java 运行时库
│   │   ├── go/             # Go 运行时库
│   │   ├── rust/           # Rust 运行时库
│   │   ├── hack/           # Hack 运行时库
│   │   └── thrift/         # Thrift 元类型定义
│   ├── conformance/        # 一致性测试套件
│   ├── doc/                # 官方文档
│   ├── example/            # 使用示例
│   ├── perf/               # 性能基准测试
│   └── test/               # 测试套件
├── build/                   # 构建工具和依赖管理
└── ThriftLibrary.cmake     # CMake 构建辅助脚本
```

## 支持的语言

### 主要支持语言
- **C++**: 完整支持，包含两代实现（cpp 和 cpp2）
- **Python**: 支持 Python 2/3 和异步 Python
- **Java**: 完整支持，包含 Swift 注解
- **Hack**: PHP/Hack 语言支持
- **Go**: 完整的 Go 语言支持
- **Rust**: Rust 语言支持

### 其他语言
- JavaScript
- Android Lite

## 关键特性

### 1. 高性能
- Cpp2 服务器比 NonblockingServer 快约 4 倍
- 支持零拷贝操作（使用 folly::IOBuf）
- 缓冲链避免大内存分配
- 异步处理和乱序响应

### 2. 易用性
- 自动生成序列化和 RPC 样板代码
- 开发者专注于 schema 和接口定义
- 支持多种序列化协议

### 3. 跨语言支持
- 不同语言间的无缝通信
- 例如：Python 客户端与 C++ 服务器通信

### 4. 向后兼容性
- 字段可以安全地添加或删除
- 保持向前和向后兼容性

### 5. 异步服务器 (Cpp2)
- 基于 C++11 实现
- 支持异步回调和 Future 接口
- 乱序响应和并行处理
- 动态压缩和追踪支持
- 使用 eventfd 替代管道通知

## 构建系统

### 依赖项

**系统依赖**:
- Boost
- CMake (>= 3.1.3)
- OpenSSL (>= 1.0.2g)
- PThreads
- Python
- Zlib

**外部依赖**:
- {fmt}
- GFlags
- GLog
- GTest 和 GMock

**Facebook 依赖**:
- Fizz
- Folly
- Wangle
- Zstd

### 构建命令

```bash
# 克隆仓库
git clone https://github.com/facebook/fbthrift
cd fbthrift

# 安装系统依赖（Linux/macOS）
sudo ./build/fbcode_builder/getdeps.py install-system-deps --recursive fbthrift

# 构建
./build/fbcode_builder/getdeps.py --allow-system-packages build fbthrift
```

### CMake 选项
- `compiler_only`: 仅构建 Thrift 编译器（默认 OFF）
- `enable_tests`: 启用测试（默认 OFF）

## IDL 语法示例

```thrift
// 定义结构体
struct Message {
  1: string message;
  2: string sender;
  3: i64 timestamp;
}

// 定义服务
service ChatRoom {
  GetMessagesResponse getMessages(1: GetMessagesRequest request);
  void sendMessage(1: SendMessageRequest request);
}
```

## 核心类型系统

### 基本类型
- `bool`: 布尔值
- `byte`: 8位有符号整数
- `i16`, `i32`, `i64`: 不同大小的整数
- `double`: 64位浮点数
- `string`: 字符串
- `binary`: 二进制数据

### 容器类型
- `list<T>`: 列表
- `set<T>`: 集合
- `map<K,V>`: 映射

### 复杂类型
- `struct`: 结构体
- `union`: 联合体
- `exception`: 异常
- `enum`: 枚举
- `typedef`: 类型别名

## 高级特性

### 1. Any 类型
支持动态类型的值，可以在运行时序列化和反序列化任意 Thrift 类型。

### 2. 类型适配器 (Adapter)
允许自定义类型映射和转换逻辑。

### 3. 静态反射 (C++)
编译时类型反射支持，用于元编程和序列化优化。

### 4. 字段掩码 (Field Mask)
支持部分序列化和选择性字段传输。

### 5. Patch 操作
支持增量更新和数据补丁。

### 6. 流式 RPC
支持流式请求和响应。

## 协议支持

### 标准协议
- **Binary**: 简单的二进制协议
- **Compact**: 紧凑的二进制协议
- **JSON**: JSON 文本协议
- **SimpleJSON**: 简化的 JSON 协议

### 自定义协议
支持通过扩展实现自定义序列化协议。

## 传输层

- TSocket: TCP 套接字
- TSSLSocket: SSL/TLS 加密套接字
- THeader: 带头部信息的传输
- HTTP: HTTP 传输

## 注解系统

Thrift 支持丰富的注解来扩展类型和行为：

```thrift
@scope.Field
struct OutputOnly {}

@scope.Field
struct Immutable {}

@scope.Field
struct Unique {}
```

## 性能优化

### Cpp2 服务器优化
- 使用 folly::IOBuf 实现零拷贝
- 异步 I/O 和事件驱动
- 连接复用和请求管道化
- 智能内存管理

### 序列化优化
- 紧凑的二进制格式
- Varint 编码
- 字段省略（terse writes）

## 测试和验证

### 测试套件
- 单元测试
- 集成测试
- 一致性测试（conformance）
- 模糊测试（fuzzer）

### 性能测试
- 基准测试套件（thrift/perf）
- 负载生成器
- 多语言性能对比

## 文档资源

- 官方文档: `thrift/doc/`
- API 参考: `thrift/doc/references/`
- 教程: `thrift/doc/tutorials/`
- 故障排除: `thrift/doc/troubleshoot/`

## 开发指南

### 代码风格
- C++: 遵循 Facebook C++ 代码风格
- 使用 clang-format 和 clang-tidy
- C++17 标准

### 贡献指南
参见 `CONTRIBUTING.md` 和 `CODE_OF_CONDUCT.md`

## 许可证

Apache License 2.0

## 与 Apache Thrift 的区别

1. **编译器**: 完全重写的编译器，更好的错误诊断
2. **异步服务器**: 全新的异步 C++ 服务器实现
3. **性能**: 显著的性能提升
4. **特性**: 更多高级特性（Any、Patch、流式 RPC 等）
5. **类型系统**: 增强的类型系统和反射支持

## 相关项目

- [Apache Thrift](https://thrift.apache.org/)
- [Folly](https://github.com/facebook/folly)
- [Wangle](https://github.com/facebook/wangle)
- [Fizz](https://github.com/facebookincubator/fizz)
