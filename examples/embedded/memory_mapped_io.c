/*
 * 内存映射 I/O 模拟示例
 * 编译: gcc -Wall -Wextra -o memory_mapped_io memory_mapped_io.c
 */
#include <stdio.h>
#include <stdint.h>

/* 模拟一个简化的 GPIO 控制器寄存器组 */
#define GPIO_BASE   0x40000000
#define GPIO_CTRL   (GPIO_BASE + 0x00)
#define GPIO_DIR    (GPIO_BASE + 0x04)
#define GPIO_OUT    (GPIO_BASE + 0x08)
#define GPIO_IN     (GPIO_BASE + 0x0C)

/* 模拟内存空间 */
static uint32_t gpio_mem[16];

#define REG32(addr) (*(volatile uint32_t *)(addr))

/* 将模拟地址映射到数组 */
#define GPIO_ADDR(offset) ((volatile uint32_t *)((uint8_t *)gpio_mem + (offset) - GPIO_BASE))

#define GPIO_CTRL_REG  (*GPIO_ADDR(GPIO_CTRL))
#define GPIO_DIR_REG   (*GPIO_ADDR(GPIO_DIR))
#define GPIO_OUT_REG   (*GPIO_ADDR(GPIO_OUT))
#define GPIO_IN_REG    (*GPIO_ADDR(GPIO_IN))

/* 位操作宏 */
#define BIT(n) (1U << (n))
#define SET_BIT(reg, n)   ((reg) |= BIT(n))
#define CLEAR_BIT(reg, n) ((reg) &= ~BIT(n))
#define READ_BIT(reg, n)  (((reg) >> (n)) & 1U)

void gpio_init(void) {
    GPIO_CTRL_REG = 0;
    GPIO_DIR_REG = 0;
    GPIO_OUT_REG = 0;
}

void gpio_set_output(int pin) {
    SET_BIT(GPIO_DIR_REG, pin);
}

void gpio_set_input(int pin) {
    CLEAR_BIT(GPIO_DIR_REG, pin);
}

void gpio_write(int pin, int value) {
    if (value) {
        SET_BIT(GPIO_OUT_REG, pin);
    } else {
        CLEAR_BIT(GPIO_OUT_REG, pin);
    }
}

int gpio_read(int pin) {
    return READ_BIT(GPIO_IN_REG, pin);
}

int main(void) {
    gpio_init();
    
    /* 设置 pin 0,1,2 为输出 */
    gpio_set_output(0);
    gpio_set_output(1);
    gpio_set_output(2);
    
    gpio_write(0, 1);
    gpio_write(1, 0);
    gpio_write(2, 1);
    
    printf("DIR  = 0x%08X\n", GPIO_DIR_REG);
    printf("OUT  = 0x%08X\n", GPIO_OUT_REG);
    
    /* 模拟外部输入 */
    GPIO_IN_REG = 0x00000008;
    printf("IN   = 0x%08X, pin3=%d\n", GPIO_IN_REG, gpio_read(3));
    
    return 0;
}
