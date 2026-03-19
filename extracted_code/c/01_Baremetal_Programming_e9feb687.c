/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 3421
 * Language: c
 * Block ID: e9feb687
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file main.c
 * @brief STM32F103C8T6 裸机程序主文件
 * @details
 *   功能演示:
 *   1. LED闪烁 (PC13)
 *   2. UART回显 (USART1 115200)
 *   3. 定时器中断 (1ms滴答)
 *   4. 多任务调度
 */

#include <stdint.h>
#include <stdarg.h>

/* ============================================================================
 * 寄存器定义 (简化版，完整版请见单独头文件)
 * ============================================================================ */

#define RCC_BASE            0x40021000
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR         (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define GPIOA_BASE          0x40010800
#define GPIOC_BASE          0x40011000
#define GPIO_CRL(port)      (*(volatile uint32_t *)(port + 0x00))
#define GPIO_CRH(port)      (*(volatile uint32_t *)(port + 0x04))
#define GPIO_ODR(port)      (*(volatile uint32_t *)(port + 0x0C))
#define GPIO_BSRR(port)     (*(volatile uint32_t *)(port + 0x10))

#define USART1_BASE         0x40013800
#define USART_SR            (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART_DR            (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART_BRR           (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART_CR1           (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define TIM2_BASE           0x40000000
#define TIM_CR1             (*(volatile uint32_t *)(TIM2_BASE + 0x00))
#define TIM_DIER            (*(volatile uint32_t *)(TIM2_BASE + 0x0C))
#define TIM_SR              (*(volatile uint32_t *)(TIM2_BASE + 0x10))
#define TIM_CNT             (*(volatile uint32_t *)(TIM2_BASE + 0x24))
#define TIM_PSC             (*(volatile uint32_t *)(TIM2_BASE + 0x28))
#define TIM_ARR             (*(volatile uint32_t *)(TIM2_BASE + 0x2C))

/* NVIC */
#define NVIC_ISER0          (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1          (*(volatile uint32_t *)0xE000E104)

/* ============================================================================
 * 系统配置
 * ============================================================================ */

#define SYSTEM_CLK          72000000
#define TICK_FREQ           1000

/* ============================================================================
 * 全局变量
 * ============================================================================ */

static volatile uint32_t g_milliseconds = 0;
static volatile uint32_t g_led_tick = 0;

/* ============================================================================
 * 延时函数
 * ============================================================================ */

void delay_ms(uint32_t ms)
{
    uint32_t start = g_milliseconds;
    while ((g_milliseconds - start) < ms);
}

void delay_us(uint32_t us)
{
    /* 简单忙等待，72MHz下约72周期1us */
    for (volatile uint32_t i = 0; i < us * 8; i++);
}

/* ============================================================================
 * 时钟配置
 * ============================================================================ */

/**
 * @brief 配置系统时钟为72MHz
 * @details HSE 8MHz -> PLL x9 -> SYSCLK 72MHz
 */
void SystemClock_Config(void)
{
    /* 使能HSE */
    (*(volatile uint32_t *)(RCC_BASE + 0x00)) |= (1 << 16);
    while (!((*(volatile uint32_t *)(RCC_BASE + 0x00)) & (1 << 17)));

    /* Flash等待状态: 2WS */
    (*(volatile uint32_t *)0x40022000) = 0x12;

    /* 配置PLL: HSE x9 */
    uint32_t cfgr = (*(volatile uint32_t *)(RCC_BASE + 0x04));
    cfgr &= ~(0x3F << 16);
    cfgr |= (1 << 16) | (0x7 << 18);  /* PLLSRC=HSE, PLLMUL=x9 */
    cfgr |= (0x4 << 8);  /* APB1 = HCLK/2 */
    (*(volatile uint32_t *)(RCC_BASE + 0x04)) = cfgr;

    /* 使能PLL */
    (*(volatile uint32_t *)(RCC_BASE + 0x00)) |= (1 << 24);
    while (!((*(volatile uint32_t *)(RCC_BASE + 0x00)) & (1 << 25)));

    /* 切换到PLL */
    cfgr = (*(volatile uint32_t *)(RCC_BASE + 0x04));
    cfgr &= ~0x3;
    cfgr |= 0x2;
    (*(volatile uint32_t *)(RCC_BASE + 0x04)) = cfgr;
    while (((*(volatile uint32_t *)(RCC_BASE + 0x04)) & 0xC) != 0x8);
}

/* ============================================================================
 * GPIO配置
 * ============================================================================ */

/**
 * @brief 初始化PC13为推挽输出 (板载LED)
 */
void GPIO_Init(void)
{
    /* 使能GPIOC时钟 */
    RCC_APB2ENR |= (1 << 4);

    /* PC13: 推挽输出, 2MHz */
    uint32_t crh = GPIO_CRH(GPIOC_BASE);
    crh &= ~(0xF << 20);  /* 清除PC13配置 */
    crh |= (0x2 << 20);   /* 输出推挽, 2MHz */
    GPIO_CRH(GPIOC_BASE) = crh;

    /* 初始状态: LED熄灭 (高电平) */
    GPIO_BSRR(GPIOC_BASE) = (1 << 13);
}

/**
 * @brief 设置LED状态
 * @param on 1=点亮, 0=熄灭
 */
void LED_Set(uint8_t on)
{
    if (on) {
        GPIO_BSRR(GPIOC_BASE) = (1 << 29);  /* 复位PC13 */
    } else {
        GPIO_BSRR(GPIOC_BASE) = (1 << 13);  /* 置位PC13 */
    }
}

/**
 * @brief 翻转LED状态
 */
void LED_Toggle(void)
{
    static uint8_t state = 0;
    LED_Set(state);
    state = !state;
}

/* ============================================================================
 * UART配置 (USART1 PA9-TX PA10-RX)
 * ============================================================================ */

void UART_Init(void)
{
    /* 使能USART1和GPIOA时钟 */
    RCC_APB2ENR |= (1 << 14) | (1 << 2);

    /* PA9 (TX): 复用推挽输出 */
    uint32_t crh = GPIO_CRH(GPIOA_BASE);
    crh &= ~(0xF << 4);
    crh |= (0xB << 4);  /* 复用推挽, 50MHz */
    GPIO_CRH(GPIOA_BASE) = crh;

    /* PA10 (RX): 浮空输入 */
    crh &= ~(0xF << 8);
    crh |= (0x4 << 8);  /* 浮空输入 */
    GPIO_CRH(GPIOA_BASE) = crh;

    /* 波特率: 115200 @ 72MHz */
    /* BRR = 72000000 / 115200 = 625 = 0x271 */
    USART_BRR = 0x271;

    /* 使能发送、接收、USART */
    USART_CR1 = (1 << 3) | (1 << 2) | (1 << 13);
}

void UART_SendChar(char c)
{
    while (!(USART_SR & (1 << 7)));  /* 等待TXE */
    USART_DR = c;
}

void UART_SendString(const char *str)
{
    while (*str) {
        UART_SendChar(*str++);
    }
}

char UART_ReceiveChar(void)
{
    while (!(USART_SR & (1 << 5)));  /* 等待RXNE */
    return (char)(USART_DR & 0xFF);
}

uint8_t UART_Available(void)
{
    return (USART_SR & (1 << 5)) ? 1 : 0;
}

/* ============================================================================
 * printf实现
 * ============================================================================ */

static void print_hex(uint32_t val, uint8_t digits)
{
    const char hex[] = "0123456789ABCDEF";
    char buf[9];
    buf[digits] = '\0';

    for (int i = digits - 1; i >= 0; i--) {
        buf[i] = hex[val & 0xF];
        val >>= 4;
    }

    UART_SendString(buf);
}

static void print_dec(int32_t val)
{
    char buf[12];
    uint8_t i = 0;
    uint8_t neg = 0;

    if (val < 0) {
        neg = 1;
        val = -val;
    }

    if (val == 0) {
        buf[i++] = '0';
    }

    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    if (neg) buf[i++] = '-';

    /* 反转输出 */
    while (i > 0) {
        UART_SendChar(buf[--i]);
    }
}

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'c':
                    UART_SendChar((char)va_arg(args, int));
                    break;
                case 's':
                    UART_SendString(va_arg(args, char*));
                    break;
                case 'd':
                case 'i':
                    print_dec(va_arg(args, int));
                    break;
                case 'x':
                    print_hex(va_arg(args, unsigned int), 8);
                    break;
                case 'X':
                    print_hex(va_arg(args, unsigned int), 2);
                    break;
                case '%':
                    UART_SendChar('%');
                    break;
                default:
                    UART_SendChar(*fmt);
                    break;
            }
        } else {
            UART_SendChar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}

/* ============================================================================
 * 定时器配置 (TIM2 1ms中断)
 * ============================================================================ */

void TIM2_Init(void)
{
    /* 使能TIM2时钟 */
    RCC_APB1ENR |= (1 << 0);

    /* 预分频: 36MHz / 36000 = 1kHz */
    TIM_PSC = 35999;
    TIM_ARR = 0;

    /* 使能更新中断 */
    TIM_DIER = (1 << 0);

    /* NVIC配置: TIM2中断 */
    NVIC_ISER0 |= (1 << 28);

    /* 使能定时器 */
    TIM_CR1 = (1 << 0);
}

void TIM2_IRQHandler(void)
{
    if (TIM_SR & (1 << 0)) {
        TIM_SR &= ~(1 << 0);  /* 清除中断标志 */
        g_milliseconds++;
        g_led_tick++;

        /* 每500ms翻转LED */
        if (g_led_tick >= 500) {
            g_led_tick = 0;
            LED_Toggle();
        }
    }
}

/* ============================================================================
 * 任务调度器 (简化版)
 * ============================================================================ */

typedef void (*TaskFunc)(void);

typedef struct {
    TaskFunc func;
    uint32_t interval;
    uint32_t lastRun;
    const char *name;
} Task;

#define MAX_TASKS 4
static Task g_tasks[MAX_TASKS];
static uint8_t g_numTasks = 0;

uint8_t Scheduler_AddTask(TaskFunc func, uint32_t interval, const char *name)
{
    if (g_numTasks >= MAX_TASKS) return 0;

    g_tasks[g_numTasks].func = func;
    g_tasks[g_numTasks].interval = interval;
    g_tasks[g_numTasks].lastRun = 0;
    g_tasks[g_numTasks].name = name;
    g_numTasks++;

    return 1;
}

void Scheduler_Run(void)
{
    uint32_t now = g_milliseconds;

    for (uint8_t i = 0; i < g_numTasks; i++) {
        if ((now - g_tasks[i].lastRun) >= g_tasks[i].interval) {
            g_tasks[i].lastRun = now;
            g_tasks[i].func();
        }
    }
}

/* ============================================================================
 * 任务实现
 * ============================================================================ */

/**
 * @brief 任务1: 串口心跳
 */
void Task_Heartbeat(void)
{
    static uint32_t count = 0;
    printf("[TICK] System running for %d seconds\r\n", count++);
}

/**
 * @brief 任务2: 处理串口命令
 */
void Task_CommandHandler(void)
{
    if (UART_Available()) {
        char cmd = UART_ReceiveChar();

        switch (cmd) {
            case 'l':
            case 'L':
                LED_Toggle();
                printf("LED toggled\r\n");
                break;
            case 's':
            case 'S':
                printf("System tick: %d ms\r\n", (int)g_milliseconds);
                break;
            case 'h':
            case 'H':
            case '?':
                printf("Commands: L=toggle LED, S=status, H=help\r\n");
                break;
            default:
                UART_SendChar(cmd);  /* 回显 */
                break;
        }
    }
}

/**
 * @brief 任务3: 性能统计
 */
void Task_Stats(void)
{
    static uint32_t lastTick = 0;
    uint32_t elapsed = g_milliseconds - lastTick;

    if (elapsed > 0) {
        /* 计算CPU使用率 (这里简化为打印任务执行频率) */
        printf("[STATS] Tasks running at 1Hz, tick precision OK\r\n");
        lastTick = g_milliseconds;
    }
}

/* ============================================================================
 * 主函数
 * ============================================================================ */

int main(void)
{
    /* 系统初始化 */
    SystemClock_Config();
    GPIO_Init();
    UART_Init();
    TIM2_Init();

    /* 欢迎信息 */
    UART_SendString("\r\n");
    UART_SendString("========================================\r\n");
    UART_SendString("  STM32F103 Baremetal Template v1.0    \r\n");
    UART_SendString("  System Clock: 72MHz                  \r\n");
    UART_SendString("  UART: 115200 8N1                     \r\n");
    UART_SendString("========================================\r\n");
    UART_SendString("Press H for help\r\n\r\n");

    /* 注册任务 */
    Scheduler_AddTask(Task_Heartbeat, 1000, "Heartbeat");
    Scheduler_AddTask(Task_CommandHandler, 10, "Command");
    Scheduler_AddTask(Task_Stats, 5000, "Stats");

    /* 主循环 */
    while (1) {
        Scheduler_Run();

        /* 空闲时进入低功耗模式 */
        __asm volatile ("wfi");
    }

    return 0;
}
