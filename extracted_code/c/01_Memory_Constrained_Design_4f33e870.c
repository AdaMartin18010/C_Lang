/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1672
 * Language: c
 * Block ID: 4f33e870
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 迷你字符串操作
 * ============================================================================ */

#include <stddef.h>

/* 自定义strlen - 小型化 */
size_t mini_strlen(const char *s)
{
    const char *p = s;
    while (*p) p++;
    return p - s;
}

/* 自定义strcpy */
char *mini_strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

/* 自定义memcpy - 对齐优化 */
void *mini_memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;

    /* 尝试4字节对齐复制 */
    if (!((uintptr_t)d & 3) && !((uintptr_t)s & 3)) {
        while (n >= 4) {
            *(uint32_t *)d = *(const uint32_t *)s;
            d += 4;
            s += 4;
            n -= 4;
        }
    }

    /* 剩余字节 */
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

/* 自定义memset */
void *mini_memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    uint8_t val = c;

    /* 4字节对齐清零 */
    if (!((uintptr_t)p & 3) && n >= 4 && val == 0) {
        while (n >= 4) {
            *(uint32_t *)p = 0;
            p += 4;
            n -= 4;
        }
    }

    while (n--) {
        *p++ = val;
    }

    return s;
}

/* 自定义memcmp */
int mini_memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;

    while (n--) {
        int diff = *p1++ - *p2++;
        if (diff) return diff;
    }
    return 0;
}

/* 快速整数转字符串 */
char *mini_itoa(int value, char *buf, int base)
{
    char *p = buf;
    char *q = buf;

    /* 处理负数 */
    if (value < 0 && base == 10) {
        *p++ = '-';
        value = -value;
    }

    /* 转换 */
    do {
        int digit = value % base;
        *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    } while (value);

    *p = '\0';

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }

    return buf;
}

/* 快速字符串转整数 */
int mini_atoi(const char *s)
{
    int sign = 1;
    int result = 0;

    /* 跳过空白 */
    while (*s == ' ' || *s == '\t') s++;

    /* 符号 */
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    /* 转换 */
    while (*s >= '0' && *s <= '9') {
        result = result * 10 + (*s - '0');
        s++;
    }

    return sign * result;
}
