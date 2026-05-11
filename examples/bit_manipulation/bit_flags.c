/*
 * 标志位枚举与操作示例
 * 编译: gcc -Wall -Wextra -o bit_flags bit_flags.c
 */
#include <stdio.h>
#include <stdint.h>

typedef enum {
    FLAG_READ   = 1 << 0,   /* 0x01 */
    FLAG_WRITE  = 1 << 1,   /* 0x02 */
    FLAG_EXEC   = 1 << 2,   /* 0x04 */
    FLAG_CREATE = 1 << 3,   /* 0x08 */
    FLAG_TRUNC  = 1 << 4,   /* 0x10 */
} FileFlags;

#define BIT_SET(x, n)     ((x) |= (1U << (n)))
#define BIT_CLEAR(x, n)   ((x) &= ~(1U << (n)))
#define BIT_TOGGLE(x, n)  ((x) ^= (1U << (n)))
#define BIT_CHECK(x, n)   (((x) >> (n)) & 1U)

void print_flags(uint32_t flags) {
    printf("Flags: 0x%02x [", flags);
    if (flags & FLAG_READ)   printf("R");
    if (flags & FLAG_WRITE)  printf("W");
    if (flags & FLAG_EXEC)   printf("X");
    if (flags & FLAG_CREATE) printf("C");
    if (flags & FLAG_TRUNC)  printf("T");
    printf("]\n");
}

int main(void) {
    uint32_t mode = FLAG_READ | FLAG_WRITE;
    print_flags(mode);  /* RW */
    
    BIT_SET(mode, 2);       /* 添加 EXEC */
    print_flags(mode);      /* RWX */
    
    BIT_CLEAR(mode, 1);     /* 移除 WRITE */
    print_flags(mode);      /* RX */
    
    if (mode & FLAG_READ) {
        printf("Read permission granted\n");
    }
    
    /* 同时检查多个标志 */
    if ((mode & (FLAG_READ | FLAG_EXEC)) == (FLAG_READ | FLAG_EXEC)) {
        printf("Both READ and EXEC granted\n");
    }
    
    return 0;
}
