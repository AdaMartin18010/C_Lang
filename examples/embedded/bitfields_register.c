/*
 * 位域寄存器访问示例
 * 编译: gcc -Wall -Wextra -o bitfields_register bitfields_register.c
 */
#include <stdio.h>
#include <stdint.h>

/* 使用位域定义寄存器 */
typedef union {
    uint32_t raw;
    struct {
        uint32_t enable    : 1;
        uint32_t mode      : 3;
        uint32_t prescaler : 8;
        uint32_t reserved  : 20;
    } bits;
} TimerControl;

/* 宏确保位域布局 */
typedef union {
    uint32_t raw;
    struct {
        uint32_t tx_enable  : 1;
        uint32_t rx_enable  : 1;
        uint32_t baud_rate  : 4;
        uint32_t parity     : 2;
        uint32_t stop_bits  : 1;
        uint32_t reserved   : 23;
    } bits;
} UartConfig;

void print_timer(TimerControl *tc) {
    printf("TimerControl(raw=0x%08X): enable=%d mode=%d prescaler=%d\n",
           tc->raw, tc->bits.enable, tc->bits.mode, tc->bits.prescaler);
}

void print_uart(UartConfig *uc) {
    printf("UartConfig(raw=0x%08X): tx=%d rx=%d baud=%d parity=%d stop=%d\n",
           uc->raw, uc->bits.tx_enable, uc->bits.rx_enable,
           uc->bits.baud_rate, uc->bits.parity, uc->bits.stop_bits);
}

int main(void) {
    TimerControl tc = {0};
    tc.bits.enable = 1;
    tc.bits.mode = 5;
    tc.bits.prescaler = 128;
    print_timer(&tc);
    
    UartConfig uc = {0};
    uc.bits.tx_enable = 1;
    uc.bits.rx_enable = 1;
    uc.bits.baud_rate = 9;   /* 115200 */
    uc.bits.parity = 2;      /* even */
    uc.bits.stop_bits = 1;
    print_uart(&uc);
    
    /* 通过 raw 值初始化 */
    UartConfig uc2 = {.raw = 0x8000004B};
    print_uart(&uc2);
    
    return 0;
}
