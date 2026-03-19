/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2413
 * Language: c
 * Block ID: c5abf599
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 精简printf实现 - 节省数十KB代码空间
 * ============================================================================ */

#ifndef MINI_PRINTF_H
#define MINI_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* 输出字符回调类型 */
typedef void (*putc_func_t)(char c);

/* 设置输出函数 */
void mini_printf_init(putc_func_t func);

/* 格式化输出 */
int mini_printf(const char *fmt, ...);
int mini_vprintf(const char *fmt, va_list args);
int mini_snprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* MINI_PRINTF_H */

/* ============================================================================
 * 实现
 * ============================================================================ */

static putc_func_t putc_func = NULL;
static char *snprintf_buf = NULL;
static size_t snprintf_size = 0;
static size_t snprintf_pos = 0;

void mini_printf_init(putc_func_t func)
{
    putc_func = func;
}

/* 输出单个字符 */
static void output_char(char c)
{
    if (snprintf_buf && snprintf_pos < snprintf_size - 1) {
        snprintf_buf[snprintf_pos++] = c;
    } else if (putc_func) {
        putc_func(c);
    }
}

/* 输出字符串 */
static void output_str(const char *s)
{
    while (*s) {
        output_char(*s++);
    }
}

/* 整数转字符串 (内部缓冲区) */
static char *itoa_internal(int32_t value, char *buf, int base, int *len)
{
    char *p = buf;
    char *q = buf;
    int is_negative = 0;

    /* 处理负数 */
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    /* 转换 */
    do {
        int digit = value % base;
        *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        value /= base;
    } while (value > 0);

    if (is_negative) {
        *p++ = '-';
    }

    *len = p - buf;

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }

    return buf;
}

/* 格式化整数输出 */
static void print_int(int32_t num, int base, int width, char pad)
{
    char buf[12];  /* 足够容纳32位整数 */
    int len;

    itoa_internal(num, buf, base, &len);

    /* 填充 */
    while (len < width) {
        output_char(pad);
        width--;
    }

    output_str(buf);
}

/* 无符号整数输出 */
static void print_uint(uint32_t num, int base, int width, char pad)
{
    char buf[12];
    char *p = buf;
    char *q = buf;

    if (num == 0) {
        *p++ = '0';
    } else {
        while (num > 0) {
            uint32_t digit = num % base;
            *p++ = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
            num /= base;
        }
    }

    int len = p - buf;

    /* 反转 */
    for (p--; q < p; q++, p--) {
        char t = *q;
        *q = *p;
        *p = t;
    }
    buf[len] = '\0';

    /* 填充 */
    while (len < width) {
        output_char(pad);
        width--;
    }

    output_str(buf);
}

/* 核心格式化函数 */
int mini_vprintf(const char *fmt, va_list args)
{
    char c;
    int count = 0;

    while ((c = *fmt++)) {
        if (c != '%') {
            output_char(c);
            count++;
            continue;
        }

        /* 解析格式 */
        char pad = ' ';
        int width = 0;
        int is_long = 0;

        c = *fmt++;

        /* 填充字符 */
        if (c == '0') {
            pad = '0';
            c = *fmt++;
        }

        /* 宽度 */
        while (c >= '0' && c <= '9') {
            width = width * 10 + (c - '0');
            c = *fmt++;
        }

        /* 长度修饰符 */
        if (c == 'l') {
            is_long = 1;
            c = *fmt++;
        }

        /* 格式字符 */
        switch (c) {
            case 'd':
            case 'i': {
                int32_t val = is_long ? va_arg(args, long) : va_arg(args, int);
                print_int(val, 10, width, pad);
                break;
            }

            case 'u': {
                uint32_t val = is_long ? va_arg(args, unsigned long)
                                       : va_arg(args, unsigned int);
                print_uint(val, 10, width, pad);
                break;
            }

            case 'x':
            case 'X': {
                uint32_t val = is_long ? va_arg(args, unsigned long)
                                       : va_arg(args, unsigned int);
                print_uint(val, 16, width, pad);
                break;
            }

            case 'c': {
                char val = (char)va_arg(args, int);
                output_char(val);
                break;
            }

            case 's': {
                const char *val = va_arg(args, const char *);
                output_str(val ? val : "(null)");
                break;
            }

            case 'p':
            case 'P': {
                void *val = va_arg(args, void *);
                output_str("0x");
                print_uint((uint32_t)val, 16, 8, '0');
                break;
            }

            case '%':
                output_char('%');
                break;

            default:
                output_char('%');
                output_char(c);
                break;
        }
    }

    /* 终止snprintf缓冲区 */
    if (snprintf_buf && snprintf_pos < snprintf_size) {
        snprintf_buf[snprintf_pos] = '\0';
    }

    return count;
}

/* 可变参数包装 */
int mini_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = mini_vprintf(fmt, args);
    va_end(args);
    return ret;
}

/* snprintf实现 */
int mini_snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;

    snprintf_buf = buf;
    snprintf_size = size;
    snprintf_pos = 0;

    va_start(args, fmt);
    int ret = mini_vprintf(fmt, args);
    va_end(args);

    snprintf_buf = NULL;
    return ret;
}
