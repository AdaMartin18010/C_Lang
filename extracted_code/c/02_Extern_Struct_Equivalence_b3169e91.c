/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 1280
 * Language: c
 * Block ID: b3169e91
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 严格别名规则详解
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// 严格别名规则: 两种类型必须兼容才能别名访问
// 允许的类型: char*, unsigned char*, void* (C 中 void* 有特殊规则)

// 未定义行为: 通过不兼容类型指针访问
void undefined_alias(int *ip, float *fp) {
    *ip = 42;
    // 假设编译器知道 ip 和 fp 不指向同一对象
    // 可能优化掉对 fp 的重新加载
    float f = *fp;  // 未定义行为如果 ip 和 fp 别名
    printf("%f\n", f);
}

// 安全: 使用 char* 访问
void safe_char_alias(int *ip) {
    char *cp = (char*)ip;
    for (size_t i = 0; i < sizeof(int); i++) {
        printf("%02x ", (unsigned char)cp[i]);
    }
    printf("\n");
}

// 安全: 使用 union (C99 起允许的类型双关)
union IntFloat {
    int i;
    float f;
};

void safe_union_pun() {
    union IntFloat u;
    u.f = 3.14159f;
    printf("Float %f has int representation: 0x%x\n", u.f, u.i);
}

// 安全: 使用 memcpy
void safe_memcpy_pun() {
    float f = 3.14159f;
    int i;
    memcpy(&i, &f, sizeof(i));
    printf("Via memcpy: 0x%x\n", i);
}

// 陷阱: 通过指针转换的类型双关
void dangerous_pun() {
    float f = 3.14159f;
    int *ip = (int*)&f;  // 违反严格别名规则
    printf("Via pointer cast: 0x%x\n", *ip);  // 未定义行为
}

// 常见模式: 网络字节序转换
uint32_t safe_ntohl(uint8_t *data) {
    // 通过 char* 访问是安全的
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8)  |
           ((uint32_t)data[3]);
}

// C23 新增: _BitInt 和 #embed 等特性
