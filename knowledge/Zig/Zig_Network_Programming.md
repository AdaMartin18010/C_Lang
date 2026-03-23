---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |

# Zig 网络编程：TCP/UDP/HTTP 全栈

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../00_GLOBAL_INDEX.md](../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../README.md](../README.md) | 模块总览与目录导航 |
| **学习路径** | [../06_Thinking_Representation/06_Learning_Paths/README.md](../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../06_Thinking_Representation/05_Concept_Mappings/README.md](../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库  
**难度等级**: L1-L6  
**前置依赖**: 核心知识体系  
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../01_Core_Knowledge_System/02_Core_Layer/README.md](../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../03_System_Technology_Domains/README.md](../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../07_Modern_Toolchain/README.md](../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../04_Industrial_Scenarios/README.md](../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构
```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../02_Formal_Semantics_and_Physics/README.md) |

---


> **定位**: Zig 应用 / 网络编程 | **难度**: ⭐⭐⭐⭐ | **目标**: 掌握 Zig 网络开发

---

## 📋 目录

- [📋 目录](#-目录)
- [📑 目录](#-目录-1)
- [一、网络基础](#一网络基础)
- [二、TCP 编程](#二tcp-编程)
  - [2.1 TCP 服务器](#21-tcp-服务器)
  - [2.2 TCP 客户端](#22-tcp-客户端)
- [三、UDP 编程](#三udp-编程)
- [四、HTTP 客户端](#四http-客户端)
- [五、异步网络](#五异步网络)
- [六、协议实现](#六协议实现)
  - [6.1 WebSocket](#61-websocket)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)

---


---

## 📑 目录

- [📋 目录](#-目录)
- [📑 目录](#-目录-1)
- [一、网络基础](#一网络基础)
- [二、TCP 编程](#二tcp-编程)
  - [2.1 TCP 服务器](#21-tcp-服务器)
  - [2.2 TCP 客户端](#22-tcp-客户端)
- [三、UDP 编程](#三udp-编程)
- [四、HTTP 客户端](#四http-客户端)
- [五、异步网络](#五异步网络)
- [六、协议实现](#六协议实现)
  - [6.1 WebSocket](#61-websocket)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)


---

## 一、网络基础

```zig
const std = @import("std");
const net = std.net;

// 地址解析
pub fn addressExamples() !void {
    // IPv4
    const ipv4 = try net.Address.parseIp4("127.0.0.1", 8080);

    // IPv6
    const ipv6 = try net.Address.parseIp6("::1", 8080);

    // 从字符串自动识别
    const addr = try net.Address.parseIp("192.168.1.1", 80);

    _ = ipv4;
    _ = ipv6;
    _ = addr;
}
```

---

## 二、TCP 编程

### 2.1 TCP 服务器

```zig
const std = @import("std");
const net = std.net;
const Thread = std.Thread;

pub const TcpServer = struct {
    allocator: std.mem.Allocator,
    listener: net.tcp.Listener,

    pub fn init(allocator: std.mem.Allocator, port: u16) !TcpServer {
        const address = try net.Address.parseIp("0.0.0.0", port);
        var listener = try net.tcp.Listener.init(address);
        try listener.listen(128);

        return .{
            .allocator = allocator,
            .listener = listener,
        };
    }

    pub fn deinit(self: *TcpServer) void {
        self.listener.deinit();
    }

    pub fn run(self: *TcpServer) !void {
        std.log.info("Server listening on port {}", .{self.listener.address.getPort()});

        while (true) {
            const conn = try self.listener.accept();

            // 为每个连接创建线程
            const thread = try Thread.spawn(.{}, handleConnection, .{self.allocator, conn});
            thread.detach();
        }
    }
};

fn handleConnection(allocator: std.mem.Allocator, conn: net.tcp.Connection) void {
    defer conn.close();

    var buffer: [4096]u8 = undefined;

    while (true) {
        const bytes_read = conn.read(&buffer) catch |err| {
            std.log.err("Read error: {}", .{err});
            return;
        };

        if (bytes_read == 0) return;  // 连接关闭

        // 回声
        _ = conn.write(buffer[0..bytes_read]) catch |err| {
            std.log.err("Write error: {}", .{err});
            return;
        };
    }
}
```

### 2.2 TCP 客户端

```zig
const std = @import("std");
const net = std.net;

pub const TcpClient = struct {
    conn: net.tcp.Connection,

    pub fn connect(host: []const u8, port: u16) !TcpClient {
        const address = try net.Address.parseIp(host, port);
        const conn = try net.tcp.Connection.connect(address);

        return .{ .conn = conn };
    }

    pub fn close(self: *TcpClient) void {
        self.conn.close();
    }

    pub fn send(self: TcpClient, data: []const u8) !void {
        _ = try self.conn.write(data);
    }

    pub fn receive(self: TcpClient, buffer: []u8) !usize {
        return try self.conn.read(buffer);
    }
};

// 使用示例
pub fn clientExample() !void {
    var client = try TcpClient.connect("127.0.0.1", 8080);
    defer client.close();

    try client.send("Hello, Server!");

    var buffer: [1024]u8 = undefined;
    const bytes = try client.receive(&buffer);

    std.log.info("Received: {s}", .{buffer[0..bytes]});
}
```

---

## 三、UDP 编程

```zig
const std = @import("std");
const net = std.net;

pub const UdpSocket = struct {
    socket: net.udp.Socket,

    pub fn bind(port: u16) !UdpSocket {
        const address = try net.Address.parseIp("0.0.0.0", port);
        const socket = try net.udp.Socket.init(address);

        return .{ .socket = socket };
    }

    pub fn deinit(self: *UdpSocket) void {
        self.socket.deinit();
    }

    pub fn sendTo(self: UdpSocket, data: []const u8, dest: net.Address) !void {
        _ = try self.socket.sendTo(data, dest);
    }

    pub fn receiveFrom(self: UdpSocket, buffer: []u8) !struct { usize, net.Address } {
        return try self.socket.receiveFrom(buffer);
    }
};

// DNS 查询示例 (简化版)
const DNSHeader = extern struct {
    id: u16,
    flags: u16,
    questions: u16,
    answer_rrs: u16,
    authority_rrs: u16,
    additional_rrs: u16,
};

pub fn dnsQuery(domain: []const u8) !void {
    var socket = try UdpSocket.bind(0);
    defer socket.deinit();

    const dns_server = try net.Address.parseIp("8.8.8.8", 53);

    // 构建 DNS 查询包
    var query: [512]u8 = undefined;
    const header = DNSHeader{
        .id = 0x1234,
        .flags = 0x0100,  // 标准查询
        .questions = 1,
        .answer_rrs = 0,
        .authority_rrs = 0,
        .additional_rrs = 0,
    };

    @memcpy(query[0..12], std.mem.asBytes(&header));
    // 添加域名编码...

    try socket.sendTo(query[0..], dns_server);

    var response: [512]u8 = undefined;
    const len, _ = try socket.receiveFrom(&response);

    std.log.info("DNS response: {} bytes", .{len});
}
```

---

## 四、HTTP 客户端

```zig
const std = @import("std");
const net = std.net;

pub const HttpClient = struct {
    allocator: std.mem.Allocator,

    pub fn get(self: HttpClient, url: []const u8) !Response {
        // 解析 URL
        const host_start = if (std.mem.startsWith(u8, url, "http://"))
            7
        else
            return error.UnsupportedProtocol;

        const path_start = std.mem.indexOf(u8, url[host_start..], "/") orelse url.len - host_start;
        const host = url[host_start .. host_start + path_start];
        const path = if (host_start + path_start >= url.len) "/" else url[host_start + path_start ..];

        // 连接到服务器
        const port: u16 = 80;
        const address = try net.Address.parseIp(host, port);
        const conn = try net.tcp.Connection.connect(address);
        defer conn.close();

        // 发送 HTTP 请求
        var request_buffer: [4096]u8 = undefined;
        const request = try std.fmt.bufPrint(&request_buffer,
            "GET {s} HTTP/1.1\r\n" ++
            "Host: {s}\r\n" ++
            "User-Agent: ZigHTTP/1.0\r\n" ++
            "Connection: close\r\n" ++
            "\r\n",
            .{ path, host }
        );

        _ = try conn.write(request);

        // 读取响应
        var response_buffer: [8192]u8 = undefined;
        var total_read: usize = 0;

        while (true) {
            const bytes = conn.read(response_buffer[total_read..]) catch break;
            if (bytes == 0) break;
            total_read += bytes;
        }

        // 解析响应
        return try self.parseResponse(response_buffer[0..total_read]);
    }

    fn parseResponse(self: HttpClient, data: []const u8) !Response {
        // 解析 HTTP 响应
        var lines = std.mem.split(u8, data, "\r\n");

        // 状态行
        const status_line = lines.next() orelse return error.InvalidResponse;
        const status_parts = std.mem.split(u8, status_line, " ");
        _ = status_parts.next();  // HTTP/1.1
        const status_str = status_parts.next() orelse return error.InvalidResponse;
        const status = try std.fmt.parseInt(u16, status_str, 10);

        // 解析头部
        var headers = std.StringHashMap([]const u8).init(self.allocator);
        errdefer headers.deinit();

        while (lines.next()) |line| {
            if (line.len == 0) break;  // 头部结束

            const colon = std.mem.indexOf(u8, line, ": ") orelse continue;
            const key = line[0..colon];
            const value = line[colon + 2 ..];

            try headers.put(key, value);
        }

        // 获取正文
        const body_start = std.mem.indexOf(u8, data, "\r\n\r\n") orelse data.len;
        const body = data[body_start + 4 ..];

        return Response{
            .status = status,
            .headers = headers,
            .body = try self.allocator.dupe(u8, body),
            .allocator = self.allocator,
        };
    }
};

pub const Response = struct {
    status: u16,
    headers: std.StringHashMap([]const u8),
    body: []const u8,
    allocator: std.mem.Allocator,

    pub fn deinit(self: *Response) void {
        self.headers.deinit();
        self.allocator.free(self.body);
    }
};
```

---

## 五、异步网络

```zig
// 基于事件循环的异步网络 (实验性)
const std = @import("std");

pub const AsyncServer = struct {
    loop: *std.event.Loop,
    address: std.net.Address,

    pub fn init(loop: *std.event.Loop, port: u16) !AsyncServer {
        const address = try std.net.Address.parseIp("0.0.0.0", port);
        return .{
            .loop = loop,
            .address = address,
        };
    }

    pub fn run(self: AsyncServer) !void {
        var server = std.x.net.tcp.Server.init(self.address);
        defer server.deinit();
        try server.listen(128);

        while (true) {
            const conn = try await server.accept();
            const handle_frame = async self.handleConnection(conn);
            // 不等待，继续接受新连接
        }
    }

    async fn handleConnection(self: AsyncServer, conn: std.x.net.tcp.Connection) !void {
        defer conn.close();

        var buffer: [4096]u8 = undefined;
        while (true) {
            const bytes = try await conn.read(&buffer);
            if (bytes == 0) break;

            _ = try await conn.write(buffer[0..bytes]);
        }
    }
};
```

---

## 六、协议实现

### 6.1 WebSocket

```zig
const std = @import("std");
const crypto = std.crypto;

pub const WebSocket = struct {
    conn: std.net.tcp.Connection,

    pub fn handshake(allocator: std.mem.Allocator, conn: std.net.tcp.Connection, request: []const u8) !WebSocket {
        // 解析 Sec-WebSocket-Key
        const key_prefix = "Sec-WebSocket-Key: ";
        const key_start = std.mem.indexOf(u8, request, key_prefix) orelse return error.InvalidHandshake;
        const key_end = std.mem.indexOf(u8, request[key_start..], "\r\n") orelse return error.InvalidHandshake;
        const client_key = request[key_start + key_prefix.len .. key_start + key_end];

        // 计算 accept key
        const magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        var hash_input: [256]u8 = undefined;
        const hash_len = try std.fmt.bufPrint(&hash_input, "{s}{s}", .{ client_key, magic });

        var hash: [20]u8 = undefined;
        crypto.hash.Sha1.hash(hash_input[0..hash_len.len], &hash, .{});

        var accept_key: [28]u8 = undefined;
        _ = std.base64.standard.Encoder.encode(&accept_key, &hash);

        // 发送响应
        var response: [512]u8 = undefined;
        const response_str = try std.fmt.bufPrint(&response,
            "HTTP/1.1 101 Switching Protocols\r\n" ++
            "Upgrade: websocket\r\n" ++
            "Connection: Upgrade\r\n" ++
            "Sec-WebSocket-Accept: {s}\r\n" ++
            "\r\n",
            .{&accept_key}
        );

        _ = try conn.write(response_str);

        return .{ .conn = conn };
    }

    pub fn sendText(self: WebSocket, text: []const u8) !void {
        const frame = try self.buildFrame(0x1, text);
        _ = try self.conn.write(frame);
    }

    pub fn sendBinary(self: WebSocket, data: []const u8) !void {
        const frame = try self.buildFrame(0x2, data);
        _ = try self.conn.write(frame);
    }

    fn buildFrame(self: WebSocket, opcode: u8, payload: []const u8) ![]const u8 {
        _ = self;

        var frame: [65536]u8 = undefined;
        var pos: usize = 0;

        // FIN=1, opcode
        frame[pos] = 0x80 | opcode;
        pos += 1;

        // Mask=0, payload length
        if (payload.len < 126) {
            frame[pos] = @intCast(payload.len);
            pos += 1;
        } else if (payload.len < 65536) {
            frame[pos] = 126;
            pos += 1;
            frame[pos] = @intCast(payload.len >> 8);
            frame[pos + 1] = @intCast(payload.len & 0xFF);
            pos += 2;
        } else {
            frame[pos] = 127;
            pos += 1;
            // 8 字节长度
            const len = payload.len;
            std.mem.writeIntBig(u64, frame[pos..][0..8], len);
            pos += 8;
        }

        // 复制 payload
        @memcpy(frame[pos..][0..payload.len], payload);
        pos += payload.len;

        return frame[0..pos];
    }

    pub fn close(self: WebSocket) void {
        _ = self.conn.write(&[_]u8{ 0x88, 0x00 }) catch {};
        self.conn.close();
    }
};
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
