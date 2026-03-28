# Modbus 功能码深度解析

> **标准**: Modbus Application Protocol V1.1b3
> **功能码范围**: 0x01-0x7F (0x80-0xFF为异常响应)
> **最后更新**: 2026-03-29

---

## 1. 功能码形式化分类体系

### 1.1 功能码层次结构

```
┌─────────────────────────────────────────────────────────────────┐
│                  Modbus 功能码分类体系                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  功能码空间: 0x01 - 0x7F (127个公共功能码)                       │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ 数据访问类 (Data Access) - 主要功能码                    │   │
│  │ ┌─────────────────────────────────────────────────────┐ │   │
│  │ │ 位操作 (Discrete)                                    │ │   │
│  │ │ ├── 0x01: Read Coils (读线圈)                        │ │   │
│  │ │ ├── 0x02: Read Discrete Inputs (读离散输入)          │ │   │
│  │ │ ├── 0x05: Write Single Coil (写单线圈)               │ │   │
│  │ │ └── 0x0F: Write Multiple Coils (写多线圈)            │ │   │
│  │ ├─────────────────────────────────────────────────────┤ │   │
│  │ │ 16位寄存器操作 (16-bit Registers)                    │ │   │
│  │ │ ├── 0x03: Read Holding Registers (读保持寄存器)      │ │   │
│  │ │ ├── 0x04: Read Input Registers (读输入寄存器)        │ │   │
│  │ │ ├── 0x06: Write Single Register (写单寄存器)         │ │   │
│  │ │ └── 0x10: Write Multiple Registers (写多寄存器)      │ │   │
│  │ └─────────────────────────────────────────────────────┘ │   │
│  ├─────────────────────────────────────────────────────────┤   │
│  │ 诊断类 (Diagnostics)                                     │   │
│  │ ├── 0x08: Diagnostics (诊断)                           │   │
│  │ └── 0x11: Get Comm Event Counter (获取通信事件计数器)   │   │
│  ├─────────────────────────────────────────────────────────┤   │
│  │ 文件访问类 (File Access)                                 │   │
│  │ ├── 0x14: Read File Record (读文件记录)                 │   │
│  │ └── 0x15: Write File Record (写文件记录)                │   │
│  ├─────────────────────────────────────────────────────────┤   │
│  │ 封装接口类 (Encapsulated Interface)                      │   │
│  │ └── 0x2B: Encapsulated Transport (封装传输)             │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 功能码属性矩阵

| 功能码 | 名称 | 数据模型 | 访问类型 | 数据长度 | 复杂度 | 常用度 |
|--------|------|----------|----------|----------|--------|--------|
| **0x01** | Read Coils | 线圈 | 读 | 1-2000位 | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **0x02** | Read Discrete Inputs | 离散输入 | 读 | 1-2000位 | ⭐⭐ | ⭐⭐⭐⭐ |
| **0x03** | Read Holding Registers | 保持寄存器 | 读 | 1-125字 | ⭐⭐ | ⭐⭐⭐⭐⭐ |
| **0x04** | Read Input Registers | 输入寄存器 | 读 | 1-125字 | ⭐⭐ | ⭐⭐⭐⭐ |
| **0x05** | Write Single Coil | 线圈 | 写 | 1位 | ⭐ | ⭐⭐⭐⭐ |
| **0x06** | Write Single Register | 保持寄存器 | 写 | 1字 | ⭐ | ⭐⭐⭐⭐⭐ |
| **0x0F** | Write Multiple Coils | 线圈 | 写 | 1-1968位 | ⭐⭐⭐ | ⭐⭐⭐ |
| **0x10** | Write Multiple Registers | 保持寄存器 | 写 | 1-123字 | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| **0x17** | Read/Write Multiple Registers | 保持寄存器 | 读写 | 读1-125/写1-121 | ⭐⭐⭐⭐ | ⭐⭐ |

---

## 2. 读操作功能码详解

### 2.1 Read Coils (0x01) - 读线圈

**形式化定义**

```
请求帧:  [Address:1] [0x01:1] [StartAddr:2] [Quantity:2]
响应帧:  [Address:1] [0x01:1] [ByteCount:1] [CoilStatus:N]

其中:
  StartAddr ∈ [0x0000, 0xFFFF]  (起始地址)
  Quantity ∈ [0x0001, 0x07D0]   (线圈数量, 1-2000)
  N = ceil(Quantity / 8)         (状态字节数)

约束条件:
  StartAddr + Quantity ≤ 0x10000 (地址溢出检查)
```

**数据编码定理**

```
定理 2.1 (线圈状态编码)
给定: n个线圈状态 C[0], C[1], ..., C[n-1], C[i] ∈ {0, 1}

编码规则:
  Byte[j] = Σ(C[i] << (i mod 8))
           其中 i ∈ [j×8, min((j+1)×8-1, n-1)]
           j ∈ [0, ceil(n/8)-1]

即: 每个字节包含8个线圈状态，LSB对应低地址线圈

示例:
  线圈地址: 00001-00008, 状态: ON, OFF, ON, ON, OFF, ON, OFF, OFF
  编码:     [0, 1, 0, 1, 1, 0, 1, 0] (从bit0到bit7)
  结果字节: 0x5A (01011010)
```

**C语言实现**

```c
/* 线圈状态打包 */
int pack_coils(const uint8_t *coils, uint16_t quantity, uint8_t *output) {
    if (quantity == 0 || quantity > 2000) {
        return ERROR_ILLEGAL_DATA_VALUE;
    }

    uint16_t byte_count = (quantity + 7) / 8;
    memset(output, 0, byte_count);

    for (uint16_t i = 0; i < quantity; i++) {
        if (coils[i]) {
            output[i / 8] |= (1 << (i % 8));
        }
    }

    return byte_count;
}

/* 线圈状态解包 */
void unpack_coils(const uint8_t *input, uint16_t quantity, uint8_t *coils) {
    for (uint16_t i = 0; i < quantity; i++) {
        coils[i] = (input[i / 8] >> (i % 8)) & 0x01;
    }
}
```

### 2.2 Read Holding Registers (0x03) - 读保持寄存器

**形式化定义**

```
请求帧:  [Address:1] [0x03:1] [StartAddr:2] [Quantity:2]
响应帧:  [Address:1] [0x03:1] [ByteCount:1] [RegisterValues:2×Quantity]

其中:
  Quantity ∈ [0x0001, 0x007D] (寄存器数量, 1-125)
  ByteCount = Quantity × 2

字节序: 大端序 (Big-Endian)
  寄存器值 0x1234 → 传输字节: [0x12] [0x34]
```

**数据类型映射矩阵**

| 数据类型 | C类型 | 寄存器数 | 字节序 | 转换函数 |
|----------|-------|----------|--------|----------|
| **UINT16** | `uint16_t` | 1 | BE | `ntohs()` |
| **INT16** | `int16_t` | 1 | BE | `ntohs()` + 符号扩展 |
| **UINT32** | `uint32_t` | 2 | BE | `(hi << 16) \| lo` |
| **INT32** | `int32_t` | 2 | BE | 同上 + 符号扩展 |
| **FLOAT32** | `float` | 2 | BE | `memcpy` + 解释 |
| **STRING** | `char[]` | n | BE | 直接复制 |

**字节序转换定理**

```
定理 2.2 (大端序转换)
设主机为小端序，寄存器数组 R[0..n-1] 需要传输

转换函数:
  对于每个寄存器 R[i] = 0xABCD:
    TxBuffer[2i]   = (R[i] >> 8) & 0xFF  // 0xAB
    TxBuffer[2i+1] = R[i] & 0xFF          // 0xCD

接收转换:
  R[i] = (RxBuffer[2i] << 8) | RxBuffer[2i+1]

证明: 转换后高字节在前，符合Modbus大端序要求。
```

---

## 3. 写操作功能码详解

### 3.1 Write Single Coil (0x05) - 写单线圈

**形式化定义**

```
请求帧:  [Address:1] [0x05:1] [CoilAddr:2] [CoilValue:2]
响应帧:  [Address:1] [0x05:1] [CoilAddr:2] [CoilValue:2]

线圈值编码:
  0xFF00 = ON (线圈闭合)
  0x0000 = OFF (线圈断开)
  其他值 = 非法数据异常 (0x03)

特点: 请求与响应完全相同 (回显)
```

**决策判定树**

```
写单线圈操作判定
│
├── 地址合法性检查
│   ├── 地址 > 最大线圈地址?
│   │   └── YES → 异常 0x02 (ILLEGAL_DATA_ADDRESS)
│   └── NO → 继续
│
├── 值合法性检查
│   ├── 值 == 0xFF00 或 值 == 0x0000?
│   │   └── NO → 异常 0x03 (ILLEGAL_DATA_VALUE)
│   └── YES → 继续
│
├── 设备状态检查
│   ├── 设备忙?
│   │   └── YES → 异常 0x06 (SLAVE_DEVICE_BUSY)
│   └── NO → 继续
│
└── 执行写入
    ├── 更新线圈状态
    ├── 触发关联动作 (如继电器)
    └── 发送正常响应
```

### 3.2 Write Multiple Registers (0x10) - 写多寄存器

**形式化定义**

```
请求帧:  [Address:1] [0x10:1] [StartAddr:2] [Quantity:2] [ByteCount:1] [Values:2×Quantity]
响应帧:  [Address:1] [0x10:1] [StartAddr:2] [Quantity:2]

约束:
  Quantity ∈ [0x0001, 0x007B] (1-123个寄存器)
  ByteCount = Quantity × 2
  ByteCount ≤ 246 (ADU长度限制)
```

**批量写入优化分析**

```
比较: 单寄存器写入 vs 批量写入

场景: 写入100个连续寄存器

方案1: 100次 0x06 写入
├── 总字节数: 100 × (8字节请求 + 8字节响应) = 1600字节
├── 总时间: 100 × (传输时间 + 处理时间 + 等待时间)
└── 可靠性: 100个独立事务，任一中断导致部分更新

方案2: 1次 0x10 批量写入
├── 总字节数: 1 × (7+200字节请求 + 8字节响应) = 215字节
├── 总时间: 单次长传输，总时间减少约85%
└── 可靠性: 原子操作，全部成功或全部失败

结论: 批量写入效率提升约7倍
```

---

## 4. 功能码组合策略

### 4.1 读写组合决策树

```
设备数据访问策略选择
│
├── 只需要读?
│   ├── 读线圈/离散量? → 0x01 / 0x02
│   └── 读寄存器? → 0x03 / 0x04
│
├── 只需要写?
│   ├── 写单个? → 0x05 / 0x06
│   └── 写多个? → 0x0F / 0x10
│
├── 需要读后写 (原子操作)?
│   └── 同一设备? → 0x17 (Read/Write Multiple Registers)
│       └── 可同时读取和写入保持寄存器，保证原子性
│
└── 需要监控变化?
    └── 轮询策略
        ├── 简单轮询: 固定间隔读取
        └── 优化轮询: 基于事件计数器 (0x0B)
```

### 4.2 异常响应处理矩阵

| 功能码 | 可能异常 | 异常码 | 处理策略 |
|--------|----------|--------|----------|
| 0x01 | 非法地址 | 0x02 | 检查地址范围 |
| 0x01 | 非法数量 | 0x03 | 限制为1-2000 |
| 0x03 | 非法地址 | 0x02 | 检查寄存器映射 |
| 0x05 | 非法值 | 0x03 | 只接受0xFF00/0x0000 |
| 0x10 | 非法数量 | 0x03 | 限制为1-123 |
| 0x10 | 设备忙 | 0x06 | 延时重试 |
| 任意 | 非法功能 | 0x01 | 检查功能码支持 |
| 任意 | 设备故障 | 0x04 | 硬件诊断 |

---

## 5. C语言实现框架

### 5.1 功能码处理状态机

```c
/* 功能码处理函数表 */
typedef int (*FunctionHandler)(ModbusFrame *req, ModbusFrame *resp);

typedef struct {
    uint8_t code;
    FunctionHandler handler;
    uint8_t min_data_len;
    uint8_t max_data_len;
} FunctionCodeEntry;

/* 功能码表 */
static const FunctionCodeEntry func_table[] = {
    {0x01, handle_read_coils,         4,  4},
    {0x02, handle_read_discrete,      4,  4},
    {0x03, handle_read_holding,       4,  4},
    {0x04, handle_read_input,         4,  4},
    {0x05, handle_write_coil,         4,  4},
    {0x06, handle_write_register,     4,  4},
    {0x0F, handle_write_coils,        5,  253},
    {0x10, handle_write_registers,    5,  253},
    {0x17, handle_read_write_regs,    9,  253},
    {0x00, NULL, 0, 0}  /* 终止符 */
};

/* 功能码分发 */
int process_function_code(ModbusFrame *req, ModbusFrame *resp) {
    uint8_t code = req->function;

    /* 检查异常响应 */
    if (code & 0x80) {
        return ERROR_EXCEPTION_RESPONSE;
    }

    /* 查找处理函数 */
    for (int i = 0; func_table[i].handler != NULL; i++) {
        if (func_table[i].code == code) {
            /* 检查数据长度 */
            if (req->data_len < func_table[i].min_data_len ||
                req->data_len > func_table[i].max_data_len) {
                return build_exception_response(resp, code, 0x03);
            }
            return func_table[i].handler(req, resp);
        }
    }

    /* 不支持的功能码 */
    return build_exception_response(resp, code, 0x01);
}
```

### 5.2 异常响应构建

```c
/* 异常响应构建 */
int build_exception_response(ModbusFrame *resp, uint8_t function, uint8_t exception_code) {
    resp->address = DEVICE_ADDRESS;
    resp->function = function | 0x80;  /* 设置MSB */
    resp->data[0] = exception_code;
    resp->data_len = 1;
    resp->crc = calculate_crc16(resp->data, resp->data_len + 2);
    return 0;
}

/* 异常码定义 */
typedef enum {
    EX_ILLEGAL_FUNCTION = 0x01,
    EX_ILLEGAL_DATA_ADDRESS = 0x02,
    EX_ILLEGAL_DATA_VALUE = 0x03,
    EX_SLAVE_DEVICE_FAILURE = 0x04,
    EX_ACKNOWLEDGE = 0x05,
    EX_SLAVE_DEVICE_BUSY = 0x06,
    EX_MEMORY_PARITY_ERROR = 0x08,
    EX_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    EX_GATEWAY_TARGET_FAILED = 0x0B
} ModbusExceptionCode;
```

---

## 6. 关联概念网络

```
功能码概念关联
│
├── 数据模型
│   ├── 线圈 (Coils) ↔ 0x01, 0x05, 0x0F
│   ├── 离散输入 (Discrete Inputs) ↔ 0x02
│   ├── 保持寄存器 (Holding Registers) ↔ 0x03, 0x06, 0x10, 0x17
│   └── 输入寄存器 (Input Registers) ↔ 0x04
│
├── 访问模式
│   ├── 读操作 ↔ 0x01-0x04
│   ├── 写操作 ↔ 0x05, 0x06, 0x0F, 0x10
│   └── 原子读写 ↔ 0x17
│
├── 批量优化
│   ├── 单操作 ↔ 0x05, 0x06 (简单但低效)
│   └── 批量操作 ↔ 0x0F, 0x10 (复杂但高效)
│
└── 异常处理
    ├── 地址错误 ↔ 0x02
    ├── 数据错误 ↔ 0x03
    └── 设备错误 ↔ 0x04, 0x06
```

---

## 7. 参考规范

- **Modbus Application Protocol V1.1b3**: 功能码详细定义
- **Modbus Messaging on TCP/IP V1.0b**: TCP模式功能码
- **PI-MBUS-300**: Modbus技术参考
