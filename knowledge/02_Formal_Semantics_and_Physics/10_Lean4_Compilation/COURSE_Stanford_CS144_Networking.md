# Stanford CS144: Computer Networking

> **层级**: L6++ (极致深度)  
> **关联**: 网络协议形式化与类型安全

---

## 课程概述

Stanford CS144涵盖计算机网络的核心原理：
- 互联网架构与设计原则
- 传输层 (TCP/UDP)
- 网络层 (IP, 路由)
- 链路层
- 网络安全基础
- 协议设计与实现

---

## 网络协议形式化

### 1.1 协议状态机

**定义1.1 (协议状态机)**:
```
Protocol = (States, Events, Transitions, Initial, Final)
```

**TCP状态机**:
```
CLOSED → SYN_SENT → ESTABLISHED → FIN_WAIT_1 → FIN_WAIT_2 → TIME_WAIT → CLOSED
       ↘ SYN_RCVD → ESTABLISHED ↗
```

### 1.2 协议正确性

**定理1.1 (TCP握手安全性)**:
```
三次握手建立唯一连接标识
```

**证明**: 序列号机制防止旧连接干扰

---

## 传输层形式化

### 2.1 可靠传输

**定理2.1 (停止等待协议正确性)**:
```
在无丢包网络中，停止等待协议可靠交付
```

**定理2.2 (滑动窗口优化)**:
```
窗口大小W的吞吐量 = min(W/RTT, 链路带宽)
```

### 2.2 TCP拥塞控制

**AIMD算法**:
```
cwnd_{t+1} = { cwnd_t + 1/cwnd_t  if ACK
             { cwnd_t / 2       if loss
```

**定理2.3 (TCP公平性)**:
```
在相同RTT的流之间，带宽均分
```

---

## 网络层形式化

### 3.1 路由算法

**定理3.1 (Dijkstra最短路径)**:
```
链路状态协议收敛到最短路径树
```

**定理3.2 (距离向量收敛)**:
```
在无计数到无穷情况下，距离向量收敛
```

### 3.2 IP协议

**定理3.3 (IP分片重组)**:
```
所有分片到达 ⟹ 成功重组原始数据报
```

---

## 网络安全

### 4.1 加密基础

**定理4.1 (TLS握手安全性)**:
```
TLS 1.3握手提供前向安全和身份认证
```

### 4.2 攻击防护

**定理4.2 (SYN Cookie防护)**:
```
SYN Flood攻击无法耗尽服务器资源
```

---

## 协议实现

### 5.1 类型安全网络

**定理5.1 (类型安全协议)**:
```
Lean类型系统防止协议状态错误
```

**示例**: 状态相关类型
```lean
inductive TCPState
| Closed | SynSent | Established | FinWait

def send (s : TCPState.Established) (data : Bytes) : IO Unit
```

---

## 参考资源

- **教材**: "Computer Networking: A Top-Down Approach" (Kurose & Ross)
- **课程**: Stanford CS144 Fall 2023
- **项目**: 实现完整TCP/IP协议栈
