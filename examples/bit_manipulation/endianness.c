/*
 * 字节序检测与转换示例
 * 编译: gcc -Wall -Wextra -o endianness endianness.c
 */
#include <stdio.h>
#include <stdint.h>

/* 编译期检测 */
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define IS_LITTLE_ENDIAN 1
#else
    #define IS_LITTLE_ENDIAN 0
#endif

/* 运行期检测 */
int is_little_endian(void) {
    union { uint16_t val; uint8_t bytes[2]; } test;
    test.val = 0x0102;
    return test.bytes[0] == 0x02;
}

uint16_t bswap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

uint32_t bswap32(uint32_t x) {
    return ((x >> 24) & 0xFF)      |
           ((x >> 8)  & 0xFF00)    |
           ((x << 8)  & 0xFF0000)  |
           ((x << 24) & 0xFF000000);
}

uint64_t bswap64(uint64_t x) {
    return ((x >> 56) & 0xFFULL)       |
           ((x >> 40) & 0xFF00ULL)     |
           ((x >> 24) & 0xFF0000ULL)   |
           ((x >> 8)  & 0xFF000000ULL) |
           ((x << 8)  & 0xFF00000000ULL) |
           ((x << 24) & 0xFF0000000000ULL) |
           ((x << 40) & 0xFF000000000000ULL) |
           ((x << 56) & 0xFF00000000000000ULL);
}

int main(void) {
    printf("Compile-time little endian: %d\n", IS_LITTLE_ENDIAN);
    printf("Run-time little endian: %d\n", is_little_endian());
    
    uint32_t host = 0x12345678;
    uint32_t net = bswap32(host);
    printf("Host: 0x%08x -> Network: 0x%08x\n", host, net);
    printf("Back: 0x%08x\n", bswap32(net));
    
    return 0;
}
