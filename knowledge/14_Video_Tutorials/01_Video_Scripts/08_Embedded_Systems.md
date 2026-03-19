# 视频脚本：嵌入式C编程

> **视频标题**: 嵌入式C编程 - 从裸机到RTOS
> **视频时长**: 40分钟
> **目标受众**: 嵌入式系统开发者、硬件工程师
> **前置知识**: C语言基础、基本数字电路知识
> **关联文档**: 嵌入式系统开发

---

## 开场 (0:00 - 2:00)

### 0:00 | 引入 | 画面：嵌入式设备的多样性展示

**讲解词**:
嵌入式系统无处不在——从你的智能手表到汽车引擎控制单元，从家用电器到工业自动化设备，从医疗设备到航天器。这些设备运行的代码大多是用C语言编写的。今天，我们将深入嵌入式C编程的世界。

### 0:45 | 嵌入式系统的特点 | 画面：资源约束的可视化

**讲解词**:
嵌入式系统与桌面系统截然不同：资源受限——内存以KB计、CPU频率以MHz计；实时性要求——必须在严格时限内响应；可靠性要求——不能崩溃、不能丢失数据；功耗敏感——电池供电是常态。

**特点对比**:

```
桌面系统 vs 嵌入式系统:
┌──────────────┬────────────────┬──────────────────┐
│ 特性         │ 桌面系统        │ 嵌入式系统        │
├──────────────┼────────────────┼──────────────────┤
│ CPU          │ 3+ GHz, 多核    │ 16-500 MHz, 单核  │
│ RAM          │ 8-64 GB         │ 4 KB - 4 MB       │
│ 存储         │ SSD 256GB+      │ 32 KB - 1 MB Flash│
│ 功耗         │ 65-250W         │ 1mW - 5W          │
│ 实时性       │ 软实时          │ 硬实时            │
│ 可靠性       │ 可重启          │ 不能失败          │
│ 成本         │ $300-3000       │ $0.50-50          │
└──────────────┴────────────────┴──────────────────┘
```

### 1:30 | 主题预告 | 画面：课程大纲

**讲解词**:
今天我们将学习：嵌入式C基础、内存映射I/O、中断处理、裸机编程、RTOS集成、调试技术，以及完整的项目案例。

---

## 第一部分：嵌入式C基础 (2:00 - 8:00)

### 2:00 | 开发环境搭建 | 画面：交叉工具链的架构

**讲解词**:
嵌入式开发需要交叉编译工具链——在PC上编译，在目标设备上运行。常见的工具链包括ARM GCC、RISC-V GCC，以及厂商特定的工具如Keil、IAR。

**工具链组件**:

```
交叉工具链:
├── 编译器
│   ├── arm-none-eabi-gcc    (ARM)
│   ├── riscv32-unknown-elf-gcc (RISC-V)
│   └── avr-gcc              (AVR)
├── 二进制工具
│   ├── objdump (反汇编)
│   ├── objcopy (格式转换)
│   ├── size    (大小分析)
│   └── nm      (符号表)
├── 调试器
│   ├── gdb-multiarch
│   └── openocd
└── 构建工具
    ├── Make/CMake
    └── 厂商IDE
```

### 2:50 | 启动代码 | 画面：启动过程的详细流程

**讲解词**:
嵌入式程序从复位向量开始执行。启动代码负责初始化栈指针、复制.data段、清零.bss段、调用main函数。

**启动代码示例**:

```c
// startup.c - 简化的启动代码

// 链接器脚本定义的符号
extern uint32_t _estack;        // 栈顶
extern uint32_t _sidata;        // ROM中的数据起始
extern uint32_t _sdata;         // RAM数据起始
extern uint32_t _edata;         // RAM数据结束
extern uint32_t _sbss;          // BSS起始
extern uint32_t _ebss;          // BSS结束

// 主函数声明
extern int main(void);

// 复位处理程序
void Reset_Handler(void) {
    // 1. 设置栈指针
    __asm volatile ("ldr sp, =_estack");

    // 2. 复制.data段从Flash到RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 3. 清零.bss段
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // 4. 调用系统初始化 (可选)
    SystemInit();

    // 5. 调用main
    main();

    // 6. 如果main返回，无限循环
    while (1);
}

// 向量表
__attribute__((section(".isr_vector")))
void (*const vector_table[])(void) = {
    (void (*)(void))&_estack,   // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,                 // NMI
    HardFault_Handler,           // 硬 fault
    // ... 更多中断
};
```

### 4:10 | 链接器脚本 | 画面：内存布局的脚本定义

**讲解词**:
链接器脚本定义内存布局和代码放置。这是嵌入式开发的关键文件，决定代码、数据在Flash和RAM中的位置。

**链接器脚本示例**:

```ld
/* linker_script.ld */

/* 内存定义 */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K
}

/* 栈顶 */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* 段定义 */
SECTIONS
{
    /* 代码和只读数据放在Flash */
    .text :
    {
        KEEP(*(.isr_vector))    /* 向量表 */
        *(.text*)               /* 代码 */
        *(.rodata*)             /* 只读数据 */
        _etext = .;
    } > FLASH

    /* 初始化数据在Flash，复制到RAM */
    _sidata = LOADADDR(.data);
    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    /* 未初始化数据在RAM */
    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /* 堆起始 */
    _heap_start = _ebss;
}
```

### 5:30 | 内存映射I/O | 画面：寄存器映射的可视化

**讲解词**:
嵌入式系统中，外设寄存器被映射到内存地址空间。通过读写特定地址，可以控制硬件外设。

**寄存器访问**:

```c
// 方式1: 直接地址
#define GPIOA_ODR  (*(volatile uint32_t *)0x40020014)
GPIOA_ODR = 0xFF;  // 设置GPIOA输出

// 方式2: 结构体映射 (推荐)
typedef struct {
    volatile uint32_t MODER;    // 0x00
    volatile uint32_t OTYPER;   // 0x04
    volatile uint32_t OSPEEDR;  // 0x08
    volatile uint32_t PUPDR;    // 0x0C
    volatile uint32_t IDR;      // 0x10
    volatile uint32_t ODR;      // 0x14
    volatile uint32_t BSRR;     // 0x18
    volatile uint32_t LCKR;     // 0x1C
} GPIO_TypeDef;

#define GPIOA ((GPIO_TypeDef *)0x40020000)

// 使用
GPIOA->MODER |= (1 << 0);   // 设置PA0为输出
GPIOA->ODR |= (1 << 0);     // PA0输出高

// volatile关键字至关重要！
// 告诉编译器不要优化掉这些访问
```

### 6:40 | 位操作技术 | 画面：常见位操作的可视化

**讲解词**:
嵌入式编程大量使用位操作来配置寄存器。掌握位操作是嵌入式C的基本功。

**位操作模式**:

```c
// 设置位 (置1)
REG |= (1 << n);           // 设置第n位
REG |= (1 << 3) | (1 << 5); // 设置第3和第5位

// 清除位 (置0)
REG &= ~(1 << n);          // 清除第n位
REG &= ~((1 << 3) | (1 << 5));

// 翻转位
REG ^= (1 << n);           // 翻转第n位

// 读取位
if (REG & (1 << n)) {      // 检查第n位是否为1
    // bit is set
}

// 修改位域 (多位)
// 先清除，再设置
REG &= ~(0xF << 4);        // 清除位4-7
REG |= (value << 4);       // 设置新的值

// 使用位域结构 (注意可移植性)
typedef union {
    uint32_t raw;
    struct {
        uint32_t EN : 1;      // 位0
        uint32_t MODE : 2;    // 位1-2
        uint32_t RESERVED : 29;
    } bits;
} Control_Reg;
```

---

## 第二部分：中断与异常 (8:00 - 14:00)

### 8:00 | 中断概念 | 画面：中断流程的动画

**讲解词**:
中断允许外设在需要CPU注意时异步通知。中断发生时，CPU暂停当前任务，执行中断服务程序(ISR)，完成后返回原任务。

**中断流程**:

```
正常执行流程          中断发生
    │                    │
    ▼                    ▼
┌────────┐          ┌──────────┐
│ Task A │          │ 保存现场  │
│ 运行中  │─────────►│ (寄存器) │
└────────┘          └────┬─────┘
    ▲                    │
    │              ┌─────▼─────┐
    │              │ 执行ISR   │
    │              │ 处理事件  │
    │              └─────┬─────┘
    │                    │
    │              ┌─────▼─────┐
    └──────────────┤ 恢复现场  │
                   │ 继续Task A│
                   └───────────┘
```

### 8:50 | 向量表 | 画面：ARM Cortex-M向量表结构

**讲解词**:
向量表是一个函数指针数组，每个条目对应一个中断或异常。复位后，CPU从向量表获取初始栈指针和复位处理程序地址。

**向量表示例**:

```c
// Cortex-M4 向量表
typedef void (* const Handler)(void);

__attribute__((section(".isr_vector")))
Handler vector_table[] = {
    // Cortex-M 系统异常 (前16个)
    (Handler)&_estack,          // 0: 初始栈指针
    Reset_Handler,               // 1: 复位
    NMI_Handler,                 // 2: 不可屏蔽中断
    HardFault_Handler,           // 3: 硬故障
    MemManage_Handler,           // 4: 内存管理故障
    BusFault_Handler,            // 5: 总线故障
    UsageFault_Handler,          // 6: 使用故障
    0, 0, 0, 0,                  // 7-10: 保留
    SVC_Handler,                 // 11: SVCall
    DebugMon_Handler,            // 12: 调试监控
    0,                           // 13: 保留
    PendSV_Handler,              // 14: PendSV
    SysTick_Handler,             // 15: SysTick

    // 外设中断 (设备特定)
    WWDG_IRQHandler,             // 0: 窗口看门狗
    PVD_IRQHandler,              // 1: PVD
    TAMP_STAMP_IRQHandler,       // 2: RTC Tamper
    // ... 更多中断
};
```

### 9:50 | 编写ISR | 画面：中断服务程序的最佳实践

**讲解词**:
ISR应该简短快速。长时间执行的ISR会阻塞其他中断，影响系统响应。复杂的处理应该放在主循环中。

**ISR示例**:

```c
// 定时器中断服务程序
volatile uint32_t tick_count = 0;

void TIM2_IRQHandler(void) {
    // 检查中断标志
    if (TIM2->SR & TIM_SR_UIF) {
        // 清除标志 (必须！)
        TIM2->SR &= ~TIM_SR_UIF;

        // 快速操作
        tick_count++;

        // 设置标志让主循环处理
        system_flags |= FLAG_TICK;
    }
}

// 主循环
int main(void) {
    system_init();

    while (1) {
        if (system_flags & FLAG_TICK) {
            system_flags &= ~FLAG_TICK;
            // 处理tick，可以花费较长时间
            process_1ms_tasks();
        }

        // 其他任务
        process_background_tasks();

        // 低功耗模式 (等待中断)
        __WFI();
    }
}
```

### 11:00 | 中断优先级 | 画面：嵌套中断的动画

**讲解词**:
现代MCU支持中断优先级，高优先级中断可以抢占低优先级中断。这需要仔细设计以避免优先级反转和死锁。

**优先级配置**:

```c
// Cortex-M 优先级配置
// 优先级数值越小，优先级越高

void NVIC_Init(void) {
    // 配置SysTick为最低优先级
    NVIC_SetPriority(SysTick_IRQn, 15);

    // 配置定时器2为中等优先级
    NVIC_SetPriority(TIM2_IRQn, 5);

    // 配置按键中断为高优先级
    NVIC_SetPriority(EXTI0_IRQn, 2);

    // 使能中断
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

// 关键区保护
uint32_t primask;

void enter_critical(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}

void exit_critical(void) {
    __set_PRIMASK(primask);
}

// 使用
enter_critical();
// 访问共享资源
exit_critical();
```

### 12:15 | 中断延迟 | 画面：延迟来源的分析

**讲解词**:
中断延迟是从中断发生到ISR开始执行的时间。影响因素包括：当前指令执行时间、更高优先级中断、缓存未命中等。

**延迟优化**:

```
延迟来源:
├── 固有延迟 (12个时钟周期 Cortex-M)
│   └── 取指、压栈、取向量
├── 当前指令
│   ├── 简单指令: 1-3周期
│   ├── 乘法: 3-5周期
│   └── 除法: 2-12周期
└── 更高优先级中断
    └── 需要等待其完成

优化策略:
1. 使用尾链优化 (Tail-chaining)
2. 延迟压栈 (Late-arrival)
3. 保持ISR简短
4. 优化缓存使用
```

### 13:10 | 异常处理 | 画面：Fault异常的诊断

**讲解词**:
Cortex-M有专门的Fault异常处理硬故障、总线故障、使用故障和内存管理故障。这些可以用于调试和错误恢复。

**Fault处理**:

```c
// HardFault处理程序
void HardFault_Handler(void) {
    __asm volatile (
        "tst lr, #4\n"           // 检查使用的栈
        "ite eq\n"
        "mrseq r0, msp\n"        // 使用MSP
        "mrsne r0, psp\n"        // 使用PSP
        "ldr r1, [r0, #24]\n"    // 获取返回地址
        "b HardFault_C\n"        // 调用C处理
    );
}

void HardFault_C(uint32_t *stack_frame) {
    uint32_t r0  = stack_frame[0];
    uint32_t r1  = stack_frame[1];
    uint32_t r2  = stack_frame[2];
    uint32_t r3  = stack_frame[3];
    uint32_t r12 = stack_frame[4];
    uint32_t lr  = stack_frame[5];
    uint32_t pc  = stack_frame[6];  // 出错指令
    uint32_t psr = stack_frame[7];

    // 记录故障信息
    fault_info.r0 = r0;
    fault_info.pc = pc;
    fault_info.lr = lr;
    fault_info.CFSR = SCB->CFSR;
    fault_info.HFSR = SCB->HFSR;
    fault_info.MMFAR = SCB->MMFAR;
    fault_info.BFAR = SCB->BFAR;

    // 重启或进入安全模式
    system_reset();
}
```

---

## 第三部分：外设驱动开发 (14:00 - 22:00)

### 14:00 | GPIO驱动 | 画面：GPIO的多种配置模式

**讲解词**:
GPIO是最基础的外设。需要配置模式（输入、输出、复用、模拟）、速度、上下拉电阻等。

**GPIO驱动**:

```c
// gpio_driver.h
typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 1,
    GPIO_MODE_AF = 2,
    GPIO_MODE_ANALOG = 3
} GPIO_Mode;

typedef enum {
    GPIO_SPEED_LOW = 0,
    GPIO_SPEED_MEDIUM = 1,
    GPIO_SPEED_HIGH = 2,
    GPIO_SPEED_VERY_HIGH = 3
} GPIO_Speed;

typedef enum {
    GPIO_NOPULL = 0,
    GPIO_PULLUP = 1,
    GPIO_PULLDOWN = 2
} GPIO_Pull;

typedef struct {
    uint8_t pin;
    GPIO_Mode mode;
    GPIO_Speed speed;
    GPIO_Pull pull;
    uint8_t af;  // 复用功能
} GPIO_Config;

// gpio_driver.c
void GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_Config *config) {
    uint32_t pin = config->pin;

    // 使能时钟
    if (GPIOx == GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    else if (GPIOx == GPIOB) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // ...

    // 配置模式
    GPIOx->MODER &= ~(3 << (pin * 2));
    GPIOx->MODER |= (config->mode << (pin * 2));

    // 配置输出类型
    GPIOx->OTYPER &= ~(1 << pin);

    // 配置速度
    GPIOx->OSPEEDR &= ~(3 << (pin * 2));
    GPIOx->OSPEEDR |= (config->speed << (pin * 2));

    // 配置上拉/下拉
    GPIOx->PUPDR &= ~(3 << (pin * 2));
    GPIOx->PUPDR |= (config->pull << (pin * 2));

    // 配置复用功能
    if (config->mode == GPIO_MODE_AF) {
        if (pin < 8) {
            GPIOx->AFR[0] &= ~(0xF << (pin * 4));
            GPIOx->AFR[0] |= (config->af << (pin * 4));
        } else {
            GPIOx->AFR[1] &= ~(0xF << ((pin - 8) * 4));
            GPIOx->AFR[1] |= (config->af << ((pin - 8) * 4));
        }
    }
}

void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint8_t pin, uint8_t value) {
    if (value) {
        GPIOx->BSRR = (1 << pin);      // 置位
    } else {
        GPIOx->BSRR = (1 << (pin + 16)); // 复位
    }
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin) {
    return (GPIOx->IDR >> pin) & 1;
}

// 使用示例
void gpio_example(void) {
    // 配置PA5为输出 (LED)
    GPIO_Config led_cfg = {
        .pin = 5,
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_LOW,
        .pull = GPIO_NOPULL
    };
    GPIO_Init(GPIOA, &led_cfg);

    // 闪烁LED
    while (1) {
        GPIO_WritePin(GPIOA, 5, 1);
        delay_ms(500);
        GPIO_WritePin(GPIOA, 5, 0);
        delay_ms(500);
    }
}
```

### 16:00 | UART驱动 | 画面：串口通信的时序图

**讲解词**:
UART是最常用的调试和通信接口。驱动需要配置波特率、数据位、停止位、校验位，处理发送和接收。

**UART驱动**:

```c
// uart_driver.c

#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 256

static volatile uint8_t tx_buffer[UART_TX_BUF_SIZE];
static volatile uint8_t rx_buffer[UART_RX_BUF_SIZE];
static volatile uint16_t tx_head = 0, tx_tail = 0;
static volatile uint16_t rx_head = 0, rx_tail = 0;

void UART_Init(USART_TypeDef *USARTx, uint32_t baudrate) {
    // 使能时钟
    if (USARTx == USART1) RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    else if (USARTx == USART2) RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 配置波特率
    uint32_t apb_clock = (USARTx == USART1) ? 84000000 : 42000000;
    uint32_t usartdiv = (apb_clock + baudrate/2) / baudrate;
    USARTx->BRR = usartdiv;

    // 使能发送、接收、接收中断
    USARTx->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

    // 使能USART
    USARTx->CR1 |= USART_CR1_UE;

    // 使能中断
    if (USARTx == USART1) NVIC_EnableIRQ(USART1_IRQn);
    else if (USARTx == USART2) NVIC_EnableIRQ(USART2_IRQn);
}

void UART_SendChar(USART_TypeDef *USARTx, uint8_t c) {
    uint16_t next_head = (tx_head + 1) % UART_TX_BUF_SIZE;

    // 等待缓冲区空间
    while (next_head == tx_tail);

    tx_buffer[tx_head] = c;
    tx_head = next_head;

    // 使能发送中断
    USARTx->CR1 |= USART_CR1_TXEIE;
}

void UART_SendString(USART_TypeDef *USARTx, const char *str) {
    while (*str) {
        UART_SendChar(USARTx, *str++);
    }
}

// 中断服务程序
void USART2_IRQHandler(void) {
    // 接收中断
    if (USART2->SR & USART_SR_RXNE) {
        uint8_t c = USART2->DR;
        uint16_t next_head = (rx_head + 1) % UART_RX_BUF_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }

    // 发送中断
    if (USART2->SR & USART_SR_TXE) {
        if (tx_head != tx_tail) {
            USART2->DR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % UART_TX_BUF_SIZE;
        } else {
            // 缓冲区空，禁用发送中断
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}
```

### 18:15 | ADC驱动 | 画面：ADC转换过程的时序

**讲解词**:
ADC将模拟信号转换为数字值。驱动需要配置采样时间、分辨率、触发方式，支持单次或连续转换。

**ADC驱动**:

```c
// adc_driver.c

void ADC_Init(ADC_TypeDef *ADCx) {
    // 使能时钟
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 配置ADC
    ADCx->CR1 = 0;  // 12位分辨率
    ADCx->CR2 = ADC_CR2_ADON;  // 上电

    // 校准
    ADCx->CR2 |= ADC_CR2_CAL;
    while (ADCx->CR2 & ADC_CR2_CAL);

    // 配置通道
    ADCx->SMPR2 = ADC_SMPR2_SMP0_0 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_2;
    // 239.5周期采样时间

    // 单通道序列
    ADCx->SQR1 = 0;  // 1个转换
    ADCx->SQR3 = 0;  // 通道0
}

uint16_t ADC_Read(ADC_TypeDef *ADCx, uint8_t channel) {
    // 设置通道
    ADCx->SQR3 = channel;

    // 启动转换
    ADCx->CR2 |= ADC_CR2_SWSTART;

    // 等待完成
    while (!(ADCx->SR & ADC_SR_EOC));

    // 读取结果
    return (uint16_t)ADCx->DR;
}

// 使用DMA的多通道扫描
void ADC_Scan_Init(ADC_TypeDef *ADCx, uint16_t *buffer, uint8_t num_channels) {
    // 配置DMA
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)buffer;
    DMA1_Channel1->CNDTR = num_channels;
    DMA1_Channel1->CCR = DMA_CCR_MINC | DMA_CCR_CIRC |
                         DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0;
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    // 配置ADC扫描模式
    ADCx->CR1 |= ADC_CR1_SCAN;
    ADCx->CR2 |= ADC_CR2_DMA | ADC_CR2_CONT;

    // 配置通道序列
    ADCx->SQR1 = ((num_channels - 1) << 20);
    for (int i = 0; i < num_channels; i++) {
        if (i < 6) {
            ADCx->SQR3 |= (i << (i * 5));
        } else {
            ADCx->SQR2 |= (i << ((i - 6) * 5));
        }
    }

    // 启动
    ADCx->CR2 |= ADC_CR2_ADON;
    ADCx->CR2 |= ADC_CR2_SWSTART;
}
```

### 20:00 | 定时器 | 画面：定时器工作模式的展示

**讲解词**:
定时器是嵌入式系统的核心，用于延时、PWM生成、输入捕获等。

**定时器驱动**:

```c
// timer_driver.c

// 延时定时器
void TIM_Delay_Init(TIM_TypeDef *TIMx, uint32_t us) {
    // 使能时钟
    if (TIMx == TIM2) RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    else if (TIMx == TIM3) RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 配置预分频和计数值
    // 假设TIM时钟为42MHz
    TIMx->PSC = 42 - 1;      // 分频到1MHz
    TIMx->ARR = us - 1;      // 计数到us

    // 单次模式
    TIMx->CR1 = TIM_CR1_OPM;
}

void TIM_Delay_us(TIM_TypeDef *TIMx, uint32_t us) {
    TIMx->ARR = us - 1;
    TIMx->CNT = 0;
    TIMx->CR1 |= TIM_CR1_CEN;
    while (TIMx->CR1 & TIM_CR1_CEN);  // 等待完成
}

// PWM输出
void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t channel, uint32_t freq, uint8_t duty) {
    // 使能时钟
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 配置GPIO为复用推挽
    // ... (GPIO配置代码)

    // 配置TIM为PWM模式
    uint32_t tim_clock = 84000000;
    uint32_t period = tim_clock / freq - 1;

    TIMx->PSC = 0;           // 不分频
    TIMx->ARR = period;
    TIMx->CCR1 = (period * duty) / 100;  // 占空比

    // PWM模式1
    TIMx->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    TIMx->CCER = TIM_CCER_CC1E;  // 使能输出

    TIMx->CR1 = TIM_CR1_CEN;  // 启动
}
```

---

## 第四部分：RTOS集成 (22:00 - 30:00)

### 22:00 | RTOS简介 | 画面：RTOS的架构和调度

**讲解词**:
实时操作系统(RTOS)提供任务调度、同步、通信等服务，简化复杂嵌入式应用开发。FreeRTOS是最流行的开源RTOS。

**RTOS特点**:

```
RTOS vs 裸机:
┌──────────────┬────────────────┬──────────────────┐
│ 特性         │ 裸机            │ RTOS              │
├──────────────┼────────────────┼──────────────────┤
│ 任务管理     │ 手动状态机      │ 多任务调度        │
│ 同步         │ 手动            │ 信号量、互斥量    │
│ 通信         │ 全局变量        │ 队列、邮箱        │
│ 定时         │ 手动            │ 软件定时器        │
│ 内存占用     │ 最小            │ 4-10KB            │
│ 复杂度       │ 简单应用        │ 复杂应用          │
└──────────────┴────────────────┴──────────────────┘
```

### 22:45 | FreeRTOS基础 | 画面：FreeRTOS的任务状态图

**讲解词**:
FreeRTOS提供抢占式优先级调度。任务可以处于运行、就绪、阻塞、挂起状态。调度器始终运行最高优先级的就绪任务。

**任务创建**:

```c
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// 任务函数
void vSensorTask(void *pvParameters) {
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1) {
        // 读取传感器
        int value = ADC_Read(ADC1, 0);

        // 发送到队列
        xQueueSend(sensor_queue, &value, 0);

        // 精确延时 (100Hz)
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
}

void vDisplayTask(void *pvParameters) {
    (void)pvParameters;
    int value;

    while (1) {
        // 等待数据
        if (xQueueReceive(sensor_queue, &value, portMAX_DELAY)) {
            // 更新显示
            LCD_Printf("Value: %d\n", value);
        }
    }
}

// 队列句柄
QueueHandle_t sensor_queue;

int main(void) {
    hardware_init();

    // 创建队列
    sensor_queue = xQueueCreate(10, sizeof(int));

    // 创建任务
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 2, NULL);
    xTaskCreate(vDisplayTask, "Display", 256, NULL, 1, NULL);

    // 启动调度器
    vTaskStartScheduler();

    // 不应该到达这里
    while (1);
}
```

### 24:15 | 任务同步 | 画面：信号量和互斥量的使用

**讲解词**:
RTOS提供多种同步机制。二值信号量用于任务通知，计数信号量用于资源计数，互斥量用于互斥访问，递归互斥量允许同一任务多次获取。

**同步示例**:

```c
// 互斥量保护共享资源
SemaphoreHandle_t spi_mutex;

void vSPITask(void *pvParameters) {
    while (1) {
        // 获取互斥量
        if (xSemaphoreTake(spi_mutex, portMAX_DELAY) == pdTRUE) {
            // 访问SPI总线
            SPI_Transfer(data, len);

            // 释放互斥量
            xSemaphoreGive(spi_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// 二值信号量用于中断通知
SemaphoreHandle_t data_ready_sem;

void EXTI_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // 通知任务
    xSemaphoreGiveFromISR(data_ready_sem, &xHigherPriorityTaskWoken);

    // 上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vProcessingTask(void *pvParameters) {
    while (1) {
        // 等待数据就绪
        if (xSemaphoreTake(data_ready_sem, portMAX_DELAY)) {
            // 处理数据
            process_data();
        }
    }
}
```

### 25:45 | 中断与RTOS | 画面：中断中使用的API

**讲解词**:
在中断服务程序中使用RTOS API需要特别小心。必须使用FromISR后缀的变体，并考虑上下文切换。

**中断安全代码**:

```c
void TIM2_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;

        // 通知任务 (FromISR版本)
        xTaskNotifyGiveFromISR(task_handle, &xHigherPriorityTaskWoken);

        // 或者使用队列
        int data = read_sensor();
        xQueueSendFromISR(queue, &data, &xHigherPriorityTaskWoken);

        // 或者使用定时器
        xTimerPendFunctionCallFromISR(function, param,
                                       priority, &xHigherPriorityTaskWoken);
    }

    // 可能的上下文切换
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

### 26:40 | 内存管理 | 画面：FreeRTOS堆分配方案

**讲解词**:
FreeRTOS提供多种内存分配方案。Heap_1最简单但不支持释放；Heap_2支持释放但不合并；Heap_3使用标准库malloc；Heap_4和Heap_5支持合并和多个堆区域。

**内存配置**:

```c
// FreeRTOSConfig.h 配置

#define configTOTAL_HEAP_SIZE           (16 * 1024)  // 16KB堆
#define configUSE_MALLOC_FAILED_HOOK    1
#define configCHECK_FOR_STACK_OVERFLOW  2

// 任务栈大小
#define configMINIMAL_STACK_SIZE        (128)

// 创建任务时指定栈大小
xTaskCreate(vTask, "Name", 512, param, priority, &handle);

// 动态内存分配
void *buffer = pvPortMalloc(256);
if (buffer != NULL) {
    // 使用
    vPortFree(buffer);
}

// 静态分配 (推荐用于安全关键系统)
static StaticTask_t task_buffer;
static StackType_t stack_buffer[256];

TaskHandle_t handle = xTaskCreateStatic(
    vTask, "Name", 256, param, priority,
    stack_buffer, &task_buffer
);
```

### 27:50 | 低功耗模式 | 画面：FreeRTOS与低功耗的结合

**讲解词**:
电池供电设备需要低功耗模式。FreeRTOS支持Tickless Idle，在空闲时停止系统滴答，进入深度睡眠。

**低功耗配置**:

```c
// FreeRTOSConfig.h
#define configUSE_TICKLESS_IDLE     1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 2

// 实现钩子函数
void vApplicationSleep(TickType_t xExpectedIdleTime) {
    // 计算睡眠时间
    uint32_t sleep_ticks = xExpectedIdleTime;
    uint32_t sleep_ms = sleep_ticks * portTICK_PERIOD_MS;

    // 配置RTC唤醒
    RTC_SetWakeup(sleep_ms);

    // 进入停止模式
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    // 唤醒后重新配置系统时钟
    SystemClock_Config();
}

// 任务设计 - 使用事件驱动
void vTask(void *pv) {
    while (1) {
        // 等待事件，阻塞任务
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // 处理事件
        process_event();

        // 完成后进入空闲，系统可进入低功耗
    }
}
```

---

## 第五部分：调试与优化 (30:00 - 36:00)

### 30:00 | 调试接口 | 画面：JTAG和SWD的连接图

**讲解词**:
嵌入式调试使用JTAG或SWD接口。SWD只需两根线，是现代ARM设备的标准。调试器如ST-Link、J-Link连接到这些接口。

**调试配置**:

```
调试接口:
┌─────────────┐      ┌──────────┐      ┌─────────────┐
│   PC/GDB    │ USB  │ ST-Link  │ SWD  │  Target MCU │
│             │─────►│  /J-Link │─────►│             │
│ 调试软件     │      │ 调试器   │      │  STM32/...  │
└─────────────┘      └──────────┘      └─────────────┘

SWD信号:
- SWDIO: 数据输入输出
- SWCLK: 时钟
- GND: 地
- NRST: 复位 (可选)
- VCC: 目标电压检测

JTAG信号 (5线):
- TDI, TDO, TMS, TCK, TRST
```

### 30:50 | GDB调试 | 画面：GDB命令的实际使用

**讲解词**:
使用GDB配合OpenOCD可以进行源代码级调试。可以设置断点、单步执行、查看变量、检查内存。

**GDB使用**:

```bash
# 启动OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# 另一个终端启动GDB
arm-none-eabi-gdb firmware.elf

(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) load                    # 加载程序
(gdb) break main
(gdb) continue

(gdb) step                    # 单步
(gdb) next                    # 单步(跳过函数)
(gdb) print variable          # 打印变量
(gdb) x/16wx 0x20000000       # 检查内存
(gdb) info registers          # 查看寄存器
(gdb) backtrace               # 调用栈

(gdb) monitor reset           # 复位目标
```

### 31:45 | 日志和追踪 | 画面：ITM和SWO的使用

**讲解词**:
ITM(仪器跟踪宏单元)提供高性能的日志输出，不占用UART。通过SWO引脚输出，可以在不影响实时性的情况下调试。

**ITM使用**:

```c
// itm_log.c

#include "core_cm4.h"

void ITM_Enable(void) {
    // 使能ITM
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    ITM->LAR = 0xC5ACCE55;  // 解锁
    ITM->TER[0] = 1;        // 使能端口0
    ITM->TCR = ITM_TCR_ITMENA_Msk;
}

void ITM_SendChar(char c) {
    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) &&
        (ITM->TER[0] & 1)) {
        while (ITM->PORT[0].u32 == 0);  // 等待空闲
        ITM->PORT[0].u8 = c;
    }
}

void ITM_Print(const char *str) {
    while (*str) {
        ITM_SendChar(*str++);
    }
}

// 重定向printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        ITM_SendChar(ptr[i]);
    }
    return len;
}
```

### 32:40 | 性能分析 | 画面：使用DWT计数器的示例

**讲解词**:
Cortex-M提供DWT(数据观察点和追踪)单元，可以精确测量代码执行周期，无需额外硬件。

**性能测量**:

```c
// profiling.c

#include "core_cm4.h"

void DWT_Init(void) {
    // 使能DWT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;           // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // 使能
}

static inline uint32_t DWT_GetCycles(void) {
    return DWT->CYCCNT;
}

// 测量函数执行时间
void measure_function(void) {
    uint32_t start = DWT_GetCycles();

    // 被测代码
    function_to_measure();

    uint32_t end = DWT_GetCycles();
    uint32_t cycles = end - start;

    printf("Function took %u cycles\n", cycles);
}

// 更精确的测量 (避免缓存影响)
#define MEASURE_START() \
    DWT->CYCCNT = 0; \
    __DSB(); \
    uint32_t _start = DWT->CYCCNT

#define MEASURE_END(msg) do { \
    __DSB(); \
    uint32_t _end = DWT->CYCCNT; \
    printf(msg ": %u cycles\n", _end - _start); \
} while(0)
```

### 33:40 | 内存分析 | 画面：栈使用和堆碎片的检查

**讲解词**:
嵌入式系统中内存有限，需要仔细监控栈使用和堆碎片。FreeRTOS提供运行时统计功能。

**内存监控**:

```c
// 栈使用监控
void check_stack_usage(void) {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                            uxArraySize, &ulTotalRunTime);

        printf("Task Stack High Water Mark:\n");
        for (x = 0; x < uxArraySize; x++) {
            printf("  %s: %u words free\n",
                pxTaskStatusArray[x].pcTaskName,
                pxTaskStatusArray[x].usStackHighWaterMark);
        }

        vPortFree(pxTaskStatusArray);
    }
}

// 堆使用监控
void check_heap_usage(void) {
    size_t free = xPortGetFreeHeapSize();
    size_t min_free = xPortGetMinimumEverFreeHeapSize();

    printf("Heap: free=%u, min_ever=%u\n", free, min_free);
}

// 栈溢出钩子
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    printf("STACK OVERFLOW in task: %s\n", pcTaskName);
    while (1);  // 停止
}
```

### 34:35 | 代码优化 | 画面：优化前后的对比

**讲解词**:
嵌入式代码优化需要在速度、大小、功耗间权衡。关键路径用-O3，非关键用-Os，避免浮点运算，使用查找表。

**优化技术**:

```c
// 优化1: 用查找表代替计算
// 不好
float sin_approx(float x) {
    return x - x*x*x/6 + x*x*x*x*x/120;
}

// 好
const float sin_table[256] = { /* ... */ };
float sin_lookup(uint8_t angle) {
    return sin_table[angle];
}

// 优化2: 循环展开
// 不好
for (int i = 0; i < 100; i++) {
    process(data[i]);
}

// 好
for (int i = 0; i < 100; i += 4) {
    process(data[i]);
    process(data[i+1]);
    process(data[i+2]);
    process(data[i+3]);
}

// 优化3: 使用位运算
// 不好
if (value % 2 == 0)  // 除法

// 好
if ((value & 1) == 0)  // 位运算

// 优化4: 缓存友好
// 不好 (列优先访问)
for (int j = 0; j < cols; j++)
    for (int i = 0; i < rows; i++)
        matrix[i][j] = 0;

// 好 (行优先访问)
for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
        matrix[i][j] = 0;

// 优化5: 编译器提示
// __restrict 表示指针不重叠
void copy(int *__restrict dst, const int *__restrict src, int n) {
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}
```

---

## 第六部分：项目实战 (36:00 - 39:00)

### 36:00 | 项目概述 | 画面：环境监测系统的架构

**讲解词**:
让我们看一个完整项目——基于STM32的环境监测系统。采集温度、湿度、气压，通过LoRa无线传输，使用FreeRTOS管理任务。

**系统架构**:

```
环境监测系统
├── 硬件
│   ├── STM32L4 (低功耗MCU)
│   ├── BME280 (温湿度气压传感器, I2C)
│   ├── SX1262 (LoRa模块, SPI)
│   └── OLED显示屏 (I2C)
│
├── 软件架构
│   ├── FreeRTOS
│   ├── 驱动层 (GPIO, I2C, SPI, UART, RTC)
│   ├── 中间件 (传感器驱动, LoRaWAN)
│   └── 应用层
│       ├── 采集任务 (100ms)
│       ├── 显示任务 (500ms)
│       ├── 传输任务 (60s)
│       └── 电源管理任务 (1s)
│
└── 电源管理
    ├── 运行模式 (全速)
    ├── 睡眠模式 (空闲时)
    └── 停止模式 (传输间隔)
```

### 36:50 | 核心代码 | 画面：关键模块的代码展示

**讲解词**:
这是系统的核心代码，展示了如何组织嵌入式项目。

**系统代码**:

```c
// main.c - 环境监测系统

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// 传感器数据
typedef struct {
    float temperature;
    float humidity;
    float pressure;
    uint32_t timestamp;
} SensorData;

// 全局资源
QueueHandle_t sensor_queue;
QueueHandle_t display_queue;
SemaphoreHandle_t i2c_mutex;

// 任务句柄
TaskHandle_t sensor_task_handle;
TaskHandle_t lora_task_handle;

// 传感器采集任务
void vSensorTask(void *pvParameters) {
    SensorData data;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // 初始化传感器
    BME280_Init();

    while (1) {
        // 读取传感器
        xSemaphoreTake(i2c_mutex, portMAX_DELAY);
        data.temperature = BME280_ReadTemperature();
        data.humidity = BME280_ReadHumidity();
        data.pressure = BME280_ReadPressure();
        xSemaphoreGive(i2c_mutex);

        data.timestamp = RTC_GetTimestamp();

        // 发送到队列
        xQueueSend(sensor_queue, &data, 0);

        // 100ms周期
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

// LoRa传输任务
void vLoRaTask(void *pvParameters) {
    SensorData data;
    uint8_t payload[16];

    // 初始化LoRa
    SX1262_Init();
    LoRaWAN_Join();

    while (1) {
        // 聚合数据 (60秒平均值)
        float temp_sum = 0, hum_sum = 0, press_sum = 0;
        int count = 0;
        uint32_t start_time = RTC_GetTimestamp();

        while (RTC_GetTimestamp() - start_time < 60) {
            if (xQueueReceive(sensor_queue, &data, pdMS_TO_TICKS(1000))) {
                temp_sum += data.temperature;
                hum_sum += data.humidity;
                press_sum += data.pressure;
                count++;
            }
        }

        if (count > 0) {
            // 打包数据
            int16_t temp = (int16_t)(temp_sum / count * 100);
            uint16_t hum = (uint16_t)(hum_sum / count * 100);
            uint16_t press = (uint16_t)((press_sum / count - 90000) / 10);

            payload[0] = temp >> 8;
            payload[1] = temp & 0xFF;
            payload[2] = hum >> 8;
            payload[3] = hum & 0xFF;
            payload[4] = press >> 8;
            payload[5] = press & 0xFF;

            // 发送
            LoRaWAN_Send(payload, 6);
        }

        // 进入低功耗
        enter_low_power();
    }
}

// 显示任务
void vDisplayTask(void *pvParameters) {
    SensorData data;
    char line[32];

    OLED_Init();

    while (1) {
        // 获取最新数据 (非阻塞)
        if (xQueuePeek(display_queue, &data, 0)) {
            // 更新显示
            OLED_Clear();

            snprintf(line, sizeof(line), "Temp: %.1f C", data.temperature);
            OLED_DrawString(0, 0, line);

            snprintf(line, sizeof(line), "Hum:  %.1f %%", data.humidity);
            OLED_DrawString(0, 16, line);

            snprintf(line, sizeof(line), "Pres: %.0f hPa", data.pressure / 100);
            OLED_DrawString(0, 32, line);

            OLED_Update();
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void) {
    // 硬件初始化
    HAL_Init();
    SystemClock_Config();
    GPIO_Init();
    I2C_Init();
    SPI_Init();
    RTC_Init();

    // 创建同步原语
    i2c_mutex = xSemaphoreCreateMutex();
    sensor_queue = xQueueCreate(10, sizeof(SensorData));
    display_queue = xQueueCreate(1, sizeof(SensorData));

    // 创建任务
    xTaskCreate(vSensorTask, "Sensor", 256, NULL, 3, &sensor_task_handle);
    xTaskCreate(vLoRaTask, "LoRa", 512, NULL, 2, &lora_task_handle);
    xTaskCreate(vDisplayTask, "Display", 256, NULL, 1, NULL);

    // 启动
    vTaskStartScheduler();

    while (1);
}
```

### 38:10 | 电源管理 | 画面：功耗分析和优化

**讲解词**:
电池供电设备需要精心管理电源。使用STM32L4的低功耗模式，可以将平均电流降至几十微安。

**功耗优化**:

```
功耗分析:
├── 运行模式:  ~10 mA
├── 睡眠模式:  ~1 mA
├── 停止模式:  ~10 uA
└── 待机模式:  ~2 uA

优化策略:
1. 空闲时进入停止模式
2. RTC定时唤醒
3. 传感器按需供电
4. 外设时钟门控
5. 降低CPU频率

实测结果:
- 采集周期: 100ms
- 传输周期: 60s
- 平均电流: ~50 uA
- 电池寿命: >1年 (2000mAh)
```

---

## 总结 (39:00 - 40:00)

### 39:00 | 核心内容回顾 | 画面：知识体系的思维导图

**讲解词**:
让我们回顾今天的内容：嵌入式C基础、内存映射I/O、中断处理、外设驱动开发、RTOS集成、调试技术、项目实战。

**知识总结**:

```
嵌入式C编程
├── 基础
│   ├── 工具链
│   ├── 启动代码
│   ├── 链接器脚本
│   └── 内存映射I/O
├── 中断
│   ├── 向量表
│   ├── ISR编写
│   ├── 优先级
│   └── Fault处理
├── 驱动
│   ├── GPIO
│   ├── UART
│   ├── ADC
│   └── Timer
├── RTOS
│   ├── FreeRTOS
│   ├── 任务管理
│   ├── 同步
│   └── 低功耗
├── 调试
│   ├── GDB/OpenOCD
│   ├── ITM/SWO
│   ├── DWT
│   └── 性能分析
└── 优化
    ├── 代码优化
    ├── 内存优化
    └── 功耗优化
```

### 39:40 | 资源推荐 | 画面：学习资源和工具

**讲解词**:
推荐以下资源：

- ARM Cortex-M参考手册
- FreeRTOS文档
- Joseph Yiu的《Cortex-M权威指南》
- 本视频项目代码

### 39:55 | 结束 | 画面：下一集预告

**讲解词**:
感谢观看！下一集我们将学习C语言性能优化技巧。敬请期待！

---

## 技术备注

### 开发板推荐

- STM32F4/F7 Discovery (学习)
- STM32L4 Nucleo (低功耗)
- ESP32 (WiFi/蓝牙)

### 参考文档

- [ARM Cortex-M4 TRM](https://developer.arm.com/)
- [STM32 Reference Manual](https://www.st.com/)
- [FreeRTOS Documentation](https://www.freertos.org/)

---

*脚本版本: 1.0*
*最后更新: 2026-03-19*
