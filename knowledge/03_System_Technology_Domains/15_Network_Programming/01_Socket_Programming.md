# Socket网络编程 - 工程级深度手册

> **层级定位**: 03 System Technology Domains / 15 Network_Programming
> **对应标准**: POSIX.1-2008, BSD Socket API, RFC 793 (TCP), RFC 768 (UDP)
> **难度级别**: L3-L5
> **预估学习时间**: 20-30小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | Socket API、TCP/UDP、I/O多路复用、并发服务器、零拷贝、高并发架构 |
| **前置知识** | C基础、进程/线程、操作系统原理、网络协议基础 |
| **后续延伸** | HTTP协议、网络安全、高性能网络、分布式系统 |
| **权威来源** | POSIX, Stevens《Unix Network Programming》, RFC标准 |

---


---

## 📑 目录

- [Socket网络编程 - 工程级深度手册](#socket网络编程---工程级深度手册)
  - [📋 本节概要](#-本节概要)
  - [📑 目录](#-目录)
  - [📖 1. 概念定义](#-1-概念定义)
    - [1.1 Socket的严格定义](#11-socket的严格定义)
    - [1.2 网络协议栈层次模型](#12-网络协议栈层次模型)
    - [1.3 TCP连接状态机](#13-tcp连接状态机)
    - [1.4 端口和地址的数学表示](#14-端口和地址的数学表示)
  - [📊 2. 属性维度矩阵](#-2-属性维度矩阵)
    - [2.1 Socket类型对比矩阵](#21-socket类型对比矩阵)
    - [2.2 Socket选项矩阵](#22-socket选项矩阵)
      - [SOL\_SOCKET级别选项](#sol_socket级别选项)
      - [IPPROTO\_TCP级别选项](#ipproto_tcp级别选项)
      - [IPPROTO\_IP级别选项](#ipproto_ip级别选项)
    - [2.3 IO模型对比矩阵](#23-io模型对比矩阵)
    - [2.4 地址族对比矩阵](#24-地址族对比矩阵)
    - [2.5 并发模型对比矩阵](#25-并发模型对比矩阵)
    - [2.6 错误码矩阵](#26-错误码矩阵)
      - [Socket API错误码分类](#socket-api错误码分类)
      - [详细错误码对照表](#详细错误码对照表)
  - [🔄 3. 形式化描述](#-3-形式化描述)
    - [3.1 TCP状态转换图(详细)](#31-tcp状态转换图详细)
    - [3.2 Socket API状态机](#32-socket-api状态机)
    - [3.3 TCP连接建立与终止时序](#33-tcp连接建立与终止时序)
      - [三次握手(连接建立)](#三次握手连接建立)
      - [四次挥手(连接终止)](#四次挥手连接终止)
  - [💻 4. 示例矩阵](#-4-示例矩阵)
    - [4.1 基本TCP客户端/服务器](#41-基本tcp客户端服务器)
      - [TCP客户端(完整版)](#tcp客户端完整版)
      - [TCP服务器(迭代版)](#tcp服务器迭代版)
    - [4.2 基本UDP客户端/服务器](#42-基本udp客户端服务器)
    - [4.3 并发服务器实现](#43-并发服务器实现)
      - [多进程并发服务器](#多进程并发服务器)
      - [多线程并发服务器](#多线程并发服务器)
      - [select多路复用服务器](#select多路复用服务器)
      - [epoll服务器(LT/ET模式)](#epoll服务器ltet模式)
    - [4.4 高并发服务器架构](#44-高并发服务器架构)
      - [Reactor模式实现](#reactor模式实现)
    - [4.5 协议解析示例](#45-协议解析示例)
      - [简单HTTP服务器](#简单http服务器)
    - [4.6 非阻塞IO实现](#46-非阻塞io实现)
    - [4.7 零拷贝传输](#47-零拷贝传输)
  - [⚠️ 5. 反例与陷阱](#️-5-反例与陷阱)
    - [5.1 常见陷阱列表](#51-常见陷阱列表)
      - [陷阱01: 忘记处理SIGPIPE](#陷阱01-忘记处理sigpipe)
      - [陷阱02: 缓冲区溢出](#陷阱02-缓冲区溢出)
      - [陷阱03: 粘包问题](#陷阱03-粘包问题)
      - [陷阱04: 字节序问题](#陷阱04-字节序问题)
      - [陷阱05: 资源泄漏(忘记close)](#陷阱05-资源泄漏忘记close)
      - [陷阱06: 僵尸进程(未处理SIGCHLD)](#陷阱06-僵尸进程未处理sigchld)
      - [陷阱07: TIME\_WAIT累积](#陷阱07-time_wait累积)
      - [陷阱08: 惊群问题(Thundering Herd)](#陷阱08-惊群问题thundering-herd)
      - [陷阱09: 半开连接(Half-Open Connection)](#陷阱09-半开连接half-open-connection)
      - [陷阱10: 拒绝服务攻击脆弱性](#陷阱10-拒绝服务攻击脆弱性)
      - [陷阱11: 不正确的错误处理](#陷阱11-不正确的错误处理)
      - [陷阱12: 线程安全问题](#陷阱12-线程安全问题)
  - [🧠 6. 思维导图](#-6-思维导图)
    - [6.1 Socket编程全景图](#61-socket编程全景图)
    - [6.2 IO模型选择图](#62-io模型选择图)
    - [6.3 服务器架构演进图](#63-服务器架构演进图)
  - [🌲 7. 决策树](#-7-决策树)
    - [7.1 Socket类型选择决策树](#71-socket类型选择决策树)
    - [7.2 IO模型选择决策树](#72-io模型选择决策树)
    - [7.3 并发策略选择决策树](#73-并发策略选择决策树)
  - [⚡ 8. 性能优化](#-8-性能优化)
    - [8.1 TCP调优参数](#81-tcp调优参数)
      - [连接建立优化](#连接建立优化)
      - [连接维护优化](#连接维护优化)
      - [传输优化](#传输优化)
      - [代码设置示例](#代码设置示例)
    - [8.2 高并发优化技巧](#82-高并发优化技巧)
      - [系统级优化](#系统级优化)
      - [应用级优化](#应用级优化)
      - [性能测试与监控](#性能测试与监控)
    - [8.3 内核参数优化速查表](#83-内核参数优化速查表)
  - [✅ 质量验收清单](#-质量验收清单)


---

## 📖 1. 概念定义

### 1.1 Socket的严格定义

**定义**: Socket是操作系统内核提供的网络通信端点抽象，是进程间通过网络进行双向数据交换的编程接口。

**形式化定义**:

```
Socket := (Protocol, LocalAddr, LocalPort, RemoteAddr, RemotePort, State)

其中:
- Protocol ∈ {TCP, UDP, RAW, ...}
- LocalAddr, RemoteAddr ∈ IPv4Address ∪ IPv6Address ∪ UnixPath
- LocalPort, RemotePort ∈ [0, 65535]
- State ∈ {CLOSED, LISTEN, SYN_SENT, SYN_RCVD, ESTABLISHED, ...}
```

**核心语义**:

| 概念 | 说明 | 类比 |
|:-----|:-----|:-----|
| **端点(Endpoint)** | 通信的一端，标识为(IP, Port)对 | 电话号码 |
| **套接字描述符** | 内核对象的文件描述符引用 | 文件句柄 |
| **五元组** | (协议, 源IP, 源端口, 目的IP, 目的端口) | 通信契约 |

### 1.2 网络协议栈层次模型

```
┌─────────────────────────────────────────────────────────────┐
│  应用层 (Application)    HTTP/FTP/SMTP/DNS...               │
├─────────────────────────────────────────────────────────────┤
│  传输层 (Transport)      TCP/UDP/SCTP                       │
│  ├─ Socket API ←─────────────────────────────────────┐      │
│  │   • socket()  - 创建通信端点                      │      │
│  │   • bind()    - 绑定本地地址                      │      │
│  │   • listen()  - 监听连接请求                      │      │
│  │   • accept()  - 接受新连接                        │      │
│  │   • connect() - 发起连接                          │      │
│  │   • send/recv - 数据传输                          │      │
│  └────────────────────────────────────────────────────┘      │
├─────────────────────────────────────────────────────────────┤
│  网络层 (Network)        IP/ICMP/IGMP                       │
├─────────────────────────────────────────────────────────────┤
│  链路层 (Link)           Ethernet/WiFi/ARP                  │
├─────────────────────────────────────────────────────────────┤
│  物理层 (Physical)       电缆/光纤/无线信号                 │
└─────────────────────────────────────────────────────────────┘
```

**Socket在协议栈中的位置**:

- 位于**传输层与应用层之间**
- 为应用层提供统一的网络编程接口
- 屏蔽底层协议差异

### 1.3 TCP连接状态机

**RFC 793定义的TCP状态机**:

```
                              ┌──────────┐
                    主动打开  │          │  被动打开
                   ┌─────────│  CLOSED  │◄────────┐
                   │         │          │         │
                   │         └────┬─────┘         │
                   │              │               │
                   ▼              │               ▼
            ┌────────────┐        │        ┌────────────┐
            │  SYN_SENT  │◄───────┴───────►│   LISTEN   │
            │ (同步已发送)│  同时打开        │  (监听)    │
            └─────┬──────┘                 └──────┬─────┘
                  │                               │
          收到SYN+ACK                       收到SYN
                  │                               │
                  ▼                               ▼
            ┌────────────┐                 ┌────────────┐
            │ SYN_RCVD   │◄────────────────┤ SYN_RCVD   │
            │(同步已接收)│    同时打开       │(同步已接收)│
            └─────┬──────┘                 └─────┬──────┘
                  │                               │
          收到ACK                               发送SYN+ACK
                  │                               │
                  ▼                               ▼
            ┌──────────────────────────────────────────┐
            │           ESTABLISHED                    │
            │           (连接已建立)                    │
            │   ◄────────────── 数据传输 ────────────► │
            └──────────────────┬───────────────────────┘
                               │
              ┌────────────────┼────────────────┐
              ▼                ▼                ▼
        ┌─────────┐     ┌─────────┐      ┌─────────┐
        │FIN_WAIT_1│    │CLOSE_WAIT│     │CLOSING  │
        └────┬────┘     └────┬────┘     └────┬────┘
             │               │               │
        收到FIN+ACK      应用关闭          收到FIN
             │               │               │
             ▼               ▼               ▼
        ┌─────────┐     ┌─────────┐      ┌─────────┐
        │FIN_WAIT_2│    │LAST_ACK │      │TIME_WAIT│
        └────┬────┘     └────┬────┘      └────┬────┘
             │               │               │
        收到FIN          收到ACK          2MSL超时
             │               │               │
             ▼               ▼               ▼
        ┌─────────┐     ┌─────────┐      ┌─────────┐
        │TIME_WAIT│     │ CLOSED  │      │ CLOSED  │
        └────┬────┘     └─────────┘      └─────────┘
             │
        2MSL超时
             │
             ▼
        ┌─────────┐
        │ CLOSED  │
        └─────────┘
```

**状态说明**:

| 状态 | 含义 | 持续时间 |
|:-----|:-----|:---------|
| **CLOSED** | 连接关闭/未建立 | - |
| **LISTEN** | 服务器等待连接 | 直到服务器关闭 |
| **SYN_SENT** | 已发送SYN，等待ACK | 约1个RTT |
| **SYN_RCVD** | 收到SYN，已发送SYN+ACK | 约1个RTT |
| **ESTABLISHED** | 连接建立，可传输数据 | 直到连接关闭 |
| **FIN_WAIT_1** | 已发送FIN，等待ACK | 约1个RTT |
| **FIN_WAIT_2** | 收到ACK，等待对方FIN | 不定 |
| **CLOSE_WAIT** | 收到FIN，等待应用关闭 | 取决于应用 |
| **CLOSING** | 双方同时关闭 | 约1个RTT |
| **LAST_ACK** | 已发送FIN，等待最后ACK | 约1个RTT |
| **TIME_WAIT** | 等待2MSL确保ACK到达 | 2×MSL (通常60s) |

### 1.4 端口和地址的数学表示

**IPv4地址表示**:

```
IPv4Address := [0, 255]⁴  (4字节，32位无符号整数)

示例: 192.168.1.1 = 0xC0A80101 = 3232235777

struct in_addr {
    uint32_t s_addr;  // 网络字节序大端存储
};
```

**IPv6地址表示**:

```
IPv6Address := [0, 65535]⁸  (16字节，128位)

struct in6_addr {
    uint8_t s6_addr[16];  // 16字节地址
};
```

**端口表示**:

```
Port := [0, 65535]  (16位无符号整数)

知名端口: [0, 1023]    - 需要root权限
注册端口: [1024, 49151] - IANA注册
动态端口: [49152, 65535] - 临时使用
```

**套接字地址结构**:

```c
// 通用地址结构
struct sockaddr {
    sa_family_t sa_family;    // 地址族 AF_INET/AF_INET6/AF_UNIX
    char        sa_data[14];  // 地址数据
};

// IPv4地址结构
struct sockaddr_in {
    sa_family_t    sin_family; // AF_INET
    in_port_t      sin_port;   // 端口号(网络字节序)
    struct in_addr sin_addr;   // IPv4地址
    char           sin_zero[8];// 填充
};

// IPv6地址结构
struct sockaddr_in6 {
    sa_family_t     sin6_family;   // AF_INET6
    in_port_t       sin6_port;     // 端口号
    uint32_t        sin6_flowinfo; // 流信息
    struct in6_addr sin6_addr;     // IPv6地址
    uint32_t        sin6_scope_id; // 作用域ID
};

// Unix域地址结构
struct sockaddr_un {
    sa_family_t sun_family; // AF_UNIX/AF_LOCAL
    char        sun_path[108]; // 路径名
};
```

---

## 📊 2. 属性维度矩阵

### 2.1 Socket类型对比矩阵

| 特性 | TCP (SOCK_STREAM) | UDP (SOCK_DGRAM) | RAW (SOCK_RAW) | UNIX (SOCK_STREAM) |
|:-----|:-----------------|:-----------------|:---------------|:-------------------|
| **传输特性** | 面向连接、可靠 | 无连接、不可靠 | 原始协议访问 | 本地进程间通信 |
| **数据边界** | 字节流，无边界 | 保留报文边界 | 依协议而定 | 字节流 |
| **可靠性** | 可靠(确认、重传、排序) | 尽力而为 | 依协议而定 | 可靠 |
| **拥塞控制** | 有 | 无 | 无 | 无 |
| **数据校验** | 有(校验和+确认) | 有(仅校验和) | 用户处理 | 有 |
| **头部开销** | 20字节 | 8字节 | 依协议而定 | 无 |
| **连接建立** | 三次握手 | 无 | 无 | 无/可连接 |
| **适用场景** | HTTP/SSH/FTP | DNS/视频流/游戏 | Ping/OSPF | 本地IPC |
| **API复杂度** | 中等 | 简单 | 复杂 | 简单 |
| **最大数据包** | 无限制(流) | 65507字节(IPv4) | 依协议 | 无限制 |
| **广播/多播** | ❌ | ✅ | ✅ | ❌ |

### 2.2 Socket选项矩阵

#### SOL_SOCKET级别选项

| 选项名 | 类型 | 默认值 | 描述 | 适用Socket |
|:-------|:-----|:-------|:-----|:-----------|
| **SO_REUSEADDR** | int | 0 | 允许重用本地地址 | 所有 |
| **SO_REUSEPORT** | int | 0 | 允许多个进程绑定同一端口 | TCP/UDP |
| **SO_KEEPALIVE** | int | 0 | 周期性检测连接存活 | TCP |
| **SO_LINGER** | struct linger | off | 关闭时的逗留行为 | TCP |
| **SO_SNDTIMEO** | struct timeval | 0 | 发送超时时间 | 所有 |
| **SO_RCVTIMEO** | struct timeval | 0 | 接收超时时间 | 所有 |
| **SO_SNDLOWAT** | int | 1 | 发送低水位标记 | 所有 |
| **SO_RCVLOWAT** | int | 1 | 接收低水位标记 | 所有 |
| **SO_SNDBUF** | int | 系统默认 | 发送缓冲区大小 | 所有 |
| **SO_RCVBUF** | int | 系统默认 | 接收缓冲区大小 | 所有 |
| **SO_ERROR** | int | 0 | 获取并清除待处理错误 | 所有 |
| **SO_TYPE** | int | - | 获取Socket类型 | 所有 |
| **SO_OOBINLINE** | int | 0 | 带外数据内联接收 | TCP |
| **SO_BROADCAST** | int | 0 | 允许发送广播数据 | UDP |
| **SO_DONTROUTE** | int | 0 | 绕过路由表 | 所有 |
| **SO_TIMESTAMP** | int | 0 | 接收时间戳 | 所有 |

#### IPPROTO_TCP级别选项

| 选项名 | 类型 | 默认值 | 描述 |
|:-------|:-----|:-------|:-----|
| **TCP_NODELAY** | int | 0 | 禁用Nagle算法 |
| **TCP_MAXSEG** | int | 536 | 最大段大小(MSS) |
| **TCP_CORK** | int | 0 | 合并小数据包 |
| **TCP_QUICKACK** | int | 0 | 禁用延迟ACK |
| **TCP_KEEPIDLE** | int | 7200 | 首次保活探测前空闲时间(秒) |
| **TCP_KEEPINTVL** | int | 75 | 保活探测间隔(秒) |
| **TCP_KEEPCNT** | int | 9 | 保活探测次数 |
| **TCP_SYNCNT** | int | 5 | SYN重传次数 |
| **TCP_DEFER_ACCEPT** | int | 0 | 延迟accept到数据到达 |
| **TCP_WINDOW_CLAMP** | int | 无 | 限制通告窗口大小 |

#### IPPROTO_IP级别选项

| 选项名 | 类型 | 默认值 | 描述 |
|:-------|:-----|:-------|:-----|
| **IP_TOS** | int | 0 | 服务类型(DSCP) |
| **IP_TTL** | int | 64 | 生存时间 |
| **IP_HDRINCL** | int | 0 | 用户构造IP头(RAW) |
| **IP_OPTIONS** | char[] | 无 | IP选项 |
| **IP_MULTICAST_TTL** | int | 1 | 多播TTL |
| **IP_MULTICAST_LOOP** | int | 1 | 多播回环 |
| **IP_ADD_MEMBERSHIP** | struct ip_mreq | - | 加入多播组 |
| **IP_DROP_MEMBERSHIP** | struct ip_mreq | - | 离开多播组 |
| **IP_BIND_ADDRESS_NO_PORT** | int | 0 | 绑定时不分配临时端口 |

### 2.3 IO模型对比矩阵

| IO模型 | 阻塞调用 | 多路复用 | 数据拷贝次数 | 并发能力 | CPU效率 | 复杂度 | 适用场景 |
|:-------|:---------|:---------|:-------------|:---------|:--------|:-------|:---------|
| **阻塞IO** | ✅ | ❌ | 2 | 低(每连接1线程) | 低 | 低 | 低并发、简单应用 |
| **非阻塞IO** | ❌ | 忙轮询 | 2 | 中 | 低 | 中 | 特定场景 |
| **IO多路复用** | select/poll/epoll | ✅ | 2 | 高(单线程多连接) | 高 | 中 | 高并发服务器 |
| **信号驱动IO** | ❌ | 信号通知 | 2 | 中 | 中 | 高 | 特定Unix系统 |
| **异步IO** | ❌ | 事件回调 | 2 | 极高 | 极高 | 高 | 超高并发 |
| **内存映射IO** | - | - | 0(零拷贝) | 高 | 极高 | 高 | 大文件传输 |

**IO模型详细对比**:

```
┌──────────────────────────────────────────────────────────────────────────┐
│                        IO模型对比图                                       │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  阻塞IO                    非阻塞IO                 IO多路复用           │
│  ┌──────┐                  ┌──────┐                 ┌──────────┐         │
│  │ 应用 │                  │ 应用 │                 │   应用   │         │
│  │ 阻塞 │                  │ 轮询 │                 │ 等待事件 │         │
│  │ 等待 │                  │ 检查 │                 │          │         │
│  └──┬───┘                  └──┬───┘                 └────┬─────┘         │
│     │ 数据就绪                │ 数据未就绪               │ 多个FD就绪    │
│     ▼                         ▼                        ▼               │
│  ┌─────────┐              ┌─────────┐              ┌──────────┐         │
│  │  内核   │              │  内核   │              │   内核   │         │
│  │ 拷贝数据│              │ 立即返回│              │ 通知就绪 │         │
│  │ 阻塞   │               │ EAGAIN │               │          │         │
│  └─────────┘              └─────────┘              └──────────┘         │
│                                                                          │
│  信号驱动IO                异步IO                    零拷贝              │
│  ┌──────────┐             ┌──────────┐             ┌──────────┐         │
│  │   应用   │             │   应用   │             │   应用   │         │
│  │ 继续执行 │             │ 提交请求 │             │ 建立映射 │         │
│  │          │             │ 继续执行 │             │          │         │
│  └────┬─────┘             └────┬─────┘             └────┬─────┘         │
│       │ SIGIO信号               │ 完成回调               │ DMA传输      │
│       ▼                         ▼                        ▼               │
│  ┌──────────┐             ┌──────────┐             ┌──────────┐         │
│  │   内核   │             │   内核   │             │   内核   │         │
│  │ 通知就绪 │             │ 全程处理 │             │  不拷贝  │         │
│  └──────────┘             └──────────┘             └──────────┘         │
│                                                                          │
└──────────────────────────────────────────────────────────────────────────┘
```

### 2.4 地址族对比矩阵

| 特性 | AF_INET (IPv4) | AF_INET6 (IPv6) | AF_UNIX (本地) | AF_PACKET (链路) |
|:-----|:---------------|:----------------|:---------------|:-----------------|
| **地址长度** | 4字节 | 16字节 | 108字节 | 可变 |
| **地址表示** | 点分十进制 | 冒号十六进制 | 文件路径 | MAC地址 |
| **回环地址** | 127.0.0.1 | ::1 | 抽象命名空间 | N/A |
| **广播支持** | ✅ | ❌(用多播) | ❌ | ✅ |
| **多播支持** | ✅ | ✅ | ❌ | ✅ |
| **最大连接** | ~65535端口 | ~65535端口 | 无限制 | N/A |
| **NAT支持** | 常用 | 设计避免 | N/A | N/A |
| **API兼容性** | 所有系统 | 现代系统 | Unix/Linux | Linux |
| **结构体** | sockaddr_in | sockaddr_in6 | sockaddr_un | sockaddr_ll |
| **地址转换函数** | inet_pton/ntop | inet_pton/ntop | 直接字符串 | 手动转换 |

### 2.5 并发模型对比矩阵

| 并发模型 | 连接数限制 | 上下文切换 | 内存占用 | 编程复杂度 | 适用QPS | 代表应用 |
|:---------|:-----------|:-----------|:---------|:-----------|:--------|:---------|
| **迭代服务器** | 1 | 无 | 最低 | 最低 | <1 | 测试程序 |
| **多进程(prefork)** | 进程数 | 高 | 高 | 低 | 1k-10k | Apache Prefork |
| **多线程** | 线程数 | 高 | 较高 | 中 | 1k-10k | Apache Worker |
| **select** | FD_SETSIZE(1024) | 低 | 低 | 中 | 1k-5k | 早期服务器 |
| **poll** | 无限制 | 低 | 低 | 中 | 5k-20k | 通用服务器 |
| **epoll(LT)** | 无限制 | 极低 | 低 | 中 | 100k+ | Nginx/Redis |
| **epoll(ET)** | 无限制 | 极低 | 低 | 高 | 200k+ | 高性能服务器 |
| **io_uring** | 无限制 | 极低 | 低 | 高 | 500k+ | 下一代Linux服务器 |
| **Reactor** | 无限制 | 极低 | 低 | 高 | 100k+ | Netty/libevent |
| **Proactor** | 无限制 | 极低 | 低 | 极高 | 200k+ | Windows IOCP |

**并发模型架构图**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      并发模型架构对比                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  多进程模型                    多线程模型                Reactor模型    │
│  ┌────────────────┐           ┌────────────────┐      ┌───────────────┐ │
│  │   主进程       │           │   主线程       │      │   事件循环    │ │
│  │  accept()      │           │  accept()      │      │  epoll_wait() │ │
│  └───────┬────────┘           └───────┬────────┘      └───────┬───────┘ │
│          │ fork()                     │ pthread_create          │       │
│          ▼                            ▼                       ▼       │
│  ┌───────┴───────┐           ┌────────┴───────┐      ┌────────┴──────┐ │
│  │ 子进程1       │           │  工作线程1     │      │  事件处理器   │ │
│  │ 处理连接1     │           │  处理连接1     │      │  (读/写处理)  │ │
│  ├───────────────┤           ├────────────────┤      ├───────────────┤ │
│  │ 子进程2       │           │  工作线程2     │      │  事件处理器   │ │
│  │ 处理连接2     │           │  处理连接2     │      │  (非阻塞)     │ │
│  ├───────────────┤           ├────────────────┤      ├───────────────┤ │
│  │ 子进程N       │           │  工作线程N     │      │  事件处理器   │ │
│  │ 处理连接N     │           │  处理连接N     │      │  (回调驱动)   │ │
│  └───────────────┘           └────────────────┘      └───────────────┘ │
│                                                                         │
│  特点: 进程隔离                特点: 共享内存            特点: 单线程    │
│  开销: 高(fork+IPC)            开销: 中(同步开销)        开销: 极低      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.6 错误码矩阵

#### Socket API错误码分类

| 错误类别 | 错误码 | 触发场景 | 处理策略 |
|:---------|:-------|:---------|:---------|
| **连接错误** | ECONNREFUSED | 目标端口未监听 | 检查服务状态 |
| | ETIMEDOUT | 连接超时 | 检查网络/重试 |
| | ECONNRESET | 连接被重置 | 关闭Socket重连 |
| | EPIPE | 写入已关闭连接 | 忽略SIGPIPE/检查 |
| **地址错误** | EADDRINUSE | 地址已被使用 | 设置SO_REUSEADDR |
| | EADDRNOTAVAIL | 地址不可用 | 检查IP配置 |
| | ENETUNREACH | 网络不可达 | 检查路由配置 |
| | EHOSTUNREACH | 主机不可达 | 检查目标主机 |
| **资源错误** | EMFILE | 进程FD达到上限 | 增加ulimit/关闭FD |
| | ENFILE | 系统FD达到上限 | 系统调优 |
| | ENOBUFS/ENOMEN | 内存不足 | 减少并发/增加内存 |
| **IO错误** | EAGAIN/EWOULDBLOCK | 非阻塞IO暂不可 | 重试/等待事件 |
| | EINTR | 被信号中断 | 重试操作 |
| | EMSGSIZE | 消息太大 | 分片发送 |

#### 详细错误码对照表

| errno | 宏定义 | 数值 | 描述 | 常见原因 |
|:------|:-------|:-----|:-----|:---------|
| 1 | EPERM | 1 | 操作不允许 | 权限不足 |
| 2 | ENOENT | 2 | 没有该文件/目录 | 路径错误 |
| 4 | EINTR | 4 | 系统调用被中断 | 信号中断 |
| 9 | EBADF | 9 | 错误的文件描述符 | FD已关闭/无效 |
| 11 | EAGAIN | 11 | 资源暂时不可用 | 非阻塞IO繁忙 |
| 12 | ENOMEM | 12 | 内存不足 | 系统资源耗尽 |
| 13 | EACCES | 13 | 权限拒绝 | 端口<1024需root |
| 14 | EFAULT | 14 | 错误的地址 | 指针无效 |
| 22 | EINVAL | 22 | 无效的参数 | 参数错误 |
| 24 | EMFILE | 24 | 打开文件过多(进程) | FD达到上限 |
| 23 | ENFILE | 23 | 打开文件过多(系统) | 系统限制 |
| 98 | EADDRINUSE | 98 | 地址已被使用 | 端口被占用 |
| 99 | EADDRNOTAVAIL | 99 | 地址不可用 | IP配置错误 |
| 101 | ENETUNREACH | 101 | 网络不可达 | 路由问题 |
| 104 | ECONNRESET | 104 | 连接被重置 | 对端强制关闭 |
| 110 | ETIMEDOUT | 110 | 连接超时 | 网络延迟/丢包 |
| 111 | ECONNREFUSED | 111 | 连接被拒绝 | 服务未启动 |
| 112 | EHOSTDOWN | 112 | 主机宕机 | 目标主机故障 |
| 113 | EHOSTUNREACH | 113 | 主机不可达 | 路由/防火墙 |

---

## 🔄 3. 形式化描述

### 3.1 TCP状态转换图(详细)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                              TCP状态机详图                                       │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│   [CLOSED]                                                                      │
│      │                                                                          │
│      │ 应用:被动打开(PASSIVE OPEN)                                              │
│      │ 动作:socket(),bind(),listen()                                           │
│      ▼                                                                          │
│   [LISTEN] ─────────────────────────┬───────────────────────────────────────┐   │
│      │                              │                                       │   │
│      │ 收到SYN                      │ 应用:主动打开(ACTIVE OPEN)            │   │
│      │ 动作:发送SYN+ACK             │ 动作:socket(),connect()               │   │
│      ▼                              ▼                                       │   │
│   [SYN_RCVD]◄─────────────────── [SYN_SENT]                                 │   │
│      │  收到SYN(同时打开)              │                                      │   │
│      │ 收到ACK                        │ 收到SYN+ACK                          │   │
│      ▼                               ▼                                       │   │
│   [ESTABLISHED]◄──────────────────[ESTABLISHED]                              │   │
│      ▲                               ▲                                       │   │
│      │                               │                                       │   │
│      └────────── 数据传输 ───────────┘                                       │   │
│      │                                                                        │   │
│      │ 应用:close() / 收到FIN                                                 │   │
│      │                                                                        │   │
│      ├───► [FIN_WAIT_1] ──收到ACK──► [FIN_WAIT_2] ──收到FIN──► [TIME_WAIT]   │   │
│      │         │                      ▲                              │        │   │
│      │         │ 收到FIN+ACK          │                              │ 2MSL   │   │
│      │         └──────────► [CLOSING]─┘                              ▼        │   │
│      │                                                                [CLOSED]│   │
│      │                                                                        │   │
│      ├───► [CLOSE_WAIT] ──应用close()──► [LAST_ACK] ──收到ACK──► [CLOSED]    │   │
│      │                                                                        │   │
│      └────────────────────────────────────────────────────────────────────────┘   │
│                                                                                 │
│   状态转换条件说明:                                                              │
│   ───────────────────────────────────────────────────────────────────────────  │
│   • 粗线: 正常客户端路径                                                         │
│   • 细线: 正常服务器路径                                                         │
│   • 虚线: 异常/同时关闭路径                                                      │
│   • [TIME_WAIT] 等待2MSL(Maximum Segment Lifetime)                              │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 Socket API状态机

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                           Socket API状态机                                       │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│   socket()                                                                      │
│      │                                                                          │
│      ▼                                                                          │
│   ┌─────────────┐     bind()      ┌─────────────┐     listen()   ┌───────────┐ │
│   │   CREATED   │ ───────────────▶│    BOUND    │ ─────────────▶│ LISTENING │ │
│   │  (已创建)   │                 │  (已绑定)   │               │  (监听中) │ │
│   └──────┬──────┘                 └──────┬──────┘               └─────┬─────┘ │
│          │                               │                          │       │
│          │ connect()                      │ accept()                 │       │
│          │ (客户端)                       │ (服务器)                 │       │
│          ▼                               ▼                          │       │
│   ┌─────────────┐                 ┌─────────────┐                   │       │
│   │  CONNECTING │ ───────────────▶│ CONNECTED   │◄──────────────────┘       │
│   │  (连接中)   │   连接成功      │  (已连接)   │                           │
│   └─────────────┘                 └──────┬──────┘                           │
│                                          │                                   │
│              ┌───────────────────────────┼───────────────────────────┐       │
│              │                           │                           │       │
│              ▼                           ▼                           ▼       │
│        ┌──────────┐               ┌──────────┐                 ┌──────────┐ │
│        │ send()   │               │ recv()   │                 │ close()  │ │
│        │ 发送数据 │               │ 接收数据 │                 │ 关闭连接 │ │
│        └──────────┘               └──────────┘                 └────┬─────┘ │
│                                                                     │       │
│                                                                     ▼       │
│                                                               ┌──────────┐  │
│                                                               │  CLOSED  │  │
│                                                               │  (已关闭)│  │
│                                                               └──────────┘  │
│                                                                               │
│   状态转换规则:                                                                │
│   ─────────────────────────────────────────────────────────────────────────  │
│   1. socket()  → 创建Socket描述符, 状态CREATED                                │
│   2. bind()    → 绑定本地地址, 状态BOUND (服务器必需, 客户端可选)              │
│   3. listen()  → 设置监听队列, 状态LISTENING (仅服务器流Socket)                │
│   4. connect() → 发起连接, 状态CONNECTING→CONNECTED                          │
│   5. accept()  → 接受连接, 返回CONNECTED状态的Socket                          │
│   6. send()    → 在CONNECTED状态发送数据                                       │
│   7. recv()    → 在CONNECTED状态接收数据                                       │
│   8. close()   → 关闭Socket, 状态CLOSED                                       │
│                                                                               │
└───────────────────────────────────────────────────────────────────────────────┘
```

### 3.3 TCP连接建立与终止时序

#### 三次握手(连接建立)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                          TCP三次握手时序                                         │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│   客户端                              服务器                                    │
│   ────────                            ────────                                  │
│                                                                                 │
│      │                                    │                                     │
│      │────────── SYN, seq=x ─────────────►│                                     │
│      │        (SYN_SENT状态)              │                                     │
│      │                                    │────┐                                │
│      │                                    │    │ 创建控制块(TCB)                 │
│      │                                    │    │ 分配资源                        │
│      │                                    │◄───┘                                │
│      │                                    │ (SYN_RCVD状态)                        │
│      │◄───────── SYN+ACK, seq=y, ack=x+1──│                                     │
│      │────┐                               │                                     │
│      │    │ 分配资源                      │                                     │
│      │◄───┘                               │                                     │
│      │ (ESTABLISHED)                      │                                     │
│      │────────── ACK, ack=y+1 ───────────►│                                     │
│      │                                    │ (ESTABLISHED)                         │
│      │                                    │                                     │
│      │◄══════════ 数据传输 ══════════════►│                                     │
│      │                                    │                                     │
│                                                                                 │
│   关键说明:                                                                      │
│   ───────────────────────────────────────────────────────────────────────────  │
│   • SYN标志消耗一个序列号(seq), 因此ack = seq + 1                              │
│   • 第三次握手可以携带数据(但在某些实现中默认不携带)                             │
│   • 握手过程中分配的资源: 发送/接收缓冲区, 定时器等                              │
│   • 未完成连接队列(syn queue)和已完成连接队列(accept queue)                      │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

#### 四次挥手(连接终止)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                          TCP四次挥手时序                                         │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│   主动关闭方                          被动关闭方                                │
│   ──────────                          ──────────                                │
│                                                                                 │
│      │                                    │                                     │
│      │ (应用close())                      │                                     │
│      │────────── FIN, seq=u ─────────────►│                                     │
│      │ (FIN_WAIT_1)                       │                                     │
│      │                                    │────┐                                │
│      │                                    │    │ 应用收到EOF                      │
│      │                                    │◄───┘                                │
│      │                                    │ (CLOSE_WAIT)                         │
│      │◄────────── ACK, ack=u+1 ───────────│                                     │
│      │ (FIN_WAIT_2)                       │                                     │
│      │                                    │                                     │
│      │                                    │ ...                                 │
│      │                                    │ (应用处理完数据后close())            │
│      │                                    │                                     │
│      │◄────────── FIN, seq=w ─────────────│                                     │
│      │                                    │ (LAST_ACK)                          │
│      │────────── ACK, ack=w+1 ───────────►│                                     │
│      │ (TIME_WAIT)                        │────┐                                │
│      │                                    │    │ 收到ACK, 关闭连接                │
│      │                                    │◄───┘                                │
│      │                                    │ (CLOSED)                            │
│      │ ...                                 │                                     │
│      │ (等待2MSL)                          │                                     │
│      │                                    │                                     │
│      │ (CLOSED)                           │                                     │
│                                                                                 │
│   关键说明:                                                                      │
│   ───────────────────────────────────────────────────────────────────────────  │
│   • 为什么需要四次: TCP全双工, 每个方向需要单独关闭                              │
│   • TIME_WAIT作用: 1)确保最后的ACK到达 2)防止旧连接数据包干扰新连接              │
│   • 2MSL时间: 通常2分钟, 可配置/proc/sys/net/ipv4/tcp_fin_timeout              │
│   • 同时关闭: 双方同时发送FIN, 都进入CLOSING状态, 收到ACK后进入TIME_WAIT         │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 💻 4. 示例矩阵

### 4.1 基本TCP客户端/服务器

#### TCP客户端(完整版)

```c
/**
 * tcp_client_full.c - 完整的TCP客户端实现
 * 包含: 错误处理、超时设置、优雅关闭
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096
#define CONNECT_TIMEOUT_SEC 5

// 全局Socket用于信号处理
static volatile int g_sockfd = -1;

void signal_handler(int sig) {
    if (sig == SIGINT && g_sockfd >= 0) {
        close(g_sockfd);
        g_sockfd = -1;
    }
}

/**
 * 设置Socket为非阻塞模式(用于connect超时控制)
 */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * 设置Socket为阻塞模式
 */
int set_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

/**
 * 带超时的connect
 */
int connect_with_timeout(int sockfd, const struct sockaddr *addr,
                         socklen_t addrlen, int timeout_sec) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int ret = connect(sockfd, addr, addrlen);
    if (ret < 0 && errno != EINPROGRESS) {
        return -1;
    }

    if (ret == 0) {
        // 立即连接成功
        fcntl(sockfd, F_SETFL, flags);
        return 0;
    }

    // 使用select等待连接完成
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    ret = select(sockfd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
        return -1; // 超时或错误
    }

    // 检查连接是否成功
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

    if (so_error != 0) {
        errno = so_error;
        return -1;
    }

    fcntl(sockfd, F_SETFL, flags);
    return 0;
}

int tcp_client_full(const char *host, int port) {
    // 忽略SIGPIPE信号,避免写入已关闭连接导致进程终止
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);

    // 1. 创建Socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    g_sockfd = sockfd;

    // 2. 解析主机名
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "ERROR: Host not found: %s\n", host);
        close(sockfd);
        return -1;
    }

    // 3. 设置服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    printf("Connecting to %s:%d...\n", inet_ntoa(serv_addr.sin_addr), port);

    // 4. 建立连接(带超时)
    if (connect_with_timeout(sockfd, (struct sockaddr *)&serv_addr,
                             sizeof(serv_addr), CONNECT_TIMEOUT_SEC) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }
    printf("Connected successfully!\n");

    // 5. 设置接收超时
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // 6. 数据通信
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(sendbuf, sizeof(sendbuf), stdin) == NULL) {
            break; // EOF
        }

        // 发送数据
        size_t len = strlen(sendbuf);
        ssize_t sent = send(sockfd, sendbuf, len, 0);
        if (sent < 0) {
            if (errno == EPIPE) {
                printf("Server closed connection\n");
            } else {
                perror("send");
            }
            break;
        }

        // 接收响应
        ssize_t n = recv(sockfd, recvbuf, sizeof(recvbuf) - 1, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("Receive timeout\n");
                continue;
            }
            perror("recv");
            break;
        } else if (n == 0) {
            printf("Server closed connection\n");
            break;
        }

        recvbuf[n] = '\0';
        printf("< %s", recvbuf);
    }

    // 7. 优雅关闭
    printf("Closing connection...\n");
    shutdown(sockfd, SHUT_WR); // 发送FIN

    // 等待对方关闭连接(半关闭状态可以接收剩余数据)
    char buf[1024];
    while (recv(sockfd, buf, sizeof(buf), 0) > 0);

    close(sockfd);
    g_sockfd = -1;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    return tcp_client_full(argv[1], atoi(argv[2]));
}
```

#### TCP服务器(迭代版)

```c
/**
 * tcp_server_iterative.c - 迭代TCP服务器
 * 处理完一个客户端后才能处理下一个
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096
#define LISTEN_BACKLOG 128

void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr->sin_port);

    printf("[Client %s:%d] Connected\n", client_ip, client_port);

    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[Client %s:%d] Received %zd bytes: %s",
               client_ip, client_port, n, buffer);

        // 简单echo服务
        if (send(client_fd, buffer, n, 0) != n) {
            perror("send");
            break;
        }
    }

    if (n < 0) {
        perror("recv");
    }

    printf("[Client %s:%d] Disconnected\n", client_ip, client_port);
    close(client_fd);
}

int tcp_server_iterative(int port) {
    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // 创建Socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    // 设置地址复用
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        close(listen_fd);
        return -1;
    }

    // 绑定地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    // 开始监听
    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    printf("Server listening on port %d...\n", port);

    // 迭代处理客户端
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue; // 被信号中断,重试
            }
            perror("accept");
            continue;
        }

        handle_client(client_fd, &client_addr);
        // 处理完才继续accept下一个
    }

    close(listen_fd);
    return 0;
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    return tcp_server_iterative(port);
}
```

### 4.2 基本UDP客户端/服务器

```c
/**
 * udp_examples.c - UDP编程完整示例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// ========== UDP客户端 ==========
int udp_client(const char *server_ip, int server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // UDP不需要connect,但可以使用connect来固定对端地址
    // connect后的UDP可以使用send/write代替sendto
    // connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    char sendbuf[1024], recvbuf[1024];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
        // 发送数据到服务器
        ssize_t sent = sendto(sockfd, sendbuf, strlen(sendbuf), 0,
                              (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (sent < 0) {
            perror("sendto");
            continue;
        }

        // 接收响应(UDP是无连接的,需要记录对端地址)
        ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf) - 1, 0,
                             (struct sockaddr *)&from_addr, &from_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        recvbuf[n] = '\0';
        printf("Received from %s:%d: %s",
               inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), recvbuf);
    }

    close(sockfd);
    return 0;
}

// ========== UDP服务器(迭代) ==========
int udp_server(int port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 允许地址复用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    printf("UDP server listening on port %d...\n", port);

    char buffer[65535]; // UDP最大包大小
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[n] = '\0';
        printf("Received %zd bytes from %s:%d\n", n,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Echo回客户端
        sendto(sockfd, buffer, n, 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}

// ========== 已连接的UDP客户端 ==========
int udp_connected_client(const char *server_ip, int server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    // 连接UDP Socket
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    // 现在可以使用send/recv代替sendto/recvfrom
    const char *msg = "Hello UDP";
    send(sockfd, msg, strlen(msg), 0);

    char buffer[1024];
    ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}

// ========== UDP广播发送 ==========
int udp_broadcast_sender(int broadcast_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 启用广播
    int broadcast = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                   &broadcast, sizeof(broadcast)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return -1;
    }

    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(broadcast_port);
    inet_pton(AF_INET, "255.255.255.255", &broadcast_addr.sin_addr);
    // 或使用受限广播: 192.168.1.255 (取决于子网)

    const char *msg = "Broadcast message!";
    sendto(sockfd, msg, strlen(msg), 0,
           (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));

    close(sockfd);
    return 0;
}

// ========== UDP多播接收 ==========
int udp_multicast_receiver(int multicast_port, const char *multicast_ip) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 允许地址复用(多播必需)
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 绑定到多播端口
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(multicast_port);
    local_addr.sin_addr.s_addr = INADDR_ANY; // 接收所有接口的多播

    bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    // 加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_ip);
    mreq.imr_interface.s_addr = INADDR_ANY; // 使用默认接口

    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP");
        close(sockfd);
        return -1;
    }

    printf("Joined multicast group %s:%d\n", multicast_ip, multicast_port);

    char buffer[1024];
    while (1) {
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Multicast: %s\n", buffer);
        }
    }

    // 离开多播组(程序退出前)
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sockfd);
    return 0;
}
```

### 4.3 并发服务器实现

#### 多进程并发服务器

```c
/**
 * tcp_server_fork.c - 多进程并发服务器
 * 每个客户端一个进程,使用signal处理僵尸进程
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096

// SIGCHLD信号处理: 收割僵尸进程
void sigchld_handler(int sig) {
    // 使用while循环收割所有已终止的子进程
    // WNOHANG: 非阻塞,没有已终止子进程时立即返回0
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n;

    // 子进程中处理客户端
    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[PID %d] Received: %s", getpid(), buffer);

        // Echo回客户端
        send(client_fd, buffer, n, 0);
    }

    close(client_fd);
    exit(0); // 子进程退出
}

int tcp_server_fork(int port) {
    // 设置SIGCHLD处理函数
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // 被信号中断的系统调用自动重试
    sigaction(SIGCHLD, &sa, NULL);

    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    // 创建监听Socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    // 地址复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    printf("Fork server listening on port %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        printf("New connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue;
        } else if (pid == 0) {
            // 子进程
            close(listen_fd); // 子进程不需要监听Socket
            handle_client(client_fd);
            // handle_client中exit
        } else {
            // 父进程
            close(client_fd); // 父进程关闭已连接的Socket
            // 继续accept下一个
        }
    }

    close(listen_fd);
    return 0;
}
```

#### 多线程并发服务器

```c
/**
 * tcp_server_thread.c - 多线程并发服务器
 * 使用线程池或每个客户端一个线程
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096
#define MAX_THREADS 100

// 线程参数结构
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
} client_info_t;

// 线程处理函数
void *client_thread(void *arg) {
    client_info_t *info = (client_info_t *)arg;
    int client_fd = info->client_fd;
    struct sockaddr_in client_addr = info->client_addr;
    free(info); // 释放参数内存

    // 分离线程,结束时自动回收资源
    pthread_detach(pthread_self());

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[Thread %lu] Handling client %s:%d\n",
           pthread_self(), client_ip, ntohs(client_addr.sin_port));

    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[Thread %lu] Received: %s", pthread_self(), buffer);
        send(client_fd, buffer, n, 0);
    }

    if (n < 0) {
        perror("recv");
    }

    printf("[Thread %lu] Client disconnected\n", pthread_self());
    close(client_fd);
    return NULL;
}

int tcp_server_thread(int port) {
    // 忽略SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    printf("Thread server listening on port %d...\n", port);

    int thread_count = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }

        // 限制并发线程数
        if (thread_count >= MAX_THREADS) {
            fprintf(stderr, "Too many connections, rejecting...\n");
            close(client_fd);
            continue;
        }

        // 创建线程参数
        client_info_t *info = malloc(sizeof(client_info_t));
        info->client_fd = client_fd;
        info->client_addr = client_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, info) != 0) {
            perror("pthread_create");
            free(info);
            close(client_fd);
            continue;
        }

        thread_count++;
    }

    close(listen_fd);
    return 0;
}
```

#### select多路复用服务器

```c
/**
 * tcp_server_select.c - select多路复用服务器
 * 单线程处理多个连接
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 4096
#define MAX_CLIENTS FD_SETSIZE  // select最大支持1024

typedef struct {
    int fd;
    char buffer[BUFFER_SIZE];
    int buf_len;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

int tcp_server_select(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    // 初始化客户端数组
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
    }

    printf("Select server listening on port %d...\n", port);

    fd_set read_fds;
    int max_fd = listen_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        max_fd = listen_fd;

        // 将所有客户端fd加入集合
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                FD_SET(clients[i].fd, &read_fds);
                if (clients[i].fd > max_fd) {
                    max_fd = clients[i].fd;
                }
            }
        }

        // select等待IO事件
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1秒超时,用于处理定时任务
        timeout.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        if (activity == 0) {
            // 超时,可以处理定时任务
            continue;
        }

        // 新连接
        if (FD_ISSET(listen_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (client_fd < 0) {
                perror("accept");
                continue;
            }

            printf("New connection: %s:%d, fd=%d\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

            // 找到空闲槽位
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = client_fd;
                    clients[i].buf_len = 0;
                    client_count++;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Too many clients\n");
                close(client_fd);
            }
        }

        // 处理客户端IO
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i].fd;
            if (fd < 0) continue;

            if (FD_ISSET(fd, &read_fds)) {
                char recv_buf[BUFFER_SIZE];
                ssize_t n = recv(fd, recv_buf, sizeof(recv_buf), 0);

                if (n <= 0) {
                    // 连接关闭或错误
                    if (n < 0) perror("recv");
                    printf("Client fd=%d disconnected\n", fd);
                    close(fd);
                    clients[i].fd = -1;
                    client_count--;
                } else {
                    // 处理数据(简单echo)
                    send(fd, recv_buf, n, 0);
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}
```

#### epoll服务器(LT/ET模式)

```c
/**
 * tcp_server_epoll.c - epoll高性能服务器
 * 支持Level Trigger和Edge Trigger模式
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTEN_BACKLOG 128
#define MAX_EVENTS 1024
#define BUFFER_SIZE 4096

// 设置非阻塞
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// LT模式处理客户端读
void handle_read_lt(int epoll_fd, int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (n <= 0) {
        if (n < 0) perror("recv");
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        printf("Client fd=%d closed (LT)\n", client_fd);
        return;
    }

    buffer[n] = '\0';
    printf("[LT] Received %zd bytes from fd=%d\n", n, client_fd);

    // Echo回客户端
    send(client_fd, buffer, n, 0);
}

// ET模式处理客户端读 - 必须读到EAGAIN
void handle_read_et(int epoll_fd, int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t total = 0;

    // ET模式下必须循环读到返回EAGAIN
    while (1) {
        ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);

        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 数据读完了
                break;
            }
            perror("recv");
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
            return;
        } else if (n == 0) {
            // 连接关闭
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
            close(client_fd);
            printf("Client fd=%d closed (ET)\n", client_fd);
            return;
        }

        total += n;
        // 处理数据...
        send(client_fd, buffer, n, 0);
    }

    printf("[ET] Total received %zd bytes from fd=%d\n", total, client_fd);
}

int tcp_server_epoll(int port, int use_et) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(listen_fd);
        return -1;
    }

    // 添加监听fd到epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    printf("Epoll server listening on port %d (%s mode)...\n",
           port, use_et ? "ET" : "LT");

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_fd) {
                // 新连接
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);

                int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                // ET模式需要设置非阻塞
                if (use_et) {
                    set_nonblocking(client_fd);
                }

                // 添加新连接到epoll
                ev.data.fd = client_fd;
                ev.events = EPOLLIN | (use_et ? EPOLLET : 0);
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                printf("New connection: %s:%d, fd=%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port), client_fd);
            } else {
                // 客户端数据
                if (use_et) {
                    handle_read_et(epoll_fd, events[i].data.fd);
                } else {
                    handle_read_lt(epoll_fd, events[i].data.fd);
                }
            }
        }
    }

    close(epoll_fd);
    close(listen_fd);
    return 0;
}
```

### 4.4 高并发服务器架构

#### Reactor模式实现

```c
/**
 * reactor.c - Reactor模式实现
 * 单线程/多线程Reactor模式
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_EVENTS 1024
#define BUFFER_SIZE 8192

// 事件处理器类型
typedef enum {
    EVENT_ACCEPT,   // 接受连接
    EVENT_READ,     // 可读
    EVENT_WRITE,    // 可写
    EVENT_ERROR     // 错误
} event_type_t;

// 事件回调函数原型
typedef void (*event_callback_t)(int fd, event_type_t type, void *arg);

// 事件处理器结构
typedef struct event_handler {
    int fd;
    event_type_t type;
    event_callback_t callback;
    void *arg;
    struct event_handler *next;
} event_handler_t;

// Reactor结构
typedef struct {
    int epoll_fd;
    int listen_fd;
    event_handler_t *handlers;  // 处理器链表
    int running;
} reactor_t;

// 连接上下文
typedef struct {
    int fd;
    char read_buf[BUFFER_SIZE];
    int read_len;
    char write_buf[BUFFER_SIZE];
    int write_len;
} connection_t;

// 创建Reactor
reactor_t *reactor_create() {
    reactor_t *reactor = malloc(sizeof(reactor_t));
    reactor->epoll_fd = epoll_create1(0);
    reactor->handlers = NULL;
    reactor->running = 0;
    return reactor;
}

// 注册事件处理器
void reactor_register(reactor_t *reactor, int fd, event_type_t type,
                      event_callback_t callback, void *arg) {
    event_handler_t *handler = malloc(sizeof(event_handler_t));
    handler->fd = fd;
    handler->type = type;
    handler->callback = callback;
    handler->arg = arg;
    handler->next = reactor->handlers;
    reactor->handlers = handler;

    struct epoll_event ev;
    ev.events = (type == EVENT_READ) ? EPOLLIN :
                (type == EVENT_WRITE) ? EPOLLOUT : EPOLLIN | EPOLLOUT;
    ev.events |= EPOLLET;  // 边缘触发
    ev.data.ptr = handler;
    epoll_ctl(reactor->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

// 修改事件
void reactor_modify(reactor_t *reactor, int fd, event_type_t new_type) {
    struct epoll_event ev;
    ev.events = (new_type == EVENT_READ) ? EPOLLIN :
                (new_type == EVENT_WRITE) ? EPOLLOUT : EPOLLIN | EPOLLOUT;
    ev.events |= EPOLLET;
    ev.data.fd = fd;
    epoll_ctl(reactor->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

// 事件循环
void reactor_run(reactor_t *reactor) {
    reactor->running = 1;
    struct epoll_event events[MAX_EVENTS];

    while (reactor->running) {
        int nfds = epoll_wait(reactor->epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            event_handler_t *handler = events[i].data.ptr;

            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
                handler->callback(handler->fd, EVENT_ERROR, handler->arg);
                continue;
            }

            if (events[i].events & EPOLLIN) {
                handler->callback(handler->fd, EVENT_READ, handler->arg);
            }

            if (events[i].events & EPOLLOUT) {
                handler->callback(handler->fd, EVENT_WRITE, handler->arg);
            }
        }
    }
}

// 接受连接回调
void on_accept(int listen_fd, event_type_t type, void *arg) {
    reactor_t *reactor = arg;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("accept");
            return;
        }

        // 设置非阻塞
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        connection_t *conn = calloc(1, sizeof(connection_t));
        conn->fd = client_fd;

        // 注册读事件
        reactor_register(reactor, client_fd, EVENT_READ, NULL, conn);

        printf("New connection: fd=%d\n", client_fd);
    }
}

// 使用示例
int reactor_server(int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(listen_fd, 128);

    // 设置非阻塞
    fcntl(listen_fd, F_SETFL, O_NONBLOCK);

    reactor_t *reactor = reactor_create();
    reactor->listen_fd = listen_fd;

    // 注册接受连接事件
    // reactor_register(reactor, listen_fd, EVENT_ACCEPT, on_accept, reactor);

    // reactor_run(reactor);

    return 0;
}
```

### 4.5 协议解析示例

#### 简单HTTP服务器

```c
/**
 * simple_http_server.c - 简单HTTP/1.0服务器
 * 演示协议解析基础
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 256

typedef struct {
    char method[16];
    char path[MAX_PATH];
    char version[16];
    char headers[4096];
    int header_len;
    int content_length;
    int keep_alive;
} http_request_t;

// 解析HTTP请求行
int parse_request_line(const char *line, http_request_t *req) {
    return sscanf(line, "%15s %255s %15s", req->method, req->path, req->version);
}

// 解析HTTP头部
void parse_headers(const char *headers, http_request_t *req) {
    const char *p = headers;

    while (*p) {
        if (strncasecmp(p, "Content-Length:", 15) == 0) {
            req->content_length = atoi(p + 15);
        } else if (strncasecmp(p, "Connection:", 11) == 0) {
            req->keep_alive = (strcasestr(p, "keep-alive") != NULL);
        }

        // 跳到下一行
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }
}

// 发送HTTP响应
void send_http_response(int client_fd, int status, const char *status_text,
                        const char *content_type, const char *body, size_t body_len) {
    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    if (body && body_len > 0) {
        send(client_fd, body, body_len, 0);
    }
}

// 处理HTTP请求
void handle_http_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    http_request_t req = {0};

    // 读取请求
    int total = 0;
    while (total < BUFFER_SIZE - 1) {
        int n = recv(client_fd, buffer + total, BUFFER_SIZE - 1 - total, 0);
        if (n <= 0) return;
        total += n;
        buffer[total] = '\0';

        // 检查是否收到完整的HTTP头
        if (strstr(buffer, "\r\n\r\n") != NULL) {
            break;
        }
    }

    // 解析请求行
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end) return;

    *line_end = '\0';
    if (parse_request_line(buffer, &req) != 3) {
        send_http_response(client_fd, 400, "Bad Request",
                          "text/plain", "Bad Request", 11);
        return;
    }

    printf("%s %s %s\n", req.method, req.path, req.version);

    // 只处理GET请求
    if (strcmp(req.method, "GET") != 0) {
        send_http_response(client_fd, 405, "Method Not Allowed",
                          "text/plain", "Method Not Allowed", 18);
        return;
    }

    // 构建文件路径(安全处理)
    char file_path[MAX_PATH + 16];
    if (strcmp(req.path, "/") == 0) {
        strcpy(req.path, "/index.html");
    }

    // 防止目录遍历攻击
    if (strstr(req.path, "..") != NULL) {
        send_http_response(client_fd, 403, "Forbidden",
                          "text/plain", "Forbidden", 9);
        return;
    }

    snprintf(file_path, sizeof(file_path), "./www%s", req.path);

    // 打开并发送文件
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        send_http_response(client_fd, 404, "Not Found",
                          "text/plain", "Not Found", 9);
        return;
    }

    struct stat st;
    fstat(fd, &st);

    // 判断Content-Type
    const char *content_type = "text/html";
    if (strstr(file_path, ".css")) content_type = "text/css";
    else if (strstr(file_path, ".js")) content_type = "application/javascript";
    else if (strstr(file_path, ".png")) content_type = "image/png";
    else if (strstr(file_path, ".jpg")) content_type = "image/jpeg";

    // 发送头部
    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type, st.st_size);
    send(client_fd, header, header_len, 0);

    // 发送文件内容
    while (1) {
        int n = read(fd, buffer, BUFFER_SIZE);
        if (n <= 0) break;
        send(client_fd, buffer, n, 0);
    }

    close(fd);
}
```

### 4.6 非阻塞IO实现

```c
/**
 * nonblocking_io.c - 非阻塞IO实现示例
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 设置非阻塞模式
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 非阻塞Connect
int nonblocking_connect(const char *ip, int port, int timeout_sec) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return -1;

    set_nonblocking(sockfd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        close(sockfd);
        return -1;
    }

    if (ret == 0) {
        // 立即连接成功
        return sockfd;
    }

    // 等待连接完成(使用select)
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    ret = select(sockfd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
        close(sockfd);
        return -1;
    }

    // 检查连接状态
    int so_error;
    socklen_t len = sizeof(so_error);
    getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);

    if (so_error != 0) {
        errno = so_error;
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// 非阻塞发送(处理EAGAIN)
ssize_t nonblocking_send(int fd, const void *buf, size_t len) {
    const char *p = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t n = send(fd, p, remaining, MSG_NOSIGNAL);

        if (n > 0) {
            p += n;
            remaining -= n;
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送缓冲区已满,需要等待
                return len - remaining; // 返回已发送字节数
            } else if (errno == EINTR) {
                continue; // 被信号中断,重试
            } else {
                return -1; // 错误
            }
        } else {
            return -1; // 连接关闭
        }
    }

    return len;
}

// 非阻塞接收(处理EAGAIN)
ssize_t nonblocking_recv(int fd, void *buf, size_t len) {
    ssize_t total = 0;
    char *p = buf;

    while (1) {
        ssize_t n = recv(fd, p + total, len - total, 0);

        if (n > 0) {
            total += n;
            if ((size_t)total >= len) break; // 缓冲区已满
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 无数据可读
                break;
            } else if (errno == EINTR) {
                continue;
            } else {
                return (total > 0) ? total : -1;
            }
        } else {
            // n == 0, 连接关闭
            break;
        }
    }

    return total;
}
```

### 4.7 零拷贝传输

```c
/**
 * zero_copy.c - 零拷贝传输实现
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>

// ========== sendfile实现文件传输 ==========
void send_file_sendfile(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    struct stat st;
    fstat(file_fd, &st);
    off_t offset = 0;

    // sendfile: 内核态直接传输,无需用户态缓冲
    // 参数: out_fd(必须是socket), in_fd(必须是支持mmap的文件), offset, count
    ssize_t sent = sendfile(client_fd, file_fd, &offset, st.st_size);

    if (sent < 0) {
        perror("sendfile");
    } else {
        printf("Sent %zd bytes using sendfile\n", sent);
    }

    close(file_fd);
}

// ========== splice实现管道零拷贝 ==========
#include <fcntl.h>

void transfer_with_splice(int in_fd, int out_fd, size_t len) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return;
    }

    size_t total = 0;
    while (total < len) {
        // 从in_fd splice到管道
        ssize_t n = splice(in_fd, NULL, pipefd[1], NULL,
                           len - total, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (n <= 0) break;

        // 从管道splice到out_fd
        ssize_t m = splice(pipefd[0], NULL, out_fd, NULL,
                           n, SPLICE_F_MOVE | SPLICE_F_MORE);
        if (m <= 0) break;

        total += m;
    }

    close(pipefd[0]);
    close(pipefd[1]);

    printf("Transferred %zu bytes using splice\n", total);
}

// ========== mmap + write 实现 ==========
#include <sys/mman.h>

void send_file_mmap(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    struct stat st;
    fstat(file_fd, &st);

    // mmap文件到内存
    void *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(file_fd);
        return;
    }

    // 发送数据(仍然需要用户态到内核态拷贝,但比read/write少一次)
    ssize_t sent = send(client_fd, mapped, st.st_size, MSG_NOSIGNAL);

    if (sent < 0) {
        perror("send");
    } else {
        printf("Sent %zd bytes using mmap\n", sent);
    }

    munmap(mapped, st.st_size);
    close(file_fd);
}

// ========== 传统方法对比 ==========
void send_file_traditional(int client_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return;
    }

    char buffer[8192];
    ssize_t n;
    size_t total = 0;

    // 传统方法: 磁盘 -> 内核缓冲区 -> 用户缓冲区 -> 内核Socket缓冲区 -> 网卡
    while ((n = read(file_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t sent = send(client_fd, buffer, n, MSG_NOSIGNAL);
        if (sent < 0) {
            perror("send");
            break;
        }
        total += sent;
    }

    printf("Sent %zu bytes using traditional method\n", total);
    close(file_fd);
}

/**
 * 零拷贝方法对比:
 *
 * 传统方法(read/write):
 *   磁盘 -> 页缓存 -> 用户缓冲区 -> Socket缓冲区 -> 网卡
 *   [4次数据拷贝, 4次上下文切换]
 *
 * mmap + send:
 *   磁盘 -> 页缓存 -> Socket缓冲区 -> 网卡
 *   [3次数据拷贝, 4次上下文切换]
 *   注意: 如果页缓存命中,实际是2次拷贝
 *
 * sendfile:
 *   磁盘 -> 页缓存 -> Socket缓冲区 -> 网卡
 *   [2次数据拷贝(如果网卡支持SG-DMA则1次), 2次上下文切换]
 *
 * splice:
 *   管道两端都是内核缓冲区,完全零拷贝
 *   [0次数据拷贝, 2次上下文切换]
 */
```

---

## ⚠️ 5. 反例与陷阱

### 5.1 常见陷阱列表

#### 陷阱01: 忘记处理SIGPIPE

```c
// ❌ 错误示例: 未处理SIGPIPE,写入已关闭连接会导致进程终止
void bad_example() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, ...);

    // 如果对端关闭了连接,send会触发SIGPIPE,默认行为是终止进程
    send(sock, "data", 4, 0);  // 进程可能被杀死!
}

// ✅ 正确做法1: 忽略SIGPIPE
void good_example1() {
    signal(SIGPIPE, SIG_IGN);  // 全局忽略
    // 或
    // sigaction(SIGPIPE, &(struct sigaction){.sa_handler = SIG_IGN}, NULL);
}

// ✅ 正确做法2: 使用MSG_NOSIGNAL标志
void good_example2(int sock) {
    send(sock, "data", 4, MSG_NOSIGNAL);  // 单次发送忽略SIGPIPE
}

// ✅ 正确做法3: 检查对端关闭状态
void good_example3(int sock) {
    // 使用poll/epoll检测EPOLLRDHUP
    // 或在send前检查连接状态
}
```

#### 陷阱02: 缓冲区溢出

```c
// ❌ 错误示例: 固定缓冲区可能导致溢出
void bad_recv(int sock) {
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';  // 如果n==1024,这里会越界!
    // ...
}

// ✅ 正确做法
void good_recv(int sock) {
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);  // 留一个字节给'\0'
    if (n > 0) {
        buffer[n] = '\0';
    }
}

// ❌ 错误示例: 不检查sprintf返回值
void bad_sprintf(char *input) {
    char output[100];
    sprintf(output, "Result: %s", input);  // input可能过长!
}

// ✅ 正确做法
void good_snprintf(const char *input) {
    char output[100];
    snprintf(output, sizeof(output), "Result: %s", input);
    output[sizeof(output) - 1] = '\0';  // 确保终止
}
```

#### 陷阱03: 粘包问题

```c
/**
 * ❌ 错误示例: 假设每次recv能收到完整的消息
 * TCP是字节流,不保留消息边界
 */
void bad_message_handling(int sock) {
    char buffer[1024];
    // 发送方发送 "Hello" 和 "World" 两次
    // 接收方可能一次收到 "HelloWorld"
    int n = recv(sock, buffer, sizeof(buffer), 0);
    // 错误假设: buffer中包含一条完整消息
    process_message(buffer, n);
}

/**
 * ✅ 正确做法1: 定长消息
 */
void good_fixed_length(int sock) {
    #define MSG_LEN 256
    char buffer[MSG_LEN];
    int received = 0;

    while (received < MSG_LEN) {
        int n = recv(sock, buffer + received, MSG_LEN - received, 0);
        if (n <= 0) return; // 错误或关闭
        received += n;
    }
    process_message(buffer, MSG_LEN);
}

/**
 * ✅ 正确做法2: 消息长度前缀
 */
typedef struct {
    uint32_t length;  // 网络字节序
    char data[];      // 变长数据
} message_t;

void good_length_prefix(int sock) {
    // 先读取4字节长度
    uint32_t net_len;
    int n = recv_all(sock, &net_len, sizeof(net_len));
    if (n != sizeof(net_len)) return;

    uint32_t len = ntohl(net_len);
    if (len > MAX_MESSAGE_SIZE) {
        // 防止内存分配攻击
        close(sock);
        return;
    }

    // 分配缓冲区并读取数据
    char *buffer = malloc(len);
    n = recv_all(sock, buffer, len);
    if (n == len) {
        process_message(buffer, len);
    }
    free(buffer);
}

/**
 * ✅ 正确做法3: 特殊分隔符
 */
void good_delimiter(int sock) {
    char buffer[4096];
    int buf_len = 0;

    while (1) {
        int n = recv(sock, buffer + buf_len, sizeof(buffer) - buf_len, 0);
        if (n <= 0) return;
        buf_len += n;

        // 查找分隔符(如\n)
        char *newline = memchr(buffer, '\n', buf_len);
        while (newline != NULL) {
            int msg_len = newline - buffer + 1;
            process_message(buffer, msg_len);

            // 移动剩余数据
            memmove(buffer, buffer + msg_len, buf_len - msg_len);
            buf_len -= msg_len;
            newline = memchr(buffer, '\n', buf_len);
        }

        if (buf_len >= sizeof(buffer)) {
            // 缓冲区满但未找到分隔符,错误
            close(sock);
            return;
        }
    }
}
```

#### 陷阱04: 字节序问题

```c
// ❌ 错误示例: 不转换字节序
void bad_byte_order() {
    struct sockaddr_in addr;
    addr.sin_port = 8080;  // 错误! 应该使用htons
    addr.sin_addr.s_addr = 0x0100007F;  // 127.0.0.1 但字节序可能错误
}

// ✅ 正确做法
void good_byte_order() {
    struct sockaddr_in addr;
    addr.sin_port = htons(8080);  // 主机字节序转网络字节序
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1
    // 或使用 inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
}

// ❌ 错误示例: 协议字段不转换
void bad_protocol_field() {
    typedef struct {
        uint32_t magic;
        uint32_t length;
        uint32_t checksum;
    } packet_header_t;

    packet_header_t hdr;
    hdr.magic = 0x12345678;  // 错误! 未转网络字节序
    hdr.length = data_len;
    send(sock, &hdr, sizeof(hdr), 0);
}

// ✅ 正确做法
void good_protocol_field() {
    packet_header_t hdr;
    hdr.magic = htonl(0x12345678);
    hdr.length = htonl(data_len);
    hdr.checksum = htonl(calculate_checksum(data));
    send(sock, &hdr, sizeof(hdr), 0);
}

// 接收时转换
void receive_protocol() {
    packet_header_t hdr;
    recv(sock, &hdr, sizeof(hdr), 0);
    uint32_t magic = ntohl(hdr.magic);
    uint32_t length = ntohl(hdr.length);
    // ...
}
```

#### 陷阱05: 资源泄漏(忘记close)

```c
// ❌ 错误示例: 资源泄漏
void bad_resource_leak() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    if (connect(sock, ...) < 0) {
        return;  // 错误! sock未关闭
    }

    // ... 使用sock

    if (error_condition) {
        return;  // 错误! sock未关闭
    }

    close(sock);
}

// ✅ 正确做法1: goto清理
void good_goto_cleanup() {
    int sock = -1;
    int ret = -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) goto cleanup;

    if (connect(sock, ...) < 0) goto cleanup;

    // ... 使用sock

    ret = 0;
cleanup:
    if (sock >= 0) close(sock);
    return ret;
}

// ✅ 正确做法2: 使用do-while(0)
void good_do_while() {
    int sock = -1;
    int result = -1;

    do {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) break;

        if (connect(sock, ...) < 0) break;

        // ... 使用sock

        result = 0;
    } while (0);

    if (sock >= 0) close(sock);
    return result;
}

// ✅ 正确做法3: 使用__attribute__((cleanup)) (GCC扩展)
#ifdef __GNUC__
void auto_close(int *fd) {
    if (*fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}

void good_cleanup_attribute() {
    int sock __attribute__((cleanup(auto_close))) = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    if (connect(sock, ...) < 0) return;  // sock会自动关闭

    // ...
}  // 函数退出时sock自动关闭
#endif
```

#### 陷阱06: 僵尸进程(未处理SIGCHLD)

```c
// ❌ 错误示例: 产生僵尸进程
void bad_fork_server() {
    while (1) {
        int client = accept(listen_fd, ...);
        if (fork() == 0) {
            // 子进程
            close(listen_fd);
            handle_client(client);
            close(client);
            exit(0);  // 子进程终止
        }
        close(client);
        // 父进程没有wait,子进程变成僵尸进程!
    }
}

// ✅ 正确做法1: signal处理SIGCHLD
#include <sys/wait.h>

void sigchld_handler(int sig) {
    // WNOHANG: 非阻塞
    // -1: 等待任何子进程
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void good_sigchld_handler() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  // 被中断的系统调用自动重启
    sigaction(SIGCHLD, &sa, NULL);

    // 或者使用signal(较简单但不适用于所有情况)
    // signal(SIGCHLD, sigchld_handler);
}

// ✅ 正确做法2: 忽略SIGCHLD (Linux特有)
void good_ignore_sigchld() {
    // Linux中忽略SIGCHLD会自动处理僵尸进程
    signal(SIGCHLD, SIG_IGN);
}

// ✅ 正确做法3: 双fork避免僵尸进程
void good_double_fork() {
    while (1) {
        int client = accept(listen_fd, ...);

        pid_t pid = fork();
        if (pid == 0) {
            // 第一层子进程
            if (fork() == 0) {
                // 第二层子进程(孤儿进程,被init收养)
                close(listen_fd);
                handle_client(client);
                exit(0);
            }
            // 第一层子进程立即退出
            exit(0);
        }

        close(client);
        waitpid(pid, NULL, 0);  // 收割第一层子进程
    }
}
```

#### 陷阱07: TIME_WAIT累积

```c
/**
 * ❌ 问题描述:
 * 主动关闭方会进入TIME_WAIT状态,持续2MSL(通常60秒)
 * 高并发短连接场景下,大量TIME_WAIT会耗尽端口或内存
 */

// 查看TIME_WAIT数量
// netstat -an | grep TIME_WAIT | wc -l

// ✅ 解决方案1: 服务器端启用SO_REUSEADDR
void good_reuseaddr() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 允许重用处于TIME_WAIT状态的地址
}

// ✅ 解决方案2: 启用TCP端口复用(Linux 3.9+)
void good_reuseport() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    // 多个socket可以绑定到同一端口(用于负载均衡)
}

// ✅ 解决方案3: 调整TCP参数(Linux)
void adjust_tcp_params() {
    // /etc/sysctl.conf:
    // net.ipv4.tcp_tw_reuse = 1      # 允许重用TIME_WAIT连接(仅客户端)
    // net.ipv4.tcp_tw_recycle = 0    # 已废弃,不使用
    // net.ipv4.tcp_fin_timeout = 30  # 减少FIN_WAIT_2时间
    // net.ipv4.tcp_max_tw_buckets = 5000  # 限制TIME_WAIT数量
}

// ✅ 解决方案4: 连接池/长连接
void use_connection_pool() {
    // 避免频繁创建/关闭连接
    // 使用keep-alive维持长连接
    int keepalive = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
}

// ✅ 解决方案5: 客户端先关闭(服务器不进入TIME_WAIT)
void server_design_pattern() {
    // 协议设计: 客户端发送请求后主动关闭连接
    // 服务器被动关闭,不进入TIME_WAIT
}
```

#### 陷阱08: 惊群问题(Thundering Herd)

```c
/**
 * ❌ 问题描述:
 * 多个进程/线程同时阻塞在accept或epoll_wait上
 * 当有新连接到来时,所有阻塞的进程都会被唤醒
 * 但只有其中一个能成功处理,其他返回EAGAIN,浪费CPU
 */

// 错误示例: accept惊群
void bad_thundering_herd() {
    // 多个进程同时accept
    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            while (1) {
                int client = accept(listen_fd, ...);  // 所有进程同时被唤醒!
                handle_client(client);
            }
        }
    }
}

// ✅ 解决方案1: 使用互斥锁保护accept (accept_mutex)
#include <pthread.h>
pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;

void good_accept_mutex() {
    while (1) {
        pthread_mutex_lock(&accept_mutex);
        int client = accept(listen_fd, ...);
        pthread_mutex_unlock(&accept_mutex);

        if (client >= 0) {
            handle_client(client);
        }
    }
}

// ✅ 解决方案2: 使用SO_REUSEPORT (Linux 3.9+)
void good_reuseport_solve() {
    // 每个进程创建独立的监听socket
    // 内核自动进行负载均衡,不会出现惊群
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    bind(sock, ...);
    listen(sock, ...);
    // 每个进程独立accept,无惊群
}

// ✅ 解决方案3: 单进程accept + 多进程/线程处理
void good_single_acceptor() {
    // 主进程专门accept
    // 将client_fd通过socketpair/unix socket传递给工作进程
    // 或使用线程池
}

// ✅ 解决方案4: epoll使用EPOLLEXCLUSIVE (Linux 4.5+)
void good_epollexclusive() {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLEXCLUSIVE;  // 只有一个进程被唤醒
    ev.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);
}
```

#### 陷阱09: 半开连接(Half-Open Connection)

```c
/**
 * ❌ 问题描述:
 * 连接一方异常终止(断电/崩溃),另一方无法感知
 * 导致连接长时间处于ESTABLISHED状态,资源被占用
 */

// 错误示例: 无心跳检测
void bad_no_keepalive(int sock) {
    while (1) {
        char buffer[1024];
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) break;  // 对方正常关闭才能检测到
        // 处理数据
    }
}

// ✅ 解决方案1: TCP Keepalive
void good_tcp_keepalive(int sock) {
    int keepalive = 1;      // 启用
    int idle = 60;          // 60秒无数据开始探测
    int interval = 10;      // 探测间隔10秒
    int count = 3;          // 探测3次无响应则断开

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
}

// ✅ 解决方案2: 应用层心跳
void good_application_heartbeat(int sock) {
    // 协议定义心跳消息
    // 每30秒发送一次心跳
    // 90秒未收到响应则断开连接
}

// ✅ 解决方案3: 设置收发超时
void good_socket_timeout(int sock) {
    struct timeval tv;
    tv.tv_sec = 60;   // 60秒超时
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

// ✅ 解决方案4: 使用TCP_USER_TIMEOUT (Linux)
void good_tcp_user_timeout(int sock) {
    unsigned int timeout = 20000;  // 20毫秒
    setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT,
               &timeout, sizeof(timeout));
}
```

#### 陷阱10: 拒绝服务攻击脆弱性

```c
/**
 * ❌ 脆弱点1: 无连接数限制
 */
void vulnerable_no_limit() {
    // 攻击者可以创建大量连接,耗尽文件描述符
    listen(sock, INT_MAX);  // 监听队列过大
    while (1) {
        accept(sock, ...);  // 无上限接受连接
    }
}

// ✅ 防护: 限制并发连接数
void protect_connection_limit() {
    #define MAX_CONNECTIONS 10000
    int connection_count = 0;

    listen(sock, 128);  // 合理的backlog

    while (1) {
        if (connection_count >= MAX_CONNECTIONS) {
            // 拒绝新连接或等待
            sleep(1);
            continue;
        }

        int client = accept(sock, ...);
        if (client >= 0) {
            connection_count++;
            // 处理连接,完成后connection_count--
        }
    }
}

/**
 * ❌ 脆弱点2: Slowloris攻击
 * 攻击者缓慢发送HTTP头,占用连接
 */
// 错误: 无读取超时
void vulnerable_slowloris(int client) {
    char buffer[4096];
    recv(client, buffer, sizeof(buffer), 0);  // 可能永远阻塞!
}

// ✅ 防护: 设置超时
void protect_timeout(int client) {
    struct timeval tv = {5, 0};  // 5秒超时
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[4096];
    int total = 0;
    time_t start = time(NULL);

    while (total < sizeof(buffer) - 1) {
        if (time(NULL) - start > 10) {  // 最多10秒
            close(client);
            return;
        }

        int n = recv(client, buffer + total, 1, 0);  // 逐字节读
        if (n <= 0) break;
        total += n;

        if (strstr(buffer, "\r\n\r\n")) break;  // 头结束
    }
}

/**
 * ❌ 脆弱点3: 内存分配攻击
 */
// 错误: 信任对端发送的长度字段
void vulnerable_memory(int sock) {
    uint32_t len;
    recv(sock, &len, sizeof(len), 0);
    len = ntohl(len);

    char *buffer = malloc(len);  // len可能是0xFFFFFFFF!
    recv(sock, buffer, len, 0);
}

// ✅ 防护: 验证长度
void protect_validate_length(int sock) {
    #define MAX_MSG_SIZE (10 * 1024 * 1024)  // 最大10MB

    uint32_t net_len;
    if (recv(sock, &net_len, sizeof(net_len), 0) != sizeof(net_len)) {
        return;
    }

    uint32_t len = ntohl(net_len);
    if (len > MAX_MSG_SIZE) {
        close(sock);  // 断开恶意连接
        return;
    }

    char *buffer = malloc(len);
    // ...
    free(buffer);
}

/**
 * ❌ 脆弱点4: 无速率限制
 */
// 错误: 无流量控制
void vulnerable_no_rate_limit(int sock) {
    while (1) {
        char buffer[8192];
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        // 处理...
    }
}

// ✅ 防护: 令牌桶算法
#include <time.h>

typedef struct {
    double tokens;      // 当前令牌数
    double rate;        // 令牌产生速率 (字节/秒)
    double capacity;    // 桶容量
    time_t last_time;   // 上次更新时间
} token_bucket_t;

int consume_tokens(token_bucket_t *bucket, int bytes) {
    time_t now = time(NULL);
    double elapsed = difftime(now, bucket->last_time);
    bucket->last_time = now;

    // 添加新产生的令牌
    bucket->tokens += elapsed * bucket->rate;
    if (bucket->tokens > bucket->capacity) {
        bucket->tokens = bucket->capacity;
    }

    // 消费令牌
    if (bucket->tokens < bytes) {
        return -1;  // 超限
    }
    bucket->tokens -= bytes;
    return 0;
}
```

#### 陷阱11: 不正确的错误处理

```c
/**
 * ❌ 错误示例1: 忽略EINTR
 */
void bad_ignore_eintr() {
    int n = recv(sock, buffer, size, 0);
    if (n < 0) {
        perror("recv");  // 如果是EINTR,这是正常的
        close(sock);     // 不应该关闭!
    }
}

// ✅ 正确处理
void good_handle_eintr() {
    ssize_t n;
    do {
        n = recv(sock, buffer, size, 0);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 非阻塞,稍后再试
        } else {
            perror("recv");
            close(sock);
        }
    } else if (n == 0) {
        // 连接关闭
        close(sock);
    }
}

/**
 * ❌ 错误示例2: 部分发送不处理
 */
void bad_partial_send() {
    const char *msg = "Hello World";
    send(sock, msg, strlen(msg), 0);  // 可能只发送了部分数据!
}

// ✅ 正确处理部分发送
ssize_t good_full_send(int sock, const void *buf, size_t len) {
    const char *p = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t n = send(sock, p, remaining, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 等待可写事件(如果使用非阻塞)
                return len - remaining;  // 返回已发送字节数
            }
            return -1;  // 错误
        }
        p += n;
        remaining -= n;
    }
    return len;
}

/**
 * ❌ 错误示例3: 错误的错误码检查顺序
 */
void bad_error_order() {
    close(sock);
    if (errno == ECONNRESET) {  // 错误! close可能重置errno
        // ...
    }
}

// ✅ 正确做法
void good_error_order() {
    int saved_errno = errno;
    close(sock);
    if (saved_errno == ECONNRESET) {
        // ...
    }
}
```

#### 陷阱12: 线程安全问题

```c
/**
 * ❌ 错误示例1: 非线程安全的全局状态
 */
int connection_count = 0;  // 全局变量

void *thread_handler(void *arg) {
    int sock = *(int *)arg;
    connection_count++;  // 非原子操作,数据竞争!
    // ...
    connection_count--;  // 同上
    return NULL;
}

// ✅ 正确做法1: 使用互斥锁
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
int connection_count = 0;

void *thread_handler_safe(void *arg) {
    int sock = *(int *)arg;

    pthread_mutex_lock(&count_mutex);
    connection_count++;
    pthread_mutex_unlock(&count_mutex);

    // ...

    pthread_mutex_lock(&count_mutex);
    connection_count--;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}

// ✅ 正确做法2: 使用原子操作(GCC扩展)
#include <stdatomic.h>
_Atomic int connection_count = 0;

void *thread_handler_atomic(void *arg) {
    atomic_fetch_add(&connection_count, 1);
    // ...
    atomic_fetch_sub(&connection_count, 1);
    return NULL;
}

/**
 * ❌ 错误示例2: 多个线程操作同一个Socket
 */
// 错误: 一个Socket同时被读线程和写线程操作
// 虽然read/write是原子的,但epoll等操作不是

// ✅ 正确做法: Socket分离读/写事件,或使用锁
pthread_mutex_t sock_mutex = PTHREAD_MUTEX_INITIALIZER;

void safe_send(int sock, const void *buf, size_t len) {
    pthread_mutex_lock(&sock_mutex);
    send(sock, buf, len, 0);
    pthread_mutex_unlock(&sock_mutex);
}

/**
 * ❌ 错误示例3: gethostbyname非线程安全
 */
void bad_gethostbyname() {
    // gethostbyname返回静态缓冲区,多线程会冲突
    struct hostent *h = gethostbyname("example.com");
    // h可能被其他线程覆盖!
}

// ✅ 正确做法: 使用线程安全版本
void good_gethostbyname() {
    struct hostent result;
    char buffer[8192];
    struct hostent *h;
    int err;

    gethostbyname_r("example.com", &result, buffer, sizeof(buffer), &h, &err);
    // 或使用 getaddrinfo() - 推荐
}
```

---

## 🧠 6. 思维导图

### 6.1 Socket编程全景图

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                            Socket编程全景图                                      │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  Socket基础                                                                      │
│  ├─ 定义: 网络通信端点抽象 (IP+Port)                                             │
│  ├─ 类型                                                                         │
│  │  ├─ SOCK_STREAM (TCP) ──► 可靠、有序、面向连接                               │
│  │  ├─ SOCK_DGRAM (UDP) ────► 无连接、快速、不可靠                              │
│  │  ├─ SOCK_RAW ────────────► 原始协议访问                                      │
│  │  └─ SOCK_SEQPACKET ──────► 有序包传输(SCTP)                                  │
│  │                                                                              │
│  ├─ 地址族                                                                       │
│  │  ├─ AF_INET (IPv4) ──────► 32位地址, 广泛支持                               │
│  │  ├─ AF_INET6 (IPv6) ─────► 128位地址, 下一代网络                            │
│  │  └─ AF_UNIX/AF_LOCAL ────► 本地IPC, 最高性能                                 │
│  │                                                                              │
│  └─ API生命周期                                                                  │
│     ├─ socket() ──► 创建                                                        │
│     ├─ bind() ────► 绑定地址                                                    │
│     ├─ listen() ──► 开始监听                                                    │
│     ├─ accept() ──► 接受连接                                                    │
│     ├─ connect() ─► 发起连接                                                    │
│     ├─ send/recv ─► 数据传输                                                    │
│     └─ close() ───► 关闭连接                                                    │
│                                                                                 │
│  IO模型                                                                          │
│  ├─ 阻塞IO ─────────────► 简单、低并发                                          │
│  ├─ 非阻塞IO ───────────► 需轮询、复杂                                          │
│  ├─ IO多路复用 ─────────► select/poll/epoll, 高并发首选                         │
│  ├─ 信号驱动IO ─────────► 特殊场景                                              │
│  └─ 异步IO ─────────────► 最高性能、最复杂                                      │
│                                                                                 │
│  并发模型                                                                        │
│  ├─ 迭代服务器 ─────────► 一次处理一个连接                                      │
│  ├─ 多进程(prefork) ────► 稳定、隔离好、开销大                                  │
│  ├─ 多线程 ─────────────► 共享内存、需同步                                      │
│  ├─ 事件驱动(Reactor) ──► epoll + 非阻塞IO, 高性能                              │
│  └─ 异步/Proactor ──────► Windows IOCP, 最高吞吐                                │
│                                                                                 │
│  协议解析                                                                        │
│  ├─ 文本协议 ───────────► HTTP/SMTP/FTP, 易调试                                 │
│  ├─ 二进制协议 ─────────► Protocol Buffers, 高效                                │
│  └─ 消息边界处理                                                                        │
│     ├─ 定长消息                                                                         │
│     ├─ 长度前缀                                                                         │
│     └─ 分隔符                                                                           │
│                                                                                 │
│  性能优化                                                                        │
│  ├─ 零拷贝 ─────────────► sendfile/splice/mmap                                  │
│  ├─ TCP调优 ────────────► 窗口大小、拥塞控制                                      │
│  ├─ 内核参数 ───────────► 缓冲区大小、连接追踪                                    │
│  └─ 架构优化 ───────────► 连接池、批处理、缓存                                    │
│                                                                                 │
│  安全与可靠性                                                                    │
│  ├─ 资源管理 ───────────► 及时close、避免泄漏                                     │
│  ├─ 错误处理 ───────────► EINTR/EAGAIN处理                                        │
│  ├─ 超时控制 ───────────► 连接/读写超时                                           │
│  ├─ 心跳检测 ───────────► 保活、应用层心跳                                        │
│  └─ 安全防护 ───────────► 速率限制、长度验证、Dos防护                             │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 6.2 IO模型选择图

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                          IO模型选择决策图                                        │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│                         开始                                                     │
│                          │                                                      │
│                          ▼                                                      │
│                    ┌─────────────┐                                              │
│                    │ 并发连接数? │                                              │
│                    └──────┬──────┘                                              │
│                           │                                                     │
│              ┌────────────┼────────────┐                                        │
│              ▼            ▼            ▼                                        │
│           少数(<10)    中等(<1K)    大量(>1K)                                    │
│              │            │            │                                        │
│              ▼            ▼            ▼                                        │
│        ┌──────────┐ ┌──────────┐ ┌──────────┐                                   │
│        │ 阻塞IO   │ │ select   │ │ epoll    │                                   │
│        │ 多线程   │ │ poll     │ │ io_uring │                                   │
│        └──────────┘ └──────────┘ └──────────┘                                   │
│              │            │            │                                        │
│              ▼            ▼            ▼                                        │
│         简单应用      兼容性好      最高性能                                      │
│                       跨平台        Linux专用                                   │
│                                                                                 │
│                    其他考虑因素                                                  │
│                    ┌─────────────────────────────────────────┐                  │
│                    │ 需要零拷贝? ──► sendfile/splice          │                  │
│                    │ 跨平台要求? ──► poll/select              │                  │
│                    │ 极高并发?   ──► io_uring/Proactor        │                  │
│                    │ 实时性要求? ──► RT priorities + 专用线程 │                  │
│                    └─────────────────────────────────────────┘                  │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 6.3 服务器架构演进图

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        服务器架构演进图                                          │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  阶段1: 迭代服务器                                                               │
│  ┌─────────────────────────────┐                                                │
│  │ while(1) {                  │                                                │
│  │   client = accept();        │ 特点: 简单,一次处理一个                         │
│  │   handle(client);           │ 并发: 1                                         │
│  │   close(client);            │ 适用: 学习、测试                                │
│  │ }                           │                                                │
│  └─────────────────────────────┘                                                │
│           │                                                                     │
│           ▼                                                                     │
│  阶段2: 多进程服务器                                                             │
│  ┌─────────────────────────────┐                                                │
│  │ while(1) {                  │                                                │
│  │   client = accept();        │ 特点: 进程隔离,稳定                             │
│  │   if(fork() == 0) {         │ 并发: 进程数受限                                │
│  │     handle(client);         │ 问题: fork开销大,惊群                           │
│  │     exit();                 │ 适用: CGI、隔离要求高                           │
│  │   }                         │                                                │
│  │ }                           │                                                │
│  └─────────────────────────────┘                                                │
│           │                                                                     │
│           ▼                                                                     │
│  阶段3: 多线程服务器                                                             │
│  ├─────────────────────────────┐                                                │
│  │ while(1) {                  │                                                │
│  │   client = accept();        │ 特点: 共享内存,轻量                             │
│  │   pthread_create(handle);   │ 并发: 线程数受限                                │
│  │ }                           │ 问题: 锁竞争,线程开销                           │
│  └─────────────────────────────┘ 适用: 中等并发                                 │
│           │                                                                     │
│           ▼                                                                     │
│  阶段4: 事件驱动 + 线程池                                                        │
│  ┌─────────────────────────────┐                                                │
│  │ epoll_wait();               │                                                │
│  │ for each event:             │ 特点: 单线程事件循环                            │
│  │   if(accept) add to epoll   │ 并发: 数万连接                                  │
│  │   if(read) handle in thread │ 问题: 多核利用                                  │
│  └─────────────────────────────┘ 适用: Nginx早期,Redis                           │
│           │                                                                     │
│           ▼                                                                     │
│  阶段5: 多Reactor + 线程池                                                       │
│  ┌─────────────────────────────┐                                                │
│  │ Main Reactor (accept)       │                                                │
│  │   └──► Sub Reactor 1 (epoll)│ 特点: 多核充分利用                              │
│  │   └──► Sub Reactor 2 (epoll)│ 并发: 百万连接                                  │
│  │   └──► Sub Reactor N (epoll)│ 代表: Nginx,Netty                               │
│  │       └──► Thread Pool      │                                                │
│  └─────────────────────────────┘                                                │
│           │                                                                     │
│           ▼                                                                     │
│  阶段6: io_uring / 异步IO                                                        │
│  ┌─────────────────────────────┐                                                │
│  │ io_uring_submit();          │                                                │
│  │ io_uring_wait_cqe();        │ 特点: 内核队列,零拷贝                           │
│  │ process completions         │ 并发: 百万+                                     │
│  └─────────────────────────────┘ 代表: 下一代高性能服务器                        │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 🌲 7. 决策树

### 7.1 Socket类型选择决策树

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        Socket类型选择决策树                                      │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│                              需要可靠传输?                                        │
│                                   │                                             │
│                    ┌──────────────┴──────────────┐                              │
│                    │是                            │否                           │
│                    ▼                              ▼                              │
│              需要有序?                      需要多播?                            │
│                 │                               │                               │
│        ┌────────┴────────┐            ┌─────────┴──────────┐                    │
│        │是               │否           │是                  │否                  │
│        ▼                 ▼             ▼                    ▼                    │
│  需要流式数据?      使用SCTP      使用UDP            极低延迟要求?               │
│       │                            (广播/组播)            │                      │
│  ┌────┴────┐                                            │                      │
│  │是       │否                                           │                      │
│  ▼         ▼                                            │                      │
│  TCP      SCTP                              ┌───────────┴───────────┐          │
│ (HTTP等) (信令)                             │是                     │否        │
│                                             ▼                       ▼          │
│                                        使用UDP +         评估TCP是否可行       │
│                                        应用层重传        (连接维护开销)        │
│                                                                                │
│  特殊情况:                                                                     │
│  • 同一主机进程间通信 ──► AF_UNIX (比TCP快30%+)                               │
│  • 需要原始协议访问 ────► SOCK_RAW (ping, OSPF)                               │
│  • 需要内核旁路 ────────► DPDK/RDMA (绕过内核协议栈)                          │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 7.2 IO模型选择决策树

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         IO模型选择决策树                                         │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│                          并发连接数预期?                                         │
│                               │                                                 │
│              ┌────────────────┼────────────────┐                                │
│              ▼                ▼                ▼                                │
│           < 100            100-1000           > 1000                            │
│              │                │                │                                │
│              ▼                ▼                ▼                                │
│      ┌──────────────┐ ┌──────────────┐ ┌──────────────┐                         │
│      │ 阻塞IO +     │ │ select/poll  │ │ epoll/kqueue │                         │
│      │ 多线程/进程  │ │ 多路复用     │ │ io_uring     │                         │
│      └──────────────┘ └──────────────┘ └──────────────┘                         │
│              │                │                │                                │
│     简单应用/快速原型   跨平台兼容需求    Linux高性能需求                         │
│                                                                                 │
│  进一步选择:                                                                    │
│                                                                                 │
│  是否使用Linux?                                                                 │
│       │                                                                         │
│  ┌────┴────┐                                                                    │
│  │是       │否                                                                   │
│  ▼         ▼                                                                    │
│  内核版本?  使用kqueue(macOS/BSD)                                               │
│       │                                                                         │
│  ┌────┴──────────┐                                                              │
│  < 5.1          >= 5.1                                                          │
│   │               │                                                             │
│   ▼               ▼                                                             │
│  epoll        io_uring                                                          │
│  (成熟稳定)   (最新高性能)                                                      │
│                                                                                 │
│  特殊场景:                                                                      │
│  • 跨平台要求高 ──► libevent/libuv 抽象层                                      │
│  • Windows环境 ───► IOCP (Proactor模式)                                        │
│  • 实时性要求 ────► 专用线程 + 忙轮询                                          │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

### 7.3 并发策略选择决策树

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        并发策略选择决策树                                        │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│                          是否需要共享状态?                                       │
│                               │                                                 │
│              ┌────────────────┴────────────────┐                                │
│              │是                               │否                               │
│              ▼                                 ▼                                │
│      状态复杂度?                       是否需要进程隔离?                        │
│           │                                    │                                │
│  ┌────────┴────────┐                  ┌────────┴────────┐                       │
│  简单               复杂               │是               │否                      │
│   │                 │                 ▼                 ▼                       │
│   ▼                 ▼            多进程              事件驱动                   │
│  多线程         Reactor +       (pre-fork)        (单线程epoll)                 │
│  + 锁           业务线程池      (Apache风格)      (Nginx风格)                   │
│                                                                                 │
│  复杂状态共享方案对比:                                                          │
│                                                                                 │
│  锁粒度选择:                                                                    │
│       │                                                                         │
│  ┌────┴──────────────┐                                                          │
│  粗粒度锁           细粒度锁                                                    │
│   │                   │                                                         │
│   ▼                   ▼                                                         │
│  简单但           复杂但                                                        │
│  竞争大           性能高                                                        │
│                                                                                 │
│  替代方案:                                                                      │
│  • Actor模型 ────► 无共享,消息传递 (Erlang/Go)                                 │
│  • 无锁数据结构 ──► ring buffer, lock-free queue                               │
│  • Thread-local ──► 每个线程独立状态                                           │
│  • 进程隔离 ──────► 共享状态通过IPC                                             │
│                                                                                 │
│  性能与复杂度权衡:                                                              │
│                                                                                 │
│  复杂度 ─────────────────────────────► 性能                                     │
│  │                                                                   │         │
│  │  单线程 ──► 多线程 ──► 线程池 ──► 多Reactor ──► 异步IO           │         │
│  │    低        中         中高         高            极高            │         │
│  │                                                                   │         │
│  │  建议演进路径:                                                    │         │
│  │  先简单 ──► 度量 ──► 优化瓶颈                                     │         │
│  │                                                                   │         │
│  └───────────────────────────────────────────────────────────────────┘         │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## ⚡ 8. 性能优化

### 8.1 TCP调优参数

#### 连接建立优化

| 参数 | 默认值 | 推荐值 | 说明 | 设置方式 |
|:-----|:-------|:-------|:-----|:---------|
| **tcp_syn_retries** | 5 | 2 | SYN重试次数 | sysctl |
| **tcp_synack_retries** | 5 | 2 | SYN+ACK重试次数 | sysctl |
| **tcp_max_syn_backlog** | 512 | 65536 | SYN队列长度 | sysctl |
| **tcp_abort_on_overflow** | 0 | 0 | 队列满时是否reset | sysctl |
| **tcp_fastopen** | 0 | 3 | TFO(TCP Fast Open) | sysctl |

#### 连接维护优化

| 参数 | 默认值 | 推荐值 | 说明 | 设置方式 |
|:-----|:-------|:-------|:-----|:---------|
| **tcp_keepalive_time** | 7200 | 600 | 首次探测前空闲时间 | sysctl |
| **tcp_keepalive_intvl** | 75 | 30 | 探测间隔 | sysctl |
| **tcp_keepalive_probes** | 9 | 3 | 探测次数 | sysctl |
| **tcp_retries2** | 15 | 5 | 数据段重传次数 | sysctl |
| **tcp_orphan_retries** | 0 | 1 |孤儿连接重试 | sysctl |

#### 传输优化

| 参数 | 默认值 | 推荐值 | 说明 | 设置方式 |
|:-----|:-------|:-------|:-----|:---------|
| **tcp_window_scaling** | 1 | 1 | 窗口缩放选项 | sysctl |
| **tcp_rmem** | 4K-8M | 4K-16M | 接收缓冲区范围 | sysctl |
| **tcp_wmem** | 4K-8M | 4K-16M | 发送缓冲区范围 | sysctl |
| **tcp_mem** | 根据内存 | 3%-6% | TCP全局内存页数 | sysctl |
| **tcp_congestion_control** | cubic | bbr | 拥塞控制算法 | sysctl |
| **tcp_notsent_lowat** | -1 | 16384 | 未发送数据低水位 | setsockopt |
| **tcp_thin_linear_timeouts** | 0 | 1 | 精简超时(短流) | setsockopt |

#### 代码设置示例

```c
/**
 * tcp_optimize.c - TCP优化参数设置
 */
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

void optimize_tcp_socket(int sockfd) {
    int opt;

    // 1. TCP_NODELAY - 禁用Nagle算法,降低延迟
    // 适用: 交互式应用、游戏、高频交易
    opt = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    // 2. TCP_QUICKACK - 立即发送ACK,不延迟
    // 适用: 需要快速响应的场景
    opt = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, &opt, sizeof(opt));

    // 3. TCP_CORK - 合并小数据包(与NODELAY互斥)
    // 适用: 批量数据传输,如文件上传
    // opt = 1;
    // setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &opt, sizeof(opt));

    // 4. SO_RCVBUF/SO_SNDBUF - 增大缓冲区
    // 注意: 需要在listen/connect之前设置
    int recvbuf = 256 * 1024;  // 256KB
    int sendbuf = 256 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, sizeof(recvbuf));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));

    // 5. TCP_KEEPALIVE - 连接保活
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    int idle = 60;      // 60秒无数据开始探测
    int interval = 10;  // 10秒间隔
    int count = 3;      // 3次无响应断开
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));

    // 6. TCP_USER_TIMEOUT - 用户态超时(应用层控制)
    unsigned int timeout = 20000;  // 20秒
    setsockopt(sockfd, IPPROTO_TCP, TCP_USER_TIMEOUT, &timeout, sizeof(timeout));

    // 7. SO_REUSEPORT - 端口复用(负载均衡)
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 8. TCP_FASTOPEN - 快速打开
    opt = 5;  // TFO队列长度
    setsockopt(sockfd, SOL_TCP, TCP_FASTOPEN, &opt, sizeof(opt));
}

void optimize_listen_socket(int listenfd) {
    // 设置监听队列长度
    // listen(listenfd, SOMAXCONN);  // 使用系统最大值
    listen(listenfd, 65535);  // 或显式设置

    // SO_REUSEADDR必须在bind之前设置
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}
```

### 8.2 高并发优化技巧

#### 系统级优化

```bash
#!/bin/bash
# optimize_sysctl.sh - 系统级网络优化

# ===== 文件描述符限制 =====
# /etc/security/limits.conf
# * soft nofile 1048576
# * hard nofile 1048576
# * soft nproc 1048576
# * hard nproc 1048576

# 当前会话生效
ulimit -n 1048576

# ===== 内核网络参数优化 =====

# 1. 连接队列
sysctl -w net.core.somaxconn=65535
sysctl -w net.core.netdev_max_backlog=65535
sysctl -w net.ipv4.tcp_max_syn_backlog=65535

# 2. 端口范围
sysctl -w net.ipv4.ip_local_port_range="1024 65535"

# 3. TIME_WAIT优化
sysctl -w net.ipv4.tcp_tw_reuse=1
sysctl -w net.ipv4.tcp_fin_timeout=10
sysctl -w net.ipv4.tcp_max_tw_buckets=2000000

# 4. 连接保活
sysctl -w net.ipv4.tcp_keepalive_time=300
sysctl -w net.ipv4.tcp_keepalive_intvl=30
sysctl -w net.ipv4.tcp_keepalive_probes=3

# 5. 内存优化
sysctl -w net.core.rmem_default=262144
sysctl -w net.core.wmem_default=262144
sysctl -w net.core.rmem_max=16777216
sysctl -w net.core.wmem_max=16777216
sysctl -w net.ipv4.tcp_rmem="4096 87380 16777216"
sysctl -w net.ipv4.tcp_wmem="4096 65536 16777216"
sysctl -w net.ipv4.tcp_mem="16777216 16777216 16777216"

# 6. 拥塞控制
sysctl -w net.ipv4.tcp_congestion_control=bbr
sysctl -w net.core.default_qdisc=fq

# 7. 连接追踪优化(如果启用iptables)
sysctl -w net.netfilter.nf_conntrack_max=1000000
sysctl -w net.netfilter.nf_conntrack_tcp_timeout_established=600

# 8. 禁用IPv6(如果不使用)
# sysctl -w net.ipv6.conf.all.disable_ipv6=1

echo "网络优化完成"
```

#### 应用级优化

```c
/**
 * high_perf_server.c - 高并发服务器优化技巧
 */

// 1. 使用SO_REUSEPORT实现内核级负载均衡
void setup_reuseport(int listen_fd, int num_workers) {
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // 每个工作进程创建自己的listen socket
    // 内核自动分配新连接到不同进程
}

// 2. 使用accept4节省一次系统调用
#include <sys/syscall.h>

int optimized_accept(int listen_fd) {
    // accept4可以一次性设置SOCK_NONBLOCK和SOCK_CLOEXEC
    int client_fd = accept4(listen_fd, NULL, NULL,
                            SOCK_NONBLOCK | SOCK_CLOEXEC);
    return client_fd;
}

// 3. 使用mmap优化缓冲区分配
#include <sys/mman.h>

void *allocate_buffer_pool(size_t size) {
    // 使用mmap大页内存减少TLB miss
    void *pool = mmap(NULL, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (pool == MAP_FAILED) {
        // 回退到普通malloc
        pool = malloc(size);
    }
    return pool;
}

// 4. 使用批处理减少系统调用
void batch_recv(int epoll_fd, struct epoll_event *events, int maxevents) {
    // 一次性获取所有就绪事件
    int nfds = epoll_wait(epoll_fd, events, maxevents, 0);  // 非阻塞

    // 批量处理读事件
    for (int i = 0; i < nfds; i++) {
        if (events[i].events & EPOLLIN) {
            // 批量读取
            int fd = events[i].data.fd;
            // 尽可能多读,直到EAGAIN
            while (1) {
                char buffer[8192];
                ssize_t n = recv(fd, buffer, sizeof(buffer), 0);
                if (n < 0) {
                    if (errno == EAGAIN) break;
                    // 错误处理
                }
                // 处理数据
            }
        }
    }
}

// 5. 使用环形缓冲区减少拷贝
typedef struct {
    char *buffer;
    size_t size;
    size_t head;
    size_t tail;
} ring_buffer_t;

int ring_buffer_write(ring_buffer_t *rb, const char *data, size_t len) {
    size_t available = (rb->head - rb->tail - 1) & (rb->size - 1);
    if (len > available) return -1;  // 空间不足

    for (size_t i = 0; i < len; i++) {
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) & (rb->size - 1);
    }
    return 0;
}

// 6. CPU亲和性绑定
#include <sched.h>

void bind_to_cpu(int cpu_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

// 7. 使用忙轮询(Busy Polling)降低延迟(Linux 3.11+)
void enable_busy_polling(int sockfd) {
    int busy_poll = 50;      // 忙轮询超时(微秒)
    int busy_budget = 64;    // 每次轮询处理的包数

    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL,
               &busy_poll, sizeof(busy_poll));
    setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL_BUDGET,
               &busy_budget, sizeof(busy_budget));
}
```

#### 性能测试与监控

```c
/**
 * perf_monitor.c - 性能监控工具
 */
#include <time.h>
#include <stdio.h>

// 高精度计时器
typedef struct {
    struct timespec start;
    struct timespec end;
} timer_t;

void timer_start(timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->start);
}

double timer_elapsed_us(timer_t *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->end);
    return (t->end.tv_sec - t->start.tv_sec) * 1e6 +
           (t->end.tv_nsec - t->start.tv_nsec) / 1e3;
}

// 性能指标统计
typedef struct {
    unsigned long total_requests;
    unsigned long total_bytes_sent;
    unsigned long total_bytes_recv;
    double total_latency_us;
    unsigned long error_count;
    time_t start_time;
} perf_stats_t;

void print_stats(perf_stats_t *stats) {
    time_t elapsed = time(NULL) - stats->start_time;
    if (elapsed == 0) return;

    printf("========== Performance Statistics ==========\n");
    printf("Runtime: %ld seconds\n", elapsed);
    printf("Total Requests: %lu\n", stats->total_requests);
    printf("Requests/sec: %.2f\n", (double)stats->total_requests / elapsed);
    printf("Avg Latency: %.2f us\n",
           stats->total_requests ? stats->total_latency_us / stats->total_requests : 0);
    printf("Error Rate: %.4f%%\n",
           stats->total_requests ? 100.0 * stats->error_count / stats->total_requests : 0);
    printf("Throughput: %.2f MB/s\n",
           (double)(stats->total_bytes_sent + stats->total_bytes_recv) / elapsed / 1024 / 1024);
    printf("============================================\n");
}

// Linux性能监控: /proc/net/sockstat
define PROC_SOCKSTAT "/proc/net/sockstat"

void monitor_sockets() {
    FILE *fp = fopen(PROC_SOCKSTAT, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

// 连接状态统计
void monitor_tcp_states() {
    system("netstat -an | awk '/^tcp/ {++S[$NF]} END {for(a in S) print a, S[a]}'");
}
```

### 8.3 内核参数优化速查表

```bash
# ===== /etc/sysctl.conf 推荐配置 =====

# 网络核心参数
net.core.somaxconn = 65535
net.core.netdev_max_backlog = 65535
net.core.rmem_default = 262144
net.core.wmem_default = 262144
net.core.rmem_max = 16777216
net.core.wmem_max = 16777216

# TCP参数
net.ipv4.tcp_max_syn_backlog = 65535
net.ipv4.tcp_syncookies = 1
net.ipv4.tcp_syn_retries = 2
net.ipv4.tcp_synack_retries = 2
net.ipv4.tcp_fin_timeout = 10
net.ipv4.tcp_keepalive_time = 300
net.ipv4.tcp_keepalive_probes = 3
net.ipv4.tcp_keepalive_intvl = 30
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_tw_recycle = 0  # 已废弃
net.ipv4.tcp_max_tw_buckets = 2000000
net.ipv4.ip_local_port_range = 1024 65535
net.ipv4.tcp_rmem = 4096 87380 16777216
net.ipv4.tcp_wmem = 4096 65536 16777216
net.ipv4.tcp_mem = 16777216 16777216 16777216
net.ipv4.tcp_congestion_control = bbr
net.ipv4.tcp_notsent_lowat = 16384
net.ipv4.tcp_no_metrics_save = 1
net.ipv4.tcp_moderate_rcvbuf = 1
net.ipv4.tcp_slow_start_after_idle = 0

# 内存参数
vm.swappiness = 10
vm.dirty_ratio = 40
vm.dirty_background_ratio = 10

# 文件描述符
fs.file-max = 2097152
fs.nr_open = 2097152

# 应用后执行: sysctl -p
```

---

## ✅ 质量验收清单

- [x] Socket严格定义与协议栈模型
- [x] TCP状态机详细描述
- [x] 端口和地址的数学表示
- [x] Socket类型对比矩阵
- [x] Socket选项矩阵(SOL_SOCKET/IP/TCP级别)
- [x] IO模型对比矩阵(5种模型)
- [x] 地址族对比矩阵
- [x] 并发模型对比矩阵
- [x] 错误码矩阵
- [x] TCP状态转换图(详细)
- [x] Socket API状态机
- [x] 三次握手/四次挥时时序图
- [x] 完整TCP客户端(含超时、错误处理)
- [x] 迭代TCP服务器
- [x] UDP客户端/服务器
- [x] UDP广播/多播
- [x] 多进程并发服务器
- [x] 多线程并发服务器
- [x] select多路复用服务器
- [x] epoll服务器(LT/ET模式)
- [x] Reactor模式实现
- [x] 简单HTTP服务器
- [x] 非阻塞IO实现
- [x] 零拷贝传输(sendfile/splice/mmap)
- [x] 陷阱01-12 (SIGPIPE、缓冲区溢出、粘包、字节序、资源泄漏、僵尸进程、TIME_WAIT、惊群、半开连接、DoS、错误处理、线程安全)
- [x] Socket编程全景图
- [x] IO模型选择图
- [x] 服务器架构演进图
- [x] Socket类型选择决策树
- [x] IO模型选择决策树
- [x] 并发策略选择决策树
- [x] TCP调优参数
- [x] 高并发优化技巧
- [x] 内核参数优化速查表

---

> **更新记录**
>
> - 2025-03-09: 创建基础版本
> - 2025-03-16: 工程级深化,添加完整概念定义、8个维度矩阵、形式化描述、7类示例、12个陷阱、3个思维导图、3个决策树、性能优化章节
