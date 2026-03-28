# I2C 接口深度解析

> **全称**: Inter-Integrated Circuit
> **标准**: NXP I2C-bus specification (UM10204)
> **物理层**: 开漏输出，上拉电阻
> **最后更新**: 2026-03-29

---

## 1. 协议形式化定义

### 1.1 概念本体

```
┌─────────────────────────────────────────────────────────────────┐
│                   I2C 协议概念结构                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  拓扑结构: 多主多从，总线型拓扑                                   │
│                                                                 │
│   ┌──────────────────────────────────────────────────────┐     │
│   │                    I2C Bus                           │     │
│   │  (SDA - 数据线) ────────────────────────────────     │     │
│   │  (SCL - 时钟线) ────────────────────────────────     │     │
│   │         │              │              │              │     │
│   │      ┌──┴──┐        ┌──┴──┐        ┌──┴──┐          │     │
│   │      │Master│        │Slave1│        │Slave2│          │     │
│   │      │(主控)│        │(设备)│        │(设备)│          │     │
│   │      └──┬──┘        └──┬──┘        └──┬──┘          │     │
│   │      Addr: N/A       Addr: 0x50     Addr: 0x3C       │     │
│   └──────────────────────────────────────────────────────┘     │
│                                                                 │
│  信号特性:                                                       │
│  ├── SDA: Serial Data (双向数据线)                               │
│  ├── SCL: Serial Clock (时钟线，主设备产生)                      │
│  ├── 逻辑0: 下拉 ( sinking current )                            │
│  └── 逻辑1: 上拉电阻释放 (总线高阻态)                            │
│                                                                 │
│  电气特性:                                                       │
│  ├── 标准模式 (Sm): 100kbps, 上拉电阻4.7kΩ                      │
│  ├── 快速模式 (Fm): 400kbps, 上拉电阻1.5-4.7kΩ                  │
│  ├── 快速模式+ (Fm+): 1Mbps, 上拉电阻1kΩ                        │
│  └── 高速模式 (Hs): 3.4Mbps, 需要电流源上拉                     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 形式化定义

**定义 1.1 (I2C传输单元)**

```
I2CTransfer ::= StartCondition × AddressPhase × DataPhase* × StopCondition

StartCondition  ::= SCL=1 ∧ SDA: 1→0 (下降沿)
StopCondition   ::= SCL=1 ∧ SDA: 0→1 (上升沿)

AddressPhase    ::= SlaveAddress(7bit) × R/W(1bit) × ACK(1bit)
SlaveAddress    ::= {0x00, ..., 0x7F}  (0x00为广播地址)
R/W             ::= 0 (Write) | 1 (Read)
ACK             ::= 0 (ACK) | 1 (NACK)

DataPhase       ::= DataByte(8bit) × ACK(1bit)
DataByte        ::= {0x00, ..., 0xFF}
```

**定义 1.2 (时钟同步与仲裁)**

```
时钟同步 (Clock Synchronization):
  在多主模式下，SCL采用"线与"逻辑
  SCL_actual = SCL_master1 ∧ SCL_master2 ∧ ... ∧ SCL_masterN

  结果: 时钟周期由最长的低电平周期决定

仲裁 (Arbitration):
  仲裁在SDA上进行，逐位比较
  若主设备发送1但总线为0，则该主设备仲裁失败，转为从模式

  仲裁安全地址: 全0地址 (0x00) 不参与仲裁
```

---

## 2. 时序分析

### 2.1 位传输时序

```
┌─────────────────────────────────────────────────────────────────┐
│                    I2C 位传输时序                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SCL     ─┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐      ┌───┐        │
│           └───┘   └───┘   └───┘   └───┘   └──────┘   │        │
│                │       │       │       │              │        │
│  SDA     ──────┤───────┼───────┼───────┼──────────────        │
│                │       │       │       │                        │
│                ▼       ▼       ▼       ▼                        │
│              Setup    Hold   Setup   Hold                       │
│              (tSU)    (tHD)  (tSU)   (tHD)                      │
│                                                                 │
│  标准模式时序参数:                                               │
│  ┌─────────────┬───────────┬──────────┐                        │
│  │ 参数        │ 最小值    │ 最大值   │                        │
│  ├─────────────┼───────────┼──────────┤                        │
│  │ fSCL        │ 0         │ 100 kHz  │                        │
│  │ tHD:STA     │ 4.0 μs    │ -        │ 起始条件保持           │
│  │ tLOW        │ 4.7 μs    │ -        │ SCL低电平时间          │
│  │ tHIGH       │ 4.0 μs    │ -        │ SCL高电平时间          │
│  │ tSU:STA     │ 4.7 μs    │ -        │ 起始条件建立           │
│  │ tSU:DAT     │ 250 ns    │ -        │ 数据建立               │
│  │ tHD:DAT     │ 0         │ 3.45 μs  │ 数据保持               │
│  │ tSU:STO     │ 4.0 μs    │ -        │ 停止条件建立           │
│  │ tBUF        │ 4.7 μs    │ -        │ 总线空闲               │
│  └─────────────┴───────────┴──────────┘                        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 完整传输时序

```
┌─────────────────────────────────────────────────────────────────────┐
│                  I2C 完整传输时序 (写操作)                           │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  SCL  ─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌┐ ┌─┐ │
│        └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └┘ └┘ └┘ └┘ └┘ └┘ └┘ └┘ └┘ └┘ │
│        │ S │A6│A5│A4│A3│A2│A1│A0│R/W│ACK│D7│D6│D5│D4│D3│D2│D1│D0│ACK│ P │
│        │ T │  │  │  │  │  │  │  │ W │   │  │  │  │  │  │  │  │  │   │   │
│        │ A │  │  │  │  │  │  │  │   │   │  │  │  │  │  │  │  │  │   │   │
│  SDA  ─┘   │  │  │  │  │  │  │  │   │   │  │  │  │  │  │  │  │  │   │   │
│         S  │  │  │  │  │  │  │  │ A │ D │  │  │  │  │  │  │  │  │ A │   │
│            │  │  │  │  │  │  │  │ C │ A │  │  │  │  │  │  │  │  │ C │ S │
│            │  │  │  │  │  │  │  │ K │ T │  │  │  │  │  │  │  │  │ K │ T │
│            │  │  │  │  │  │  │  │   │ A │  │  │  │  │  │  │  │  │   │ O │
│            │  │  │  │  │  │  │  │   │   │  │  │  │  │  │  │  │  │   │ P │
│                                                                     │
│  传输阶段:                                                          │
│  1. Start (S): 起始条件，主设备拉低SDA                              │
│  2. Address: 7位从地址 + 1位读写位(0=写)                            │
│  3. ACK: 从设备拉低SDA确认                                          │
│  4. Data: 8位数据字节                                               │
│  5. ACK: 从设备确认                                                 │
│  6. Stop (P): 停止条件，主设备释放SDA                               │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 3. C语言驱动状态机

### 3.1 主机发送状态机

```c
/* I2C主机状态机定义 */
typedef enum {
    I2C_STATE_IDLE = 0,         /* 空闲 */
    I2C_STATE_START,            /* 发送起始条件 */
    I2C_STATE_ADDR_TX,          /* 发送地址 */
    I2C_STATE_ADDR_ACK,         /* 等待地址ACK */
    I2C_STATE_DATA_TX,          /* 发送数据 */
    I2C_STATE_DATA_ACK,         /* 等待数据ACK */
    I2C_STATE_REP_START,        /* 重复起始 */
    I2C_STATE_STOP,             /* 发送停止 */
    I2C_STATE_ERROR             /* 错误状态 */
} I2C_MasterState;

/* 状态转换表 */
const I2C_MasterState state_transitions[][8] = {
    /* 事件:  START_SENT  ADDR_SENT  ACK_RECVD  NACK_RECVD  DATA_SENT  STOP_SENT  ERROR */
    /* IDLE */      {START,      IDLE,      IDLE,      IDLE,       IDLE,      IDLE,      ERROR},
    /* START */     {START,      ADDR_TX,   START,     START,      START,     IDLE,      ERROR},
    /* ADDR_TX */   {ADDR_TX,    ADDR_TX,   ADDR_ACK,  ERROR,      ADDR_TX,   IDLE,      ERROR},
    /* ADDR_ACK */  {ADDR_ACK,   DATA_TX,   DATA_TX,   STOP,       DATA_TX,   STOP,      ERROR},
    /* DATA_TX */   {DATA_TX,    DATA_TX,   DATA_ACK,  ERROR,      DATA_TX,   STOP,      ERROR},
    /* DATA_ACK */  {DATA_ACK,   DATA_TX,   DATA_TX,   STOP,       REP_START, STOP,      ERROR},
    /* REP_START */ {START,      ADDR_TX,   REP_START, REP_START,  REP_START, STOP,      ERROR},
    /* STOP */      {IDLE,       IDLE,      IDLE,      IDLE,       IDLE,      IDLE,      IDLE},
    /* ERROR */     {ERROR,      ERROR,     ERROR,     ERROR,      ERROR,     IDLE,      ERROR}
};
```

### 3.2 状态机流程图

```
I2C主机发送状态机
│
├── [IDLE] 空闲
│   └── 用户调用I2C_Transmit()
│       └── 发起START → [START]
│
├── [START] 起始条件已发送
│   ├── 成功 → 发送地址 → [ADDR_TX]
│   └── 失败 → [ERROR]
│
├── [ADDR_TX] 地址已发送
│   ├── 收到ACK → [ADDR_ACK]
│   └── 收到NACK → [STOP]
│
├── [ADDR_ACK] 地址确认
│   ├── 有数据 → 发送数据 → [DATA_TX]
│   └── 无数据 → 发送STOP → [STOP]
│
├── [DATA_TX] 数据已发送
│   ├── 收到ACK → [DATA_ACK]
│   └── 收到NACK → [STOP]
│
├── [DATA_ACK] 数据确认
│   ├── 更多数据 → 发送数据 → [DATA_TX]
│   ├── 需要读 → 重复START → [REP_START]
│   └── 完成 → 发送STOP → [STOP]
│
├── [REP_START] 重复起始
│   └── 发送新地址 → [ADDR_TX]
│
├── [STOP] 停止条件
│   └── 返回 → [IDLE]
│
└── [ERROR] 错误处理
    └── 发送STOP → [IDLE]
```

---

## 4. 设备地址空间

### 4.1 7位地址分配

```
┌─────────────────────────────────────────────────────────────────┐
│                 I2C 7位地址分配                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  地址范围        类型              说明                          │
│  ─────────────────────────────────────────────────────────────  │
│                                                                 │
│  0x00           广播地址           通用广播，所有设备响应         │
│  0x01-0x07      保留              CBUS地址等                    │
│  0x08-0x77      设备地址           标准设备地址范围               │
│  │                                                              │
│  │  常用设备地址:                                                │
│  │  ├── 0x3C, 0x3D   SSD1306 OLED                               │
│  │  ├── 0x48-0x4B   ADS1115 ADC                                 │
│  │  ├── 0x50-0x57   EEPROM (24Cxx)                              │
│  │  ├── 0x68, 0x69   MPU6050 IMU                                │
│  │  ├── 0x76, 0x77   BME280 传感器                              │
│  │  └── 0x40-0x4F   PCA9685 PWM                                 │
│  │                                                              │
│  0x78-0x7B      10位地址前缀      10位地址设备低4位             │
│  0x7C-0x7F      保留              未来扩展                      │
│                                                                 │
│  地址冲突解决:                                                   │
│  1. 硬件地址选择引脚 (A0, A1, A2)                               │
│  2. 软件配置 (部分设备支持)                                      │
│  3. 使用I2C多路复用器 (PCA9548A等)                              │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.2 地址扫描算法

```c
/* I2C设备地址扫描 */
void I2C_Scan(I2C_Handle *hi2c, uint8_t *found_devices, uint8_t *count) {
    *count = 0;
    uint8_t dummy_data = 0;

    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        /* 尝试发送设备地址 */
        if (I2C_Master_Transmit(hi2c, addr << 1, &dummy_data, 0, 100) == SUCCESS) {
            /* 收到ACK，设备存在 */
            found_devices[(*count)++] = addr;
        }

        /* 发送STOP条件 */
        I2C_GenerateStop(hi2c);

        /* 延时等待总线稳定 */
        delay_ms(1);
    }
}

/* 打印扫描结果 */
void I2C_PrintScanResults(uint8_t *devices, uint8_t count) {
    printf("I2C设备扫描结果: 发现%d个设备\n", count);
    for (uint8_t i = 0; i < count; i++) {
        printf("  地址: 0x%02X (7-bit) / 0x%02X (8-bit)\n",
               devices[i], devices[i] << 1);
    }
}
```

---

## 5. 高级特性

### 5.1 时钟拉伸 (Clock Stretching)

```
┌─────────────────────────────────────────────────────────────────┐
│                    时钟拉伸机制                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  场景: 从设备处理速度慢，需要更多时间                            │
│                                                                 │
│  SCL  ─┐ ┌───┐   ┌───────┐   ┌───┐   ┌───┐   ┌───┐            │
│        └─┘   └───┘       └───┘   └───┘   └───┘   └────        │
│                 ▲       ▲                                      │
│                 │       │                                      │
│                 │       └─ 从设备拉低SCL，请求等待              │
│                 └─ 主设备释放SCL高电平                          │
│                                                                 │
│  实现:                                                          │
│  1. 从设备在SCL低电平期间拉低SCL                                │
│  2. 主设备检测到SCL为低，等待                                   │
│  3. 从设备处理完成，释放SCL                                     │
│  4. 主设备继续产生时钟                                          │
│                                                                 │
│  C语言处理:                                                     │
│  - 硬件I2C: 自动处理                                            │
│  - 软件I2C (bit-bang): 需要检测SCL状态                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 多主仲裁

```c
/* 多主模式仲裁检测 */
typedef struct {
    uint8_t is_master;
    uint8_t arbitration_lost;
    uint8_t own_address;
} I2C_MultiMaster;

/* 仲裁丢失检测 */
void I2C_CheckArbitration(I2C_MultiMaster *mm, uint8_t sent_bit, uint8_t bus_bit) {
    if (mm->is_master && sent_bit != bus_bit) {
        /* 仲裁丢失 */
        mm->arbitration_lost = 1;
        mm->is_master = 0;

        /* 转为从模式，等待被寻址 */
        I2C_SetSlaveMode(mm->own_address);
    }
}

/* 仲裁恢复策略 */
void I2C_ArbitrationRecovery(I2C_MultiMaster *mm) {
    if (mm->arbitration_lost) {
        /* 等待总线空闲 */
        while (I2C_BusBusy()) {
            /* 延时或任务切换 */
        }

        /* 尝试重新成为主设备 */
        mm->arbitration_lost = 0;
        mm->is_master = 1;
    }
}
```

---

## 6. 常见设备驱动框架

```c
/* I2C设备抽象层 */
typedef struct {
    I2C_Handle *i2c;
    uint8_t     addr;
    uint8_t     initialized;
} I2C_Device;

/* 设备操作函数指针 */
typedef struct {
    int (*init)(I2C_Device *dev);
    int (*read)(I2C_Device *dev, uint8_t reg, uint8_t *data, uint16_t len);
    int (*write)(I2C_Device *dev, uint8_t reg, uint8_t *data, uint16_t len);
    int (*deinit)(I2C_Device *dev);
} I2C_DeviceOps;

/* EEPROM驱动示例 */
int EEPROM_WritePage(I2C_Device *dev, uint16_t addr, uint8_t *data, uint8_t len) {
    uint8_t buf[34];  /* 2字节地址 + 32字节数据 */

    /* 检查参数 */
    if (len > 32 || addr + len > 0x8000) {
        return ERROR_INVALID_PARAM;
    }

    /* 构造写入缓冲 */
    buf[0] = (addr >> 8) & 0xFF;  /* 高地址字节 */
    buf[1] = addr & 0xFF;          /* 低地址字节 */
    memcpy(&buf[2], data, len);

    /* 发送数据 */
    if (I2C_Master_Transmit(dev->i2c, dev->addr, buf, len + 2, 100) != SUCCESS) {
        return ERROR_I2C_TIMEOUT;
    }

    /* 等待写入完成 (轮询或延时) */
    delay_ms(5);

    return SUCCESS;
}
```

---

## 7. 故障排查决策树

```
I2C通信故障排查
│
├── 无法检测到设备
│   ├── 检查物理连接
│   │   ├── SDA/SCL是否接反?
│   │   ├── 上拉电阻是否存在? (4.7kΩ)
│   │   ├── 地线是否连通?
│   │   └── 电源电压是否正确? (3.3V/5V)
│   │
│   ├── 检查地址
│   │   ├── 使用7位还是8位地址? (软件通常用7位)
│   │   ├── 地址引脚(A0-A2)配置?
│   │   └── 是否有地址冲突?
│   │
│   └── 检查时序
│       ├── 波特率是否匹配?
│       └── 时钟拉伸超时?
│
├── 间歇性通信失败
│   ├── 信号完整性
│   │   ├── 总线电容是否过大? (<400pF)
│   │   ├── 线缆长度? (<1米@100kHz)
│   │   └── 是否有干扰源?
│   │
│   └── 软件问题
│       ├── 中断处理是否及时?
│       └── 是否有竞态条件?
│
└── 数据错误
    ├── 寄存器地址错误
    ├── 数据长度不匹配
    └── 设备未正确初始化
```

---

## 8. 参考规范

- **UM10204**: I2C-bus specification and user manual (NXP)
- **I2C Standard**: I2C Bus Specification, Version 6.0, April 2014
- **SMBus**: System Management Bus Specification, Version 3.1
- **PMBus**: Power Management Bus Specification, Part I & II
