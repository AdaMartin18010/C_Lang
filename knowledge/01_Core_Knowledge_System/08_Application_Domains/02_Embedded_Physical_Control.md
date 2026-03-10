# C语言物理嵌入式控制：工控与电子电器处理技巧全解

> **层级定位**: 01 Core Knowledge System / 08 Application Domains
> **主题**: 物理嵌入式、工控系统、电子电器控制
> **难度级别**: L4 分析 → L5 专家
> **预估学习时间**: 20-30 小时

---

## 📋 目录

- [C语言物理嵌入式控制：工控与电子电器处理技巧全解](#c语言物理嵌入式控制工控与电子电器处理技巧全解)
  - [📋 目录](#-目录)
  - [一、硬件抽象层语义](#一硬件抽象层语义)
    - [1.1 内存映射I/O的本质](#11-内存映射io的本质)
    - [1.2 volatile的物理语义](#12-volatile的物理语义)
    - [1.3 寄存器位域的语义抽象](#13-寄存器位域的语义抽象)
  - [二、寄存器操作技巧](#二寄存器操作技巧)
    - [2.1 读-改-写模式（RMW）](#21-读-改-写模式rmw)
    - [2.2 影子寄存器与双缓冲](#22-影子寄存器与双缓冲)
    - [2.3 寄存器组遍历技巧](#23-寄存器组遍历技巧)
  - [三、位运算与掩码艺术](#三位运算与掩码艺术)
    - [3.1 位操作语义表](#31-位操作语义表)
    - [3.2 状态标志处理模式](#32-状态标志处理模式)
    - [3.3 位图管理 (Bitmap)](#33-位图管理-bitmap)
  - [四、物理量处理语义](#四物理量处理语义)
    - [4.1 定点数运算（无FPU场景）](#41-定点数运算无fpu场景)
    - [4.2 物理单位与量纲](#42-物理单位与量纲)
  - [五、中断与实时响应](#五中断与实时响应)
    - [5.1 中断处理语义](#51-中断处理语义)
    - [5.2 临界区保护](#52-临界区保护)
  - [六、通信协议实现](#六通信协议实现)
    - [6.1 Modbus RTU协议栈](#61-modbus-rtu协议栈)
    - [6.2 CAN总线通信](#62-can总线通信)
  - [七、电机与运动控制](#七电机与运动控制)
    - [7.1 PWM电机控制](#71-pwm电机控制)
    - [7.2 步进电机控制](#72-步进电机控制)
  - [八、传感器数据处理](#八传感器数据处理)
    - [8.1 ADC采样与滤波](#81-adc采样与滤波)
    - [8.2 温度补偿算法](#82-温度补偿算法)
  - [九、电源管理与低功耗](#九电源管理与低功耗)
    - [9.1 睡眠模式管理](#91-睡眠模式管理)
    - [9.2 外设电源门控](#92-外设电源门控)
  - [十、可靠性设计技巧](#十可靠性设计技巧)
    - [10.1 看门狗机制](#101-看门狗机制)
    - [10.2 断言与故障处理](#102-断言与故障处理)
  - [附录: 完整示例 - 温度控制器](#附录-完整示例---温度控制器)

---

## 一、硬件抽象层语义

### 1.1 内存映射I/O的本质

```c
// ============================================================
// 物理视角: 寄存器是硬件的"软件接口"
// ============================================================

// 物理 reality: 0x40020000 地址对应GPIOA控制器的硬件寄存器
// 软件 abstraction: 通过C语言类型系统赋予语义

// 反模式: 直接硬编码（丧失语义）
*(volatile uint32_t *)0x40020000 = 0x01;  // 什么意思？谁知道！

// 正模式: 语义化抽象
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
GPIOA_MODER = GPIO_MODER_MODE0_OUTPUT;  // 清晰: 设置PA0为输出模式

// 进阶模式: 结构体映射（类型安全 + 偏移自动计算）
typedef struct {
    volatile uint32_t MODER;    // 0x00: 模式配置 (00=输入, 01=输出, 10=复用, 11=模拟)
    volatile uint32_t OTYPER;   // 0x04: 输出类型 (0=推挽, 1=开漏)
    volatile uint32_t OSPEEDR;  // 0x08: 输出速度 (00=2MHz, 01=25MHz, 10=50MHz, 11=100MHz)
    volatile uint32_t PUPDR;    // 0x0C: 上下拉   (00=浮空, 01=上拉, 10=下拉, 11=保留)
    volatile uint32_t IDR;      // 0x10: 输入数据 (只读)
    volatile uint32_t ODR;      // 0x14: 输出数据 (读写)
    volatile uint32_t BSRR;     // 0x18: 位设置/清除寄存器 (写1有效, 原子操作)
    volatile uint32_t LCKR;     // 0x1C: 锁定寄存器
    volatile uint32_t AFRL;     // 0x20: 复用功能低 [7:0]
    volatile uint32_t AFRH;     // 0x24: 复用功能高 [15:8]
} GPIO_TypeDef;

// 语义: GPIOA是一个GPIO_TypeDef类型的对象，位于0x40020000
#define GPIOA_BASE  0x40020000
#define GPIOA       ((GPIO_TypeDef *)GPIOA_BASE)

// 使用: 语法即语义
void gpio_init_output(GPIO_TypeDef *gpio, uint32_t pin) {
    // 清除模式位 (先清后设，避免影响其他位)
    gpio->MODER &= ~(3U << (pin * 2));           // 11b << pos → 清除两位
    gpio->MODER |= (1U << (pin * 2));            // 01b = 输出模式

    // 配置输出类型: 推挽 (0)
    gpio->OTYPER &= ~(1U << pin);

    // 配置速度: 50MHz (10b)
    gpio->OSPEEDR &= ~(3U << (pin * 2));
    gpio->OSPEEDR |= (2U << (pin * 2));
}
```

### 1.2 volatile的物理语义

```c
// ============================================================
// volatile = "编译器，不要优化我，我有副作用"
// ============================================================

// 场景1: 硬件寄存器 (读取有副作用)
// 物理现实: 读取状态寄存器可能清除标志位
volatile uint32_t *STATUS_REG = (uint32_t *)0x40001000;

// 错误: 编译器优化后只读一次
uint32_t status = *STATUS_REG;
if (status & FLAG_A) { /* ... */ }
if (status & FLAG_B) { /* ... */ }  // 可能错过B标志的变化

// 正确: 每次读取都是新的硬件状态
if (*STATUS_REG & FLAG_A) { /* ... */ }
if (*STATUS_REG & FLAG_B) { /* ... */ }

// 场景2: 内存映射I/O (写入必须发生)
// 物理现实: 写入GPIO就是改变引脚电平
volatile uint32_t *GPIO_OUT = (uint32_t *)0x40020014;

// 错误: 编译器认为无意义，优化掉
*GPIO_OUT = 1;
*GPIO_OUT = 0;  // 可能被优化，只保留最后一条

// 正确: 两次写入都会发生
volatile uint32_t * const GPIO_OUT = (uint32_t *)0x40020014;
*GPIO_OUT = 1;  // 引脚变高
*GPIO_OUT = 0;  // 引脚变低 (产生脉冲)

// 场景3: 多核/中断共享内存
volatile int data_ready = 0;
volatile int shared_data = 0;

// ISR (中断服务程序)
void ISR(void) {
    shared_data = sensor_read();  // 写入共享数据
    data_ready = 1;               // 标记就绪
}

// 主循环
while (!data_ready) { /* 等待 */ }  // 必须volatile，否则可能永远循环
int data = shared_data;              // 读取共享数据
```

### 1.3 寄存器位域的语义抽象

```c
// ============================================================
// 位域: 赋予每一位语义含义
// ============================================================

// 传统掩码方式 (容易出错)
#define ADC_CR1_ADON    (1U << 0)   // 开启ADC
#define ADC_CR1_CONT    (1U << 1)   // 连续转换
#define ADC_CR1_CAL     (1U << 2)   // 校准
#define ADC_CR1_RES_12B (0U << 24)  // 12位分辨率
#define ADC_CR1_RES_10B (1U << 24)  // 10位分辨率
#define ADC_CR1_RES_8B  (2U << 24)  // 8位分辨率

ADC->CR1 = ADC_CR1_ADON | ADC_CR1_CONT | ADC_CR1_RES_12B;  // 一堆或操作

// 位域结构方式 (C99, 可移植性差，不适合跨平台)
typedef struct {
    uint32_t ADON     : 1;   // bit 0: 开启
    uint32_t CONT     : 1;   // bit 1: 连续模式
    uint32_t CAL      : 1;   // bit 2: 校准
    uint32_t          : 21;  // bits 3-23: 保留
    uint32_t RES      : 2;   // bits 24-25: 分辨率 (0=12b, 1=10b, 2=8b, 3=6b)
    uint32_t          : 6;   // bits 26-31: 保留
} ADC_CR1_Bits;

// Union方式访问 (兼顾语义和效率)
typedef union {
    uint32_t reg;           // 整体访问
    ADC_CR1_Bits bits;      // 位域访问
} ADC_CR1_Type;

// 使用
ADC_CR1_Type cr1;
cr1.reg = 0;
cr1.bits.ADON = 1;      // 开启ADC
cr1.bits.CONT = 1;      // 连续模式
cr1.bits.RES = 0;       // 12位分辨率
ADC->CR1 = cr1.reg;     // 一次性写入

// 宏封装方式 (推荐，兼顾可读性和可移植性)
#define ADC_SET_RESOLUTION(adc, res) do { \
    uint32_t tmp = (adc)->CR1; \
    tmp &= ~(3U << 24); \
    tmp |= ((res) & 3U) << 24; \
    (adc)->CR1 = tmp; \
} while(0)

#define ADC_ENABLE(adc)     ((adc)->CR1 |= (1U << 0))
#define ADC_DISABLE(adc)    ((adc)->CR1 &= ~(1U << 0))
#define ADC_IS_ENABLED(adc) ((adc)->CR1 & (1U << 0))
```

---

## 二、寄存器操作技巧

### 2.1 读-改-写模式（RMW）

```c
// ============================================================
// 读-改-写: 嵌入式寄存器操作的基本模式
// 物理原因: 寄存器是32位整体，不能只改某几位
// ============================================================

// 标准RMW模式 (非原子，可能被中断打断)
void set_bit_rmw(volatile uint32_t *reg, uint32_t bit) {
    uint32_t tmp = *reg;    // 1. 读 (Read)
    tmp |= bit;             // 2. 改 (Modify)
    *reg = tmp;             // 3. 写 (Write)
}

// 问题: 中断可能在这里发生，修改同一寄存器，导致丢失修改
// 时间线: 主程序读 -> 中断修改寄存器 -> 主程序写 (覆盖中断的修改)

// 解决方案1: 临界区保护
void set_bit_safe(volatile uint32_t *reg, uint32_t bit) {
    disable_interrupts();   // 关中断
    uint32_t tmp = *reg;
    tmp |= bit;
    *reg = tmp;
    enable_interrupts();    // 开中断
}

// 解决方案2: 硬件位带（Cortex-M特有）
// 位带: 每个bit映射到单独的32位地址，写操作天然原子
#define BITBAND_ADDR(addr, bitnum) (((addr) & 0xF0000000) + 0x2000000 + \
                                    (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2))
#define BITBAND_PTR(addr, bitnum) ((volatile uint32_t *)BITBAND_ADDR(addr, bitnum))

// 原子设置PA0 (位带)
*BITBAND_PTR(GPIOA_BASE + 0x14, 0) = 1;

// 解决方案3: 硬件RMW寄存器（如GPIO的BSRR）
// BSRR: 低16位写1置位，高16位写1清零，各自独立，原子操作
#define GPIO_SET_PIN(gpio, pin)   ((gpio)->BSRR = (1U << (pin)))       // 设置
#define GPIO_RESET_PIN(gpio, pin) ((gpio)->BSRR = (1U << ((pin) + 16))) // 清零
```

### 2.2 影子寄存器与双缓冲

```c
// ============================================================
// 影子寄存器: 硬件自动同步，避免中间状态
// ============================================================

// PWM周期配置示例 (STM32)
// TIM_ARR: 自动重装载寄存器 (有影子寄存器)
// 物理意义: 改变周期值时，当前周期不会被打断，新值在下周期生效

typedef struct {
    volatile uint32_t CR1;   // 控制寄存器1
    volatile uint32_t CR2;   // 控制寄存器2
    volatile uint32_t SMCR;  // 从模式控制
    volatile uint32_t DIER;  // DMA/中断使能
    volatile uint32_t SR;    // 状态寄存器
    volatile uint32_t EGR;   // 事件生成
    volatile uint32_t CCMR1; // 捕获/比较模式1
    volatile uint32_t CCMR2; // 捕获/比较模式2
    volatile uint32_t CCER;  // 捕获/比较使能
    volatile uint32_t CNT;   // 计数器 (实时值)
    volatile uint32_t PSC;   // 预分频器 (有影子)
    volatile uint32_t ARR;   // 自动重装载 (有影子)
    // ...
} TIM_TypeDef;

// 配置PWM频率 (安全，不会导致毛刺)
void pwm_set_frequency(TIM_TypeDef *tim, uint32_t freq_hz) {
    uint32_t period = SYSTEM_CLOCK / freq_hz - 1;

    // 写入ARR，但新值不会立即生效
    // 硬件自动: 当前计数周期完成后，才加载影子寄存器
    tim->ARR = period;

    // 生成更新事件，立即加载影子寄存器 (如果需要)
    tim->EGR = (1U << 0);  // UG位
}

// 软件层面的双缓冲 (类似概念)
typedef struct {
    uint32_t active;    // 当前生效值
    uint32_t shadow;    // 待生效值
    volatile uint8_t update_pending;  // 更新标志
} DoubleBuffer;

void double_buffer_update(DoubleBuffer *db, uint32_t new_value) {
    db->shadow = new_value;           // 写入影子
    db->update_pending = 1;           // 标记待更新
}

// 在合适的时机 (如VSYNC中断) 切换
void double_buffer_commit(DoubleBuffer *db) {
    if (db->update_pending) {
        db->active = db->shadow;      // 影子->生效
        db->update_pending = 0;
    }
}
```

### 2.3 寄存器组遍历技巧

```c
// ============================================================
// 寄存器数组: 批量操作多个相似外设
// ============================================================

// 方法1: 基址数组 (多个GPIO端口)
GPIO_TypeDef * const GPIO_PORTS[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
#define NUM_GPIO_PORTS 5

void gpio_init_all_input(void) {
    for (int i = 0; i < NUM_GPIO_PORTS; i++) {
        GPIO_PORTS[i]->MODER = 0;  // 全部设为输入 (00)
    }
}

// 方法2: 基址 + 偏移计算 (更紧凑)
#define GPIO_BASE_ADDR  0x40020000
#define GPIO_PORT_SIZE  0x400       // 每个GPIO端口占用1KB
#define NUM_PORTS       8

static inline GPIO_TypeDef *gpio_port(int port) {
    return (GPIO_TypeDef *)(GPIO_BASE_ADDR + port * GPIO_PORT_SIZE);
}

void gpio_set_all(uint32_t pin, int value) {
    for (int port = 0; port < NUM_PORTS; port++) {
        if (value) {
            gpio_port(port)->BSRR = (1U << pin);
        } else {
            gpio_port(port)->BSRR = (1U << (pin + 16));
        }
    }
}

// 方法3: 位带批量操作 (原子设置多个端口同一位)
void gpio_set_all_pin0_atomic(int value) {
    // 所有GPIO端口的ODR bit0的位带地址
    volatile uint32_t * const pin0_bits[] = {
        BITBAND_PTR(GPIOA_BASE + 0x14, 0),
        BITBAND_PTR(GPIOB_BASE + 0x14, 0),
        // ...
    };

    for (int i = 0; i < NUM_PORTS; i++) {
        *pin0_bits[i] = value;  // 每条都是原子操作
    }
}
```

---

## 三、位运算与掩码艺术

### 3.1 位操作语义表

```c
// ============================================================
// 位操作 = 对硬件寄存器的精确控制
// ============================================================

// 基本操作语义
#define BIT(n)          (1U << (n))                 // 第n位为1
#define BITS(m, n)      (((1U << ((m)-(n)+1)) - 1) << (n))  // m到n位为1
#define SET_BIT(x, b)   ((x) |= BIT(b))              // 置位
#define CLEAR_BIT(x, b) ((x) &= ~BIT(b))             // 清零
#define TOGGLE_BIT(x,b) ((x) ^= BIT(b))              // 翻转
#define READ_BIT(x, b)  (((x) >> (b)) & 1U)          // 读取

// 字段操作 (多bit配置)
#define FIELD_MASK(pos, width)  (((1U << (width)) - 1) << (pos))
#define SET_FIELD(reg, pos, width, val) do { \
    (reg) &= ~FIELD_MASK(pos, width); \
    (reg) |= ((val) << (pos)) & FIELD_MASK(pos, width); \
} while(0)
#define GET_FIELD(reg, pos, width) (((reg) >> (pos)) & ((1U << (width)) - 1))

// 使用示例: 配置ADC分辨率 (bits 24-25, 2位宽)
// 00=12bit, 01=10bit, 10=8bit, 11=6bit
SET_FIELD(ADC1->CR1, 24, 2, 2);  // 设置为8bit分辨率
uint32_t res = GET_FIELD(ADC1->CR1, 24, 2);  // 读取当前分辨率
```

### 3.2 状态标志处理模式

```c
// ============================================================
// 状态寄存器处理: 检查-清除模式
// ============================================================

// 轮询方式 (简单但阻塞)
void uart_send_poll(uint8_t data) {
    // 等待发送寄存器空 (TXE = Transmit Empty)
    while (!(UART->SR & UART_SR_TXE)) {
        // 忙等待，CPU空转
    }
    UART->DR = data;  // 写入数据
}

// 中断方式 (非阻塞，推荐)
// 发送缓冲区
#define TX_BUF_SIZE 256
static uint8_t tx_buffer[TX_BUF_SIZE];
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;

// 启动发送
int uart_send_async(uint8_t data) {
    uint16_t next_head = (tx_head + 1) % TX_BUF_SIZE;
    if (next_head == tx_tail) {
        return -1;  // 缓冲区满
    }

    tx_buffer[tx_head] = data;
    tx_head = next_head;

    // 使能发送中断 (如果当前未使能)
    UART->CR1 |= UART_CR1_TXEIE;
    return 0;
}

// 中断服务程序
void UART_IRQHandler(void) {
    uint32_t sr = UART->SR;  // 必须先读SR

    // 检查并处理发送空中断
    if (sr & UART_SR_TXE) {
        if (tx_head != tx_tail) {
            UART->DR = tx_buffer[tx_tail];  // 发送下一个字节
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            // 缓冲区空，关闭发送中断
            UART->CR1 &= ~UART_CR1_TXEIE;
        }
    }

    // 检查并处理接收中断
    if (sr & UART_SR_RXNE) {
        uint8_t data = UART->DR;  // 读DR自动清除RXNE
        // 处理接收数据...
    }

    // 错误处理
    if (sr & (UART_SR_ORE | UART_SR_NE | UART_SR_FE)) {
        // 清除错误标志: 先读SR，再读DR
        (void)UART->DR;
    }
}
```

### 3.3 位图管理 (Bitmap)

```c
// ============================================================
// 位图: 紧凑的状态/标志管理
// ============================================================

// 32个资源的占用状态 (1位表示一个资源)
static uint32_t resource_bitmap = 0;

// 分配一个资源 (返回0-31的索引，-1表示无可用)
int resource_alloc(void) {
    // __builtin_ffs: GCC内建函数，返回最低位1的位置 (1-32)，0表示无
    int bit = __builtin_ffs(~resource_bitmap) - 1;  // 找第一个0位
    if (bit < 0 || bit >= 32) return -1;

    resource_bitmap |= (1U << bit);
    return bit;
}

// 释放资源
void resource_free(int id) {
    if (id >= 0 && id < 32) {
        resource_bitmap &= ~(1U << id);
    }
}

// 检查资源是否占用
int resource_is_busy(int id) {
    return (resource_bitmap >> id) & 1U;
}

// 位图遍历所有占用资源
void resource_foreach_busy(void (*callback)(int id)) {
    uint32_t bm = resource_bitmap;
    while (bm) {
        int bit = __builtin_ffs(bm) - 1;  // 找最低位的1
        callback(bit);
        bm &= ~(1U << bit);  // 清除该位，继续遍历
    }
}
```

---

## 四、物理量处理语义

### 4.1 定点数运算（无FPU场景）

```c
// ============================================================
// 定点数: 用整数模拟小数，避免浮点运算
// 格式: Qm.n = m位整数 + n位小数，共m+n+1位（含符号）
// ============================================================

// Q15格式: 1位符号 + 15位小数，范围[-1, 1-2^-15]
typedef int16_t q15_t;
#define Q15_SHIFT   15
#define Q15_ONE     (1 << Q15_SHIFT)  // 1.0 = 32768

// Q31格式: 1位符号 + 31位小数
typedef int32_t q31_t;
#define Q31_SHIFT   31
#define Q31_ONE     (1LL << Q31_SHIFT)

// 浮点转定点
static inline q15_t float_to_q15(float f) {
    return (q15_t)(f * Q15_ONE);
}

// 定点转浮点
static inline float q15_to_float(q15_t q) {
    return (float)q / Q15_ONE;
}

// 定点加法 (直接相加，注意溢出)
static inline q15_t q15_add(q15_t a, q15_t b) {
    int32_t sum = (int32_t)a + (int32_t)b;  // 提升到32位防止溢出
    // 饱和处理
    if (sum > INT16_MAX) return INT16_MAX;
    if (sum < INT16_MIN) return INT16_MIN;
    return (q15_t)sum;
}

// 定点乘法 (需要移位)
// (a * b) >> n，注意中间结果用32位
static inline q15_t q15_mul(q15_t a, q15_t b) {
    int32_t prod = (int32_t)a * (int32_t)b;  // 32位中间结果
    return (q15_t)((prod + (1 << (Q15_SHIFT - 1))) >> Q15_SHIFT);  // 四舍五入
}

// 定点除法
static inline q15_t q15_div(q15_t a, q15_t b) {
    int32_t num = (int32_t)a << Q15_SHIFT;   // 被除数左移
    return (q15_t)(num / b);
}

// 应用: PID控制器 (纯定点实现)
typedef struct {
    q15_t kp, ki, kd;       // Q15格式参数
    q15_t integral;          // 积分项
    q15_t prev_error;        // 上次的误差
    q15_t output_min, output_max;  // 输出限制
} PID_Fixed;

q15_t pid_compute_fixed(PID_Fixed *pid, q15_t setpoint, q15_t measurement) {
    q15_t error = q15_sub(setpoint, measurement);

    // 积分项
    pid->integral = q15_add(pid->integral, error);
    // 积分限幅...

    // 微分项
    q15_t derivative = q15_sub(error, pid->prev_error);
    pid->prev_error = error;

    // 输出 = kp*error + ki*integral + kd*derivative
    q15_t output = q15_mul(pid->kp, error);
    output = q15_add(output, q15_mul(pid->ki, pid->integral));
    output = q15_add(output, q15_mul(pid->kd, derivative));

    // 输出限幅...
    return output;
}
```

### 4.2 物理单位与量纲

```c
// ============================================================
// 物理量类型安全: 防止单位混淆
// 技巧: 用结构体包装，编译期检查
// ============================================================

typedef struct { int32_t raw; } Millivolt;      // 毫伏
typedef struct { int32_t raw; } Milliampere;    // 毫安
typedef struct { int32_t raw; } Millicelsius;   // 毫摄氏度
typedef struct { int32_t raw; } Millibar;       // 毫巴（压力）

#define MV(val)  ((Millivolt){(val)})
#define MA(val)  ((Milliampere){(val)})
#define MC(val)  ((Millicelsius){(val)})

// 类型安全运算
static inline Millivolt mv_add(Millivolt a, Millivolt b) {
    return MV(a.raw + b.raw);
}

// 转换函数 (显式)
Millivolt adc_to_voltage(uint16_t adc_raw, uint16_t vref_mv) {
    // ADC 12bit, Vref = vref_mv mV
    // Vout = adc_raw * vref_mv / 4095
    return MV((int32_t)adc_raw * vref_mv / 4095);
}

// 错误: 编译失败！类型不匹配
// Millivolt v = MA(100);  // 错误: 不能混用

// 正确: 显式转换
Milliampere ma = MA(100);
// Millivolt v = (Millivolt){ma.raw};  // 可以，但显式
```

---

## 五、中断与实时响应

### 5.1 中断处理语义

```c
// ============================================================
// 中断: 硬件事件的软件响应
// 关键语义: 异步、快速、最小化
// ============================================================

// 中断向量表定义 (Cortex-M)
typedef void (*ISR_Handler)(void);

__attribute__((section(".isr_vector")))
const ISR_Handler vector_table[] = {
    (ISR_Handler)&_estack,      // 栈顶指针
    Reset_Handler,              // 复位
    NMI_Handler,                // 非屏蔽中断
    HardFault_Handler,          // 硬错误
    // ...
    TIM2_IRQHandler,            // 定时器2
    TIM3_IRQHandler,            // 定时器3
    // ...
};

// 中断处理最佳实践
// 原则1: ISR必须短小精悍
// 原则2: 不能在ISR中阻塞
// 原则3: 与主程序共享数据必须volatile + 同步

// 典型ISR结构: "标记-通知"模式
static volatile uint32_t tim2_tick = 0;
static volatile uint8_t tim2_flag = 0;

void TIM2_IRQHandler(void) {
    // 1. 清除中断标志 (必须先做，防止重入)
    TIM2->SR &= ~TIM_SR_UIF;

    // 2. 最小工作: 更新时间戳，设置标志
    tim2_tick++;
    tim2_flag = 1;

    // 3. 唤醒任务 (如果使用RTOS)
    // osSignalSet(task_id, SIG_TIM2);
}

// 主循环处理
void main_loop(void) {
    if (tim2_flag) {
        tim2_flag = 0;  // 清除标志

        // 执行耗时操作
        process_periodic_task();
    }
}
```

### 5.2 临界区保护

```c
// ============================================================
// 临界区: 保护共享资源的不可分割操作
// ============================================================

// 方法1: 全局中断开关 (最粗粒度)
#define CRITICAL_SECTION_BEGIN()  __disable_irq()
#define CRITICAL_SECTION_END()    __enable_irq()

// 问题: 如果嵌套，外层的enable会提前开启中断

// 改进: 嵌套安全版本
static volatile uint32_t critical_nesting = 0;

void enter_critical(void) {
    __disable_irq();
    critical_nesting++;
}

void exit_critical(void) {
    if (--critical_nesting == 0) {
        __enable_irq();
    }
}

// 使用
void update_shared_data(void) {
    enter_critical();
    // ... 访问共享资源
    exit_critical();
}

// 方法2: 基 PriMask (Cortex-M特有，更快)
// 只屏蔽 configurable 优先级的中断，NMI和HardFault仍然响应

static inline uint32_t primask_get(void) {
    uint32_t primask;
    __asm volatile ("mrs %0, primask" : "=r" (primask));
    return primask;
}

static inline void primask_set(uint32_t pri) {
    __asm volatile ("msr primask, %0" : : "r" (pri));
}

#define CRITICAL_SECTION_PM() \
    for (uint32_t _pm_save = primask_get(), ##__COUNTER__ = 0; \
         __COUNTER__ == 0; \
         primask_set(_pm_save), ##__COUNTER__ = 1) \
        for (primask_set(1); __COUNTER__ == 0; __COUNTER__ = 1)

// 使用
void foo(void) {
    CRITICAL_SECTION_PM() {
        // 临界区代码
        shared_var++;
    }
}

// 方法3: 使用LDREX/STREX指令 (原子操作，无需关中断)
// Cortex-M3/M4支持

// 原子加
uint32_t atomic_add(volatile uint32_t *ptr, uint32_t val) {
    uint32_t result;
    uint32_t tmp;

    __asm volatile (
        "1: ldrex %0, [%2]\n"
        "   add   %0, %0, %3\n"
        "   strex %1, %0, [%2]\n"
        "   teq   %1, #0\n"
        "   bne   1b"
        : "=&r" (result), "=&r" (tmp)
        : "r" (ptr), "r" (val)
        : "cc", "memory"
    );
    return result;
}
```

---

## 六、通信协议实现

### 6.1 Modbus RTU协议栈

```c
// ============================================================
// Modbus RTU: 工业控制常用协议
// 帧格式: [地址][功能码][数据][CRC16]
// ============================================================

#define MODBUS_SLAVE_ADDR   0x01
#define MODBUS_FUNC_READ_HOLDING  0x03
#define MODBUS_FUNC_WRITE_SINGLE  0x06

// CRC16计算 (Modbus标准多项式: 0xA001)
uint16_t modbus_crc16(const uint8_t *data, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 构造读保持寄存器请求
int modbus_build_read_holding(uint8_t *frame, uint16_t addr, uint16_t count) {
    frame[0] = MODBUS_SLAVE_ADDR;
    frame[1] = MODBUS_FUNC_READ_HOLDING;
    frame[2] = (addr >> 8) & 0xFF;      // 起始地址高字节
    frame[3] = addr & 0xFF;              // 起始地址低字节
    frame[4] = (count >> 8) & 0xFF;      // 寄存器数量高字节
    frame[5] = count & 0xFF;             // 寄存器数量低字节

    uint16_t crc = modbus_crc16(frame, 6);
    frame[6] = crc & 0xFF;               // CRC低字节先传
    frame[7] = (crc >> 8) & 0xFF;

    return 8;  // 帧长度
}

// 解析响应
int modbus_parse_response(const uint8_t *frame, uint16_t len,
                          uint16_t *out_data, uint16_t max_count) {
    if (len < 5) return -1;  // 太短

    // CRC检查
    uint16_t rx_crc = (frame[len-1] << 8) | frame[len-2];
    if (modbus_crc16(frame, len-2) != rx_crc) return -2;  // CRC错误

    // 地址检查
    if (frame[0] != MODBUS_SLAVE_ADDR) return -3;

    // 异常检查
    if (frame[1] & 0x80) {
        return -frame[2];  // 返回异常码
    }

    // 提取数据
    uint8_t byte_count = frame[2];
    if (byte_count > max_count * 2) return -4;

    for (int i = 0; i < byte_count / 2; i++) {
        out_data[i] = (frame[3 + i*2] << 8) | frame[4 + i*2];
    }

    return byte_count / 2;  // 返回寄存器数量
}
```

### 6.2 CAN总线通信

```c
// ============================================================
// CAN: 控制器局域网，汽车和工业控制标准
// 帧格式: 标识符 + 数据长度码 + 数据(0-8字节)
// ============================================================

typedef struct {
    uint32_t id;        // 标识符 (11位标准或29位扩展)
    uint8_t is_ext;     // 是否扩展帧
    uint8_t is_rtr;     // 是否远程帧
    uint8_t dlc;        // 数据长度码 (0-8)
    uint8_t data[8];    // 数据
} CAN_Frame;

// 发送CAN帧
int can_send(CAN_TypeDef *can, const CAN_Frame *frame) {
    // 找到空的发送邮箱
    uint32_t tsr = can->TSR;
    uint32_t mailbox;

    if (tsr & CAN_TSR_TME0) {
        mailbox = 0;
    } else if (tsr & CAN_TSR_TME1) {
        mailbox = 1;
    } else if (tsr & CAN_TSR_TME2) {
        mailbox = 2;
    } else {
        return -1;  // 无空邮箱
    }

    volatile uint32_t *tir = &can->sTxMailBox[mailbox].TIR;
    volatile uint32_t *tdtr = &can->sTxMailBox[mailbox].TDTR;
    volatile uint32_t *tdlr = &can->sTxMailBox[mailbox].TDLR;
    volatile uint32_t *tdhr = &can->sTxMailBox[mailbox].TDHR;

    // 配置标识符
    uint32_t id_reg;
    if (frame->is_ext) {
        id_reg = (frame->id << 3) | CAN_ID_EXT;
    } else {
        id_reg = (frame->id << 21);
    }
    if (frame->is_rtr) {
        id_reg |= CAN_RTR_REMOTE;
    }

    *tir = id_reg;
    *tdtr = frame->dlc;

    // 填充数据 (小端)
    uint32_t low = frame->data[0] | (frame->data[1] << 8) |
                   (frame->data[2] << 16) | (frame->data[3] << 24);
    uint32_t high = frame->data[4] | (frame->data[5] << 8) |
                    (frame->data[6] << 16) | (frame->data[7] << 24);
    *tdlr = low;
    *tdhr = high;

    // 请求发送
    *tir |= CAN_TI0R_TXRQ;

    return 0;
}

// CAN过滤器配置 (接收特定ID)
void can_filter_config(CAN_TypeDef *can, uint8_t bank, uint32_t id, uint32_t mask) {
    // 进入过滤器初始化模式
    can->FMR |= CAN_FMR_FINIT;

    // 选择过滤器模式: 掩码模式
    can->FS1R |= (1U << bank);  // 32位模式
    can->FM1R &= ~(1U << bank); // 掩码模式 (非列表)

    // 配置ID和掩码
    can->sFilterRegister[bank].FR1 = (id << 21);      // 标准ID左移21位
    can->sFilterRegister[bank].FR2 = (mask << 21);    // 掩码

    // 使能过滤器
    can->FA1R |= (1U << bank);

    // 退出初始化模式
    can->FMR &= ~CAN_FMR_FINIT;
}
```

---

## 七、电机与运动控制

### 7.1 PWM电机控制

```c
// ============================================================
// PWM: 脉冲宽度调制，电机调速的标准方法
// 占空比 = 高电平时间 / 周期
// ============================================================

// 初始化PWM (以TIM1为例，带死区插入，用于半桥驱动)
void pwm_init_motor(TIM_TypeDef *tim, uint32_t freq_hz) {
    // 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // 计算预分频和周期
    // PWM频率 = SystemClock / (PSC+1) / (ARR+1)
    uint32_t clock = SystemCoreClock;  // 168MHz
    uint32_t period = clock / freq_hz;  // 如1kHz -> 168000

    if (period > 65535) {
        // 需要预分频
        tim->PSC = (period / 65536);     // 预分频值
        tim->ARR = (period / (tim->PSC + 1)) - 1;
    } else {
        tim->PSC = 0;
        tim->ARR = period - 1;
    }

    // PWM模式1: 向上计数时，CNT < CCR输出有效电平
    tim->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;  // 模式1
    tim->CCMR1 |= TIM_CCMR1_OC1PE;  // 预装载使能

    // 互补输出 + 死区
    tim->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE;  // CH1和CH1N都使能

    // 死区时间配置 (DTG[7:5]=0xx => DT=DTG[7:0] * tdtg, tdtg=125ns @ 8MHz)
    // 1us死区: 1us / 125ns = 8
    tim->BDTR = (8 & 0x7F);  // DTG = 8
    tim->BDTR |= TIM_BDTR_MOE;  // 主输出使能

    // 启动
    tim->CR1 |= TIM_CR1_CEN;
}

// 设置占空比 (-1000 到 +1000，表示-100%到+100%)
// 负值: 反转，正值: 正转
void pwm_set_duty(TIM_TypeDef *tim, int16_t duty_permille) {
    // 限幅
    if (duty_permille > 1000) duty_permille = 1000;
    if (duty_permille < -1000) duty_permille = -1000;

    uint32_t arr = tim->ARR;
    uint32_t ccr;

    if (duty_permille >= 0) {
        // 正转
        ccr = (uint32_t)(duty_permille * arr / 1000);
        tim->CCR1 = ccr;
        // 如果需要方向控制，设置GPIO
        GPIO_SET_PIN(DIR_PORT, DIR_PIN);  // 假设高电平正转
    } else {
        // 反转
        ccr = (uint32_t)(-duty_permille * arr / 1000);
        tim->CCR1 = ccr;
        GPIO_RESET_PIN(DIR_PORT, DIR_PIN);
    }
}
```

### 7.2 步进电机控制

```c
// ============================================================
// 步进电机: 开环位置控制，每步固定角度
// 控制参数: 步数、方向、速度(脉冲频率)
// ============================================================

typedef struct {
    GPIO_TypeDef *step_port;    // 步进引脚
    uint16_t step_pin;
    GPIO_TypeDef *dir_port;     // 方向引脚
    uint16_t dir_pin;

    int32_t position;           // 当前位置 (步数)
    int32_t target;             // 目标位置
    uint32_t speed;             // 当前速度 (脉冲/秒)
    uint32_t max_speed;
    uint32_t accel;             // 加速度
} Stepper;

// 定时器中断生成步进脉冲
void Stepper_IRQHandler(Stepper *motor) {
    if (motor->position != motor->target) {
        // 生成步进脉冲
        GPIO_SET_PIN(motor->step_port, motor->step_pin);

        // 确定方向
        if (motor->target > motor->position) {
            GPIO_SET_PIN(motor->dir_port, motor->dir_pin);
            motor->position++;
        } else {
            GPIO_RESET_PIN(motor->dir_port, motor->dir_pin);
            motor->position--;
        }

        // 延时后拉低 (最小脉冲宽度)
        delay_us(1);
        GPIO_RESET_PIN(motor->step_port, motor->step_pin);

        // 计算下一步的定时器周期 (变速控制)
        int32_t dist = motor->target - motor->position;
        if (dist < 100) {
            // 接近目标，减速
            motor->speed = motor->speed * 9 / 10;  // 减速
        } else if (motor->speed < motor->max_speed) {
            // 加速
            motor->speed += motor->accel;
        }

        // 更新定时器周期
        uint32_t period = 1000000 / motor->speed;  // us
        set_timer_period(period);
    }
}

// 移动到绝对位置
void stepper_move_to(Stepper *motor, int32_t pos) {
    motor->target = pos;
    // 使能定时器中断
    enable_timer_irq();
}
```

---

## 八、传感器数据处理

### 8.1 ADC采样与滤波

```c
// ============================================================
// ADC: 模拟到数字转换，传感器数据采集的基础
// ============================================================

// 启动ADC转换
uint16_t adc_read_channel(ADC_TypeDef *adc, uint8_t channel) {
    // 配置通道
    adc->SQR3 = channel;  // 设置转换序列

    // 启动转换
    adc->CR2 |= ADC_CR2_SWSTART;

    // 等待完成
    while (!(adc->SR & ADC_SR_EOC));

    // 读取结果
    return (uint16_t)adc->DR;
}

// 多次采样平均 (减少噪声)
uint16_t adc_read_average(ADC_TypeDef *adc, uint8_t channel, uint8_t samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += adc_read_channel(adc, channel);
    }
    return (uint16_t)(sum / samples);
}

// 滑动平均滤波器 (实时滤波)
#define FILTER_SIZE 16

typedef struct {
    uint16_t buffer[FILTER_SIZE];
    uint8_t index;
    uint32_t sum;
} MovingAverageFilter;

void maf_init(MovingAverageFilter *f) {
    for (int i = 0; i < FILTER_SIZE; i++) {
        f->buffer[i] = 0;
    }
    f->index = 0;
    f->sum = 0;
}

uint16_t maf_update(MovingAverageFilter *f, uint16_t new_value) {
    // 减去最老的值
    f->sum -= f->buffer[f->index];

    // 加入新值
    f->buffer[f->index] = new_value;
    f->sum += new_value;

    // 更新索引
    f->index = (f->index + 1) % FILTER_SIZE;

    // 返回平均
    return (uint16_t)(f->sum / FILTER_SIZE);
}

// 中值滤波 (去除脉冲噪声)
uint16_t median_filter3(uint16_t a, uint16_t b, uint16_t c) {
    if (a > b) {
        if (b > c) return b;      // a > b > c
        if (a > c) return c;      // a > c > b
        return a;                 // c > a > b
    } else {
        if (a > c) return a;      // b > a > c
        if (b > c) return c;      // b > c > a
        return b;                 // c > b > a
    }
}
```

### 8.2 温度补偿算法

```c
// ============================================================
// 温度传感器: 通常需要线性化或查表
// NTC热敏电阻: R = R0 * exp(B*(1/T - 1/T0))
// ============================================================

#include <math.h>

// NTC参数
#define NTC_R0      10000.0f   // 25°C时阻值 10kΩ
#define NTC_T0      298.15f    // 25°C的绝对温度
#define NTC_B       3950.0f    // B值
#define PULLUP_R    10000.0f   // 上拉电阻

// ADC值转温度 (Steinhart-Hart简化)
float ntc_adc_to_temp(uint16_t adc_val, uint16_t adc_max) {
    if (adc_val == 0) return -999;  // 开路
    if (adc_val >= adc_max) return 999;  // 短路

    // 计算电阻值
    float v_ratio = (float)adc_val / adc_max;
    float r_ntc = PULLUP_R * v_ratio / (1.0f - v_ratio);

    // Steinhart-Hart方程
    float invT = 1.0f / NTC_T0 + (1.0f / NTC_B) * logf(r_ntc / NTC_R0);
    float temp_k = 1.0f / invT;
    float temp_c = temp_k - 273.15f;

    return temp_c;
}

// 查表法 (更快，适合资源受限)
// 预计算的ADC值到温度映射
static const int16_t temp_lut[] = {
    // ADC=0..4095对应的温度(乘以10)
    // 通过脚本预生成
    -400, -395, -390, /* ... */  // 索引0对应ADC=0
};

int16_t temp_lookup(uint16_t adc_val) {
    // 限幅
    if (adc_val >= sizeof(temp_lut)/sizeof(temp_lut[0])) {
        adc_val = sizeof(temp_lut)/sizeof(temp_lut[0]) - 1;
    }
    return temp_lut[adc_val];  // 返回温度*10
}
```

---

## 九、电源管理与低功耗

### 9.1 睡眠模式管理

```c
// ============================================================
// 低功耗模式: 空闲时降低功耗
// Cortex-M: Sleep, Deep Sleep, Standby
// ============================================================

// 进入睡眠模式，等待中断唤醒
void enter_sleep_mode(void) {
    // 配置系统控制寄存器
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;  // 清除DEEP位，普通睡眠

    // 执行WFI (Wait For Interrupt)
    __WFI();

    // 被中断唤醒后继续执行
}

// 进入停止模式 (更深睡眠，更低功耗)
void enter_stop_mode(void) {
    // 保存外设状态
    save_peripheral_state();

    // 配置唤醒源 (如RTC闹钟、外部中断)
    EXTI->IMR |= EXTI_IMR_MR0;  // 使能EXTI0

    // 进入Deep Sleep
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    // 配置电压调节器
    PWR->CR &= ~PWR_CR_PDDS;  // 停止模式，非待机
    PWR->CR |= PWR_CR_LPMS_STOP1;

    __WFI();

    // 唤醒后恢复
    restore_peripheral_state();
    SystemInit();  // 重新初始化时钟
}

// 主循环中的低功耗管理
void main_loop_lowpower(void) {
    while (1) {
        // 检查是否有工作要做
        if (has_pending_tasks()) {
            process_tasks();
        } else {
            // 空闲，进入低功耗
            // 设置一个定时器在下一个任务到期前唤醒
            set_wakeup_timer(get_next_task_time());
            enter_stop_mode();
        }
    }
}
```

### 9.2 外设电源门控

```c
// ============================================================
// 时钟门控: 不用的外设关闭时钟，减少功耗
// ============================================================

// 外设时钟管理
typedef enum {
    PERIPH_GPIOA = 0,
    PERIPH_GPIOB,
    PERIPH_USART1,
    PERIPH_USART2,
    PERIPH_SPI1,
    PERIPH_ADC1,
    PERIPH_TIM2,
    // ...
    PERIPH_COUNT
} PeriphID;

typedef struct {
    volatile uint32_t *enr;  // 时钟使能寄存器地址
    uint32_t bit;
    uint32_t use_count;      // 引用计数
} PeriphClock;

static PeriphClock periph_clocks[] = {
    [PERIPH_GPIOA]  = {&RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN, 0},
    [PERIPH_GPIOB]  = {&RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN, 0},
    [PERIPH_USART1] = {&RCC->APB2ENR, RCC_APB2ENR_USART1EN, 0},
    // ...
};

// 使能外设时钟 (引用计数)
void periph_clock_enable(PeriphID id) {
    if (id >= PERIPH_COUNT) return;

    PeriphClock *pc = &periph_clocks[id];

    if (pc->use_count == 0) {
        *pc->enr |= pc->bit;  // 首次使用，开启时钟
    }
    pc->use_count++;
}

// 关闭外设时钟
void periph_clock_disable(PeriphID id) {
    if (id >= PERIPH_COUNT) return;

    PeriphClock *pc = &periph_clocks[id];

    if (pc->use_count > 0) {
        pc->use_count--;
        if (pc->use_count == 0) {
            *pc->enr &= ~pc->bit;  // 无人使用，关闭时钟
        }
    }
}
```

---

## 十、可靠性设计技巧

### 10.1 看门狗机制

```c
// ============================================================
// 看门狗: 检测程序跑飞，自动复位
// 必须在设定时间内"喂狗"
// ============================================================

// 独立看门狗 (IWDG): 使用独立RC振荡器，即使主时钟失效也能工作
void iwdg_init(uint32_t timeout_ms) {
    // 使能寄存器访问
    IWDG->KR = 0x5555;

    // 配置预分频 (40kHz / 分频)
    uint32_t prescaler;
    if (timeout_ms < 100) {
        prescaler = IWDG_PR_PR_0;  // /4
    } else if (timeout_ms < 1000) {
        prescaler = IWDG_PR_PR_2;  // /16
    } else {
        prescaler = IWDG_PR_PR_3;  // /32
    }
    IWDG->PR = prescaler;

    // 配置重载值
    // 超时时间 = (重载值 + 1) * 分频 / 40000
    uint32_t reload = timeout_ms * 40 / (4 << prescaler);
    if (reload > 0xFFF) reload = 0xFFF;
    IWDG->RLR = reload;

    // 启动看门狗
    IWDG->KR = 0xCCCC;
}

// 喂狗
void iwdg_feed(void) {
    IWDG->KR = 0xAAAA;  // 写入重载值到计数器
}

// 窗口看门狗 (WWDG): 必须在窗口内喂狗，不能太早也不能太晚
void wwdg_init(uint8_t window, uint8_t counter) {
    // 使能WWDG时钟
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;

    // 配置窗口值
    WWDG->CFR = window;

    // 启动并设置计数器
    WWDG->CR = WWDG_CR_WDGA | counter;
}

// 喂窗口看门狗
void wwdg_feed(uint8_t counter) {
    // 检查是否在窗口内 (由硬件自动检查)
    WWDG->CR = counter;
}

// 安全的喂狗模式: 只在主循环喂，不在ISR中喂
void main_loop_safe(void) {
    while (1) {
        // 执行所有任务
        process_all_tasks();

        // 只有正常执行完才喂狗
        iwdg_feed();
    }
}
```

### 10.2 断言与故障处理

```c
// ============================================================
// 断言: 调试时捕获非法状态
// 发布时转为错误记录或安全状态
// ============================================================

#ifdef DEBUG
    #define ASSERT(cond) do { \
        if (!(cond)) { \
            __disable_irq(); \
            fault_handler(__FILE__, __LINE__, #cond); \
        } \
    } while(0)
#else
    #define ASSERT(cond) do { \
        if (!(cond)) { \
            log_fault(FAULT_ASSERT, __LINE__); \
            enter_safe_state(); \
        } \
    } while(0)
#endif

// 故障类型
typedef enum {
    FAULT_ASSERT = 0,
    FAULT_NULL_PTR,
    FAULT_STACK_OVERFLOW,
    FAULT_WDT_TIMEOUT,
    FAULT_BUS_ERROR,
    FAULT_COUNT
} FaultType;

// 故障记录
typedef struct {
    FaultType type;
    uint32_t timestamp;
    uint32_t param;
    const char *file;
    uint32_t line;
} FaultRecord;

#define FAULT_LOG_SIZE 10
static FaultRecord fault_log[FAULT_LOG_SIZE];
static uint8_t fault_count = 0;

void log_fault(FaultType type, uint32_t param) {
    if (fault_count < FAULT_LOG_SIZE) {
        fault_log[fault_count].type = type;
        fault_log[fault_count].timestamp = get_tick();
        fault_log[fault_count].param = param;
        fault_count++;
    }
}

// 进入安全状态
void enter_safe_state(void) {
    // 关闭所有输出
    disable_all_outputs();

    // 记录状态到非易失存储
    save_fault_log();

    // 尝试恢复或等待复位
    if (can_recover()) {
        attempt_recovery();
    } else {
        // 触发复位
        NVIC_SystemReset();
    }
}

// 使用断言检查关键条件
void motor_set_speed(Motor *m, int16_t speed) {
    ASSERT(m != NULL);                    // 空指针检查
    ASSERT(m->state != MOTOR_FAULT);      // 状态检查
    ASSERT(speed >= -MAX_SPEED && speed <= MAX_SPEED);  // 范围检查

    m->target_speed = speed;
}
```

---

## 附录: 完整示例 - 温度控制器

```c
// ============================================================
// 完整示例: PID温度控制器
// 硬件: NTC热敏电阻 + 加热器(PWM控制)
// ============================================================

#include <stdint.h>
#include <stdbool.h>

// ========== 配置 ==========
#define TEMP_SETPOINT   50.0f   // 目标温度 50°C
#define TEMP_HYSTERESIS 2.0f    // 回差 2°C
#define PWM_MAX         1000    // PWM最大值
#define ADC_CHANNEL     0       // ADC通道

// PID参数
#define KP  10.0f
#define KI  0.5f
#define KD  1.0f

// ========== 数据结构 ==========
typedef struct {
    float setpoint;
    float input;
    float output;
    float integral;
    float prev_error;
    float kp, ki, kd;
} PID_Controller;

// ========== 函数实现 ==========

void pid_init(PID_Controller *pid, float kp, float ki, float kd) {
    pid->setpoint = 0;
    pid->input = 0;
    pid->output = 0;
    pid->integral = 0;
    pid->prev_error = 0;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

float pid_compute(PID_Controller *pid, float input) {
    pid->input = input;
    float error = pid->setpoint - input;

    // 积分项 (带抗饱和)
    pid->integral += error;
    if (pid->integral > 1000) pid->integral = 1000;
    if (pid->integral < -1000) pid->integral = -1000;

    // 微分项
    float derivative = error - pid->prev_error;
    pid->prev_error = error;

    // 计算输出
    pid->output = pid->kp * error +
                  pid->ki * pid->integral +
                  pid->kd * derivative;

    // 限幅
    if (pid->output > PWM_MAX) pid->output = PWM_MAX;
    if (pid->output < 0) pid->output = 0;

    return pid->output;
}

// ADC读取温度 (NTC)
float read_temperature(void) {
    uint16_t adc = adc_read_average(ADC1, ADC_CHANNEL, 16);
    return ntc_adc_to_temp(adc, 4095);
}

// 设置加热器功率
void set_heater_power(uint16_t power) {
    pwm_set_duty(TIM1, power);
}

// ========== 主程序 ==========

int main(void) {
    // 初始化
    system_init();
    adc_init(ADC1);
    pwm_init_motor(TIM1, 1000);  // 1kHz PWM

    PID_Controller pid;
    pid_init(&pid, KP, KI, KD);
    pid.setpoint = TEMP_SETPOINT;

    // 看门狗
    iwdg_init(1000);  // 1秒超时

    while (1) {
        // 读取当前温度
        float temp = read_temperature();

        // PID计算
        float power = pid_compute(&pid, temp);

        // 输出到加热器
        set_heater_power((uint16_t)power);

        // 安全监控
        if (temp > 80.0f) {
            // 超温保护
            set_heater_power(0);
            log_fault(FAULT_OVER_TEMP, (uint32_t)(temp * 100));
        }

        // 喂狗
        iwdg_feed();

        // 100ms控制周期
        delay_ms(100);
    }
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 适用场景: 工业控制、汽车电子、家电控制、物联网终端
> - 参考标准: MISRA C, IEC 61508, ISO 26262
