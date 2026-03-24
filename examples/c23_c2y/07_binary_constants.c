/*
 * C23 二进制常量示例
 * 编译: gcc -std=c23 -o 07_binary_constants 07_binary_constants.c
 *       clang -std=c23 -o 07_binary_constants 07_binary_constants.c
 */

#include <stdio.h>
#include <stdint.h>

// 二进制常量使位操作代码更清晰
#define FLAG_READ   0b00000001
#define FLAG_WRITE  0b00000010
#define FLAG_EXEC   0b00000100
#define FLAG_HIDDEN 0b00001000
#define FLAG_SYSTEM 0b00010000

// 位掩码
#define MASK_LOW_NIBBLE  0b00001111
#define MASK_HIGH_NIBBLE 0b11110000

// 权限组合
#define PERM_RW   0b00000011
#define PERM_RWX  0b00000111

int main(void) {
    printf("C23 Binary Constants Demo\n\n");
    
    // 基本二进制常量
    printf("Binary constants:\n");
    printf("0b00000001 = %d\n", 0b00000001);
    printf("0b00001010 = %d (binary for 10)\n", 0b00001010);
    printf("0b11111111 = %d (binary for 255)\n", 0b11111111);
    
    // 使用数字分隔符增强可读性
    printf("\nBinary with digit separators:\n");
    printf("0b1111'0000 = %d\n", 0b1111'0000);
    printf("0b1010'1010 = %d\n", 0b1010'1010);
    printf("0b1111'1111'0000'0000 = %d\n", 0b1111'1111'0000'0000);
    
    // 位标志操作
    printf("\nBit flags:\n");
    uint8_t permissions = 0;
    
    // 设置权限
    permissions |= FLAG_READ;
    permissions |= FLAG_WRITE;
    printf("After setting READ and WRITE: 0b%08b = %d\n", permissions, permissions);
    
    // 检查权限
    if (permissions & FLAG_READ) {
        printf("READ permission is set\n");
    }
    
    // 使用预定义组合
    uint8_t full_access = PERM_RWX;
    printf("Full access (RWX): 0b%08b = %d\n", full_access, full_access);
    
    // 位掩码操作
    uint8_t data = 0b1010'1100;
    uint8_t low_nibble = data & MASK_LOW_NIBBLE;
    uint8_t high_nibble = (data & MASK_HIGH_NIBBLE) >> 4;
    
    printf("\nBit masking:\n");
    printf("data = 0b%08b = %d\n", data, data);
    printf("low_nibble = 0b%04b = %d\n", low_nibble, low_nibble);
    printf("high_nibble = 0b%04b = %d\n", high_nibble, high_nibble);
    
    // 不同进制对比
    printf("\nDifferent bases for the same value:\n");
    printf("Decimal: 170\n");
    printf("Hex: 0xAA\n");
    printf("Binary: 0b10101010\n");
    printf("Octal: 0252\n");
    printf("All represent the same value: %d\n", 0b10101010);
    
    // 硬件寄存器示例
    printf("\nHardware register example:\n");
    // 配置串口：8位数据，1位停止，无校验
    uint8_t uart_config = 0b000'11'0'00;  // 简化的寄存器配置
    printf("UART config: 0b%09b\n", uart_config);
    
    return 0;
}
