/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 2488
 * Language: c
 * Block ID: 77c74875
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file cert_examples.c
 * @brief CERT C 合规示例
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== EXP33-C: 不要读取未初始化的内存 ========== */

/* ❌ 不合规 */
/* int sign; */
/* if (x < 0) sign = -1; */
/* else if (x > 0) sign = 1; */
/* return sign;  // x==0时sign未初始化 */

/* ✅ 合规 */
int get_sign(int32_t x)
{
    int32_t sign = 0;  /* 始终初始化 */

    if (x < 0) {
        sign = -1;
    } else if (x > 0) {
        sign = 1;
    }
    /* x == 0 时 sign = 0 */

    return (int)sign;
}

/* ========== EXP34-C: 不要解引用空指针 ========== */

/* ❌ 不合规 */
/* void process(char *str) { */
/*     str[0] = 'A';  // str可能为NULL */
/* } */

/* ✅ 合规 */
void process_string(char *str)
{
    if (str == NULL) {
        return;  /* 或错误处理 */
    }
    str[0] = 'A';
}

/* ========== FIO30-C: 排除用户输入格式化字符串 ========== */

/* ❌ 严重漏洞：格式化字符串攻击 */
/* void log_message(const char *user_input) { */
/*     printf(user_input);  // 危险！ */
/* } */
/* // 攻击者输入: "%s%s%s%s%s%s%s%s" 导致崩溃或信息泄露 */

/* ✅ 合规 */
void log_message_safe(const char *user_input)
{
    /* 方法1: 使用格式字符串 */
    printf("%s", user_input);

    /* 方法2: 使用fputs */
    fputs(user_input, stdout);
}

/* ========== MEM30-C: 不要访问已释放的内存 ========== */

/* ❌ 不合规：返回局部指针 */
/* char *get_error_message(int code) { */
/*     char msg[100]; */
/*     sprintf(msg, "Error code: %d", code); */
/*     return msg;  // 返回局部变量地址！ */
/* } */

/* ✅ 合规 */
static char g_error_buffer[256];  /* 静态缓冲区 */

const char *get_error_message(int32_t code)
{
    snprintf(g_error_buffer, sizeof(g_error_buffer),
             "Error code: %d", (int)code);
    return g_error_buffer;
}

/* ✅ 更好：调用者提供缓冲区 */
void get_error_message_safe(int32_t code,
                            char *buffer,
                            size_t buffer_size)
{
    if (buffer != NULL && buffer_size > 0) {
        snprintf(buffer, buffer_size, "Error code: %d", (int)code);
    }
}

/* ========== STR31-C: 确保字符串存储有足够的空间 ========== */

/* ❌ 不合规 */
/* char dest[10]; */
/* strcpy(dest, long_source_string);  // 缓冲区溢出 */

/* ✅ 合规 */
void safe_copy_example(void)
{
    char dest[16];
    const char *source = "Hello, World!";

    /* 方法1: 使用strncpy */
    strncpy(dest, source, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';  /* 确保终止 */

    /* 方法2: 使用strlcpy（如果可用）*/
    /* strlcpy(dest, source, sizeof(dest)); */
}

/* ========== ARR30-C: 不要形成或使用超出范围的指针 ========== */

/* ❌ 不合规 */
/* int arr[10]; */
/* int *end = &arr[10];  // 形成越界指针（虽然不下解引用）*/
/* for (int *p = arr; p <= end; p++) { // 访问arr[10]越界！ */
/*     *p = 0; */
/* } */

/* ✅ 合规 */
void array_init_example(void)
{
    int32_t arr[10];

    /* 方法1: 使用数组索引 */
    for (size_t i = 0; i < 10; i++) {
        arr[i] = 0;
    }

    /* 方法2: 正确计算结束位置 */
    for (int32_t *p = arr; p < arr + 10; p++) {
        *p = 0;
    }

    /* 方法3: 使用size_t避免负数问题 */
    /* for (size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) */
}

/* ========== MSC33-C: 不要依赖rand()的伪随机性 ========== */

#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

/* ❌ 不合规 */
/* srand(time(NULL)); */
/* int key = rand();  // 可预测的随机数 */

/* ✅ 合规：使用加密安全的随机数 */
int secure_random_bytes(void *buffer, size_t length)
{
#ifdef _WIN32
    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL,
                              PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        return -1;
    }
    BOOL result = CryptGenRandom(hCryptProv, (DWORD)length, (BYTE*)buffer);
    CryptReleaseContext(hCryptProv, 0);
    return result ? 0 : -1;
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) return -1;

    ssize_t total = 0;
    while ((size_t)total < length) {
        ssize_t n = read(fd, (char*)buffer + total, length - (size_t)total);
        if (n < 0) {
            close(fd);
            return -1;
        }
        total += n;
    }
    close(fd);
    return 0;
#endif
}
