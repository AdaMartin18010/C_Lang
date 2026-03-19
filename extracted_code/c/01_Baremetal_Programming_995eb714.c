/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 4202
 * Language: c
 * Block ID: 995eb714
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file fault_handler.c
 * @brief 硬故障诊断程序
 */

#include <stdint.h>

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
} HardFault_StackFrame;

/* SCB寄存器 */
#define SCB_HFSR    (*(volatile uint32_t *)0xE000ED2C)
#define SCB_CFSR    (*(volatile uint32_t *)0xE000ED28)
#define SCB_BFAR    (*(volatile uint32_t *)0xE000ED38)
#define SCB_MMAR    (*(volatile uint32_t *)0xE000ED34)

/* CFSR位定义 */
#define CFSR_IACCVIOL   (1 << 0)   /* 取指访问冲突 */
#define CFSR_DACCVIOL   (1 << 1)   /* 数据访问冲突 */
#define CFSR_MUNSTKERR  (1 << 3)   /* 出栈错误 */
#define CFSR_MSTKERR    (1 << 4)   /* 入栈错误 */
#define CFSR_IBUSERR    (1 << 8)   /* 取指总线错误 */
#define CFSR_PRECISERR  (1 << 9)   /* 精确数据总线错误 */
#define CFSR_IMPRECISERR (1 << 10) /* 非精确数据总线错误 */
#define CFSR_UNSTKERR   (1 << 11)  /* 出栈总线错误 */
#define CFSR_STKERR     (1 << 12)  /* 入栈总线错误 */
#define CFSR_UNDEFINSTR (1 << 16)  /* 未定义指令 */
#define CFSR_INVSTATE   (1 << 17)  /* 无效状态 */
#define CFSR_INVPC      (1 << 18)  /* 无效PC */
#define CFSR_NOCP       (1 << 19)  /* 无协处理器 */
#define CFSR_UNALIGNED  (1 << 24)  /* 非对齐访问 */
#define CFSR_DIVBYZERO  (1 << 25)  /* 除零错误 */

/* 外部定义的UART发送函数 */
extern void UART_SendChar(char c);
extern void UART_SendString(const char *str);

static void send_hex(uint32_t val)
{
    const char hex[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        UART_SendChar(hex[(val >> (i * 4)) & 0xF]);
    }
}

void HardFault_Handler_C(HardFault_StackFrame *stackFrame)
{
    UART_SendString("\r\n*** HARDFAULT ***\r\n");

    UART_SendString("R0:  0x"); send_hex(stackFrame->r0);  UART_SendString("\r\n");
    UART_SendString("R1:  0x"); send_hex(stackFrame->r1);  UART_SendString("\r\n");
    UART_SendString("R2:  0x"); send_hex(stackFrame->r2);  UART_SendString("\r\n");
    UART_SendString("R3:  0x"); send_hex(stackFrame->r3);  UART_SendString("\r\n");
    UART_SendString("R12: 0x"); send_hex(stackFrame->r12); UART_SendString("\r\n");
    UART_SendString("LR:  0x"); send_hex(stackFrame->lr);  UART_SendString("\r\n");
    UART_SendString("PC:  0x"); send_hex(stackFrame->pc);  UART_SendString("\r\n");
    UART_SendString("PSR: 0x"); send_hex(stackFrame->psr); UART_SendString("\r\n");

    UART_SendString("HFSR: 0x"); send_hex(SCB_HFSR); UART_SendString("\r\n");
    UART_SendString("CFSR: 0x"); send_hex(SCB_CFSR); UART_SendString("\r\n");

    if (SCB_CFSR & CFSR_BFARVALID) {
        UART_SendString("BFAR: 0x"); send_hex(SCB_BFAR); UART_SendString("\r\n");
    }
    if (SCB_CFSR & CFSR_MMARVALID) {
        UART_SendString("MMAR: 0x"); send_hex(SCB_MMAR); UART_SendString("\r\n");
    }

    /* 诊断错误类型 */
    UART_SendString("Fault: ");
    if (SCB_CFSR & CFSR_IACCVIOL) UART_SendString("Instruction access violation\r\n");
    if (SCB_CFSR & CFSR_DACCVIOL) UART_SendString("Data access violation\r\n");
    if (SCB_CFSR & CFSR_IBUSERR)  UART_SendString("Instruction bus error\r\n");
    if (SCB_CFSR & CFSR_UNDEFINSTR) UART_SendString("Undefined instruction\r\n");
    if (SCB_CFSR & CFSR_INVSTATE) UART_SendString("Invalid state\r\n");
    if (SCB_CFSR & CFSR_INVPC)    UART_SendString("Invalid PC\r\n");
    if (SCB_CFSR & CFSR_DIVBYZERO) UART_SendString("Divide by zero\r\n");

    /* 无限循环等待调试 */
    while (1) {
        __asm volatile ("nop");
    }
}

/* 汇编入口 */
__attribute__((naked))
void HardFault_Handler(void)
{
    __asm volatile (
        "tst    lr, #4          \n"  /* 检查EXC_RETURN位2 */
        "ite    eq              \n"
        "mrseq  r0, msp         \n"  /* 使用MSP */
        "mrsne  r0, psp         \n"  /* 使用PSP */
        "ldr    r1, =HardFault_Handler_C \n"
        "bx     r1              \n"
    );
}
