/*
 * CVE 风格漏洞审计案例
 * 包含 3 个经典漏洞模式：缓冲区溢出、Use-After-Free、整数溢出
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -D_FORTIFY_SOURCE=2 -o cve_cases cve_cases.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ================================================================
 * CASE 1: HeartBleed 风格缓冲区过度读取
 * CVE-2014-0160 原理简化版
 * ================================================================ */

typedef struct {
    uint8_t type;
    uint16_t payload_len;
    uint8_t data[64];
} HeartbeatMsg;

/* ❌ 漏洞：未验证 payload_len，信任客户端输入 */
void heartbeat_vulnerable(const HeartbeatMsg *req, HeartbeatMsg *resp) {
    resp->type = 2; /* response */
    resp->payload_len = req->payload_len;
    /* 危险：req->payload_len 可能大于 resp->data 容量 */
    memcpy(resp->data, req->data, req->payload_len);
}

/* ✅ 修复：严格验证长度 */
void heartbeat_fixed(const HeartbeatMsg *req, HeartbeatMsg *resp) {
    resp->type = 2;
    uint16_t copy_len = req->payload_len;
    if (copy_len > sizeof(resp->data)) {
        copy_len = sizeof(resp->data);
    }
    resp->payload_len = copy_len;
    memcpy(resp->data, req->data, copy_len);
}

void demo_heartbeat(void) {
    printf("=== CASE 1: HeartBleed-style Buffer Over-read ===\n");
    HeartbeatMsg req = {0};
    req.type = 1;
    req.payload_len = 1000; /* 恶意：请求 1000 字节，但 data 只有 64 */
    strcpy((char *)req.data, "secret_key=abc123");

    HeartbeatMsg resp = {0};
    printf("Vulnerable: copying %d bytes from 64-byte buffer\n", req.payload_len);
    /* 不运行漏洞版本，避免崩溃 */
    /* heartbeat_vulnerable(&req, &resp); */

    heartbeat_fixed(&req, &resp);
    printf("Fixed: only copied %d bytes (clamped to buffer size)\n", resp.payload_len);
    printf("\n");
}

/* ================================================================
 * CASE 2: Use-After-Free（双重释放 / 悬空指针）
 * 类似 CVE-2019-1010023 (glibc) 模式
 * ================================================================ */

typedef struct {
    char *data;
    size_t len;
} String;

/* ❌ 漏洞：释放后继续使用指针 */
void string_duplicate_vulnerable(String *dst, const String *src) {
    free(dst->data);
    dst->data = malloc(src->len);
    if (!dst->data) return; /* 分配失败，但 dst->data 已是悬空指针！ */
    memcpy(dst->data, src->data, src->len);
    dst->len = src->len;
}

/* ✅ 修复：先分配再释放，失败时保持原状态 */
void string_duplicate_fixed(String *dst, const String *src) {
    char *new_data = malloc(src->len);
    if (!new_data) return; /* 原数据保持不变 */
    memcpy(new_data, src->data, src->len);
    free(dst->data);
    dst->data = new_data;
    dst->len = src->len;
}

/* ❌ 漏洞：函数返回局部对象地址 */
String *string_create_vulnerable(const char *text) {
    String local = {strdup(text), strlen(text)};
    return &local; /* 返回栈地址 */
}

/* ✅ 修复：堆分配 */
String *string_create_fixed(const char *text) {
    String *s = malloc(sizeof(String));
    if (!s) return NULL;
    s->data = strdup(text);
    s->len = strlen(text);
    return s;
}

void demo_uaf(void) {
    printf("=== CASE 2: Use-After-Free ===\n");

    String a = {strdup("original"), 8};
    String b = {strdup("new_data_longer"), 15};

    printf("Before duplicate: a='%s' (%zu)\n", a.data, a.len);
    string_duplicate_fixed(&a, &b);
    printf("After duplicate:  a='%s' (%zu)\n", a.data, a.len);

    free(a.data);
    free(b.data);

    /* 演示返回局部地址的危险 */
    String *bad = string_create_fixed("safe");
    printf("Safe create: '%s'\n", bad->data);
    free(bad->data);
    free(bad);
    printf("\n");
}

/* ================================================================
 * CASE 3: 整数溢出导致缓冲区溢出
 * 类似 CVE-2018-1000001 (glibc getcwd) 模式
 * ================================================================ */

/* ❌ 漏洞：整数溢出导致分配过小缓冲区 */
void *allocate_buffer_vulnerable(uint32_t width, uint32_t height, size_t elem_size) {
    uint32_t total = width * height; /* 可能溢出！ */
    return malloc(total * elem_size);
}

/* ✅ 修复：使用宽类型并检查溢出 */
void *allocate_buffer_fixed(uint32_t width, uint32_t height, size_t elem_size) {
    if (width == 0 || height == 0 || elem_size == 0) return NULL;

    /* 用 128 位或检查溢出 */
    uint64_t total = (uint64_t)width * (uint64_t)height;
    if (total > SIZE_MAX / elem_size) return NULL; /* 乘法溢出 */

    size_t bytes = (size_t)(total * elem_size);
    void *p = malloc(bytes);
    if (p) memset(p, 0, bytes);
    return p;
}

/* ❌ 漏洞：有符号/无符号混合导致条件绕过 */
int check_bounds_vulnerable(int index, size_t count) {
    /* index 为负数时，提升为无符号后变成极大正数 */
    if ((size_t)index < count) {
        return 1; /* 允许访问 */
    }
    return 0;
}

/* ✅ 修复：先检查负数 */
int check_bounds_fixed(int index, size_t count) {
    if (index < 0 || (size_t)index >= count) {
        return 0;
    }
    return 1;
}

void demo_integer_overflow(void) {
    printf("=== CASE 3: Integer Overflow ===\n");

    /* 演示整数溢出 */
    uint32_t w = 0x10000; /* 65536 */
    uint32_t h = 0x10000; /* 65536 */
    void *p = allocate_buffer_vulnerable(w, h, 4);
    printf("Vulnerable alloc: %p (requested %u x %u x 4, but overflowed to small size)\n",
           p, w, h);
    free(p);

    p = allocate_buffer_fixed(w, h, 4);
    printf("Fixed alloc: %s\n", p ? "SUCCESS" : "REJECTED (overflow detected)");
    free(p);

    /* 演示有符号/无符号混合 */
    int idx = -5;
    size_t count = 10;
    printf("Bounds check index=%d count=%zu: vulnerable=%d fixed=%d\n",
           idx, count,
           check_bounds_vulnerable(idx, count),
           check_bounds_fixed(idx, count));
    printf("\n");
}

/* ================================================================
 * 防御编程清单
 * ================================================================ */

void print_defense_checklist(void) {
    printf("=== Defense Checklist ===\n");
    printf("[ ] 所有外部输入的长度都经过验证\n");
    printf("[ ] 乘法/加法运算检查溢出（size_t 范围）\n");
    printf("[ ] 分配失败时保持对象一致性（不先 free）\n");
    printf("[ ] 不返回局部变量地址\n");
    printf("[ ] 有符号数先检查负数再转无符号\n");
    printf("[ ] 使用 AddressSanitizer (-fsanitize=address) 测试\n");
    printf("[ ] 使用 FORTIFY_SOURCE (-D_FORTIFY_SOURCE=2) 编译\n");
    printf("[ ] 静态分析：Cppcheck, clang-analyzer\n");
}

int main(void) {
    demo_heartbeat();
    demo_uaf();
    demo_integer_overflow();
    print_defense_checklist();
    return 0;
}
