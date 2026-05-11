/*
 * MISRA C:2023 类型安全规则对照
 * 编译: gcc -Wall -Wextra -std=c11 -Wconversion -o type_safety type_safety.c
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* ================================================================
 * RULE 10.1: 操作数不应是本质不合适的类型
 * ================================================================ */

/* ❌ 违规: 有符号与无符号混合 */
int rule10_1_violation(signed int s, unsigned int u) {
    return s < u;  /* s 被提升为无符号，负数变很大正数 */
}

/* ✅ 合规: 显式转换或避免混合 */
int rule10_1_compliant(signed int s, unsigned int u) {
    if (s < 0) return 1;  /* 先处理负数情况 */
    return (unsigned int)s < u;
}

/* ================================================================
 * RULE 10.3: 赋值表达式不应改变本质类型
 * ================================================================ */

/* ❌ 违规: 窄化转换 */
void rule10_3_violation(void) {
    uint32_t big = 300;
    uint8_t small = big;  /* 窄化，可能截断 */
    (void)small;
}

/* ✅ 合规: 显式范围检查 */
void rule10_3_compliant(uint32_t big) {
    if (big > UINT8_MAX) {
        /* 处理错误 */
        return;
    }
    uint8_t small = (uint8_t)big;
    (void)small;
}

/* ================================================================
 * RULE 10.4: 位操作仅应用于无符号整数本质类型
 * ================================================================ */

/* ❌ 违规: 对有符号数进行位操作 */
int rule10_4_violation(int flags) {
    return flags & 0x80;  /* 有符号位操作是未定义行为（某些位） */
}

/* ✅ 合规: 使用无符号类型 */
unsigned int rule10_4_compliant(unsigned int flags) {
    return flags & 0x80U;
}

/* ================================================================
 * RULE 10.8: 复合表达式的值不应转换为不同的本质类型类别
 * ================================================================ */

/* ❌ 违规: 转换复合表达式 */
float rule10_8_violation(int a, int b) {
    return (float)(a / b);  /* 整数除法先执行，然后转 float，丢失小数 */
}

/* ✅ 合规: 先转换再运算 */
float rule10_8_compliant(int a, int b) {
    return (float)a / (float)b;
}

/* ================================================================
 * RULE 11.x: 指针类型转换
 * ================================================================ */

/* ❌ 违规: 对象指针与函数指针互转 */
void rule11_violation(void) {
    void (*fp)(void) = (void (*)(void))malloc;  /* 未定义行为 */
    (void)fp;
}

/* ✅ 合规: 保持类型一致性 */
void *rule11_compliant(size_t size) {
    return malloc(size);
}

/* ================================================================
 * RULE 12.2: 移位操作数应在有效范围内
 * ================================================================ */

/* ❌ 违规: 移位数 >= 类型宽度 */
uint32_t rule12_2_violation(uint32_t x, uint32_t shift) {
    return x << shift;  /* 如果 shift >= 32，未定义行为 */
}

/* ✅ 合规: 检查移位数 */
uint32_t rule12_2_compliant(uint32_t x, uint32_t shift) {
    if (shift >= 32U) {
        return 0;  /* 或报错 */
    }
    return x << shift;
}

/* ================================================================
 * 测试主函数
 * ================================================================ */
int main(void) {
    printf("MISRA Type Safety Examples compiled successfully\n");
    printf("10.1 compliant: %d\n", rule10_1_compliant(-5, 10));
    rule10_3_compliant(100);
    printf("10.4 compliant: %u\n", rule10_4_compliant(0xFF));
    printf("10.8 compliant: %f\n", rule10_8_compliant(5, 2));
    printf("12.2 compliant: %u\n", rule12_2_compliant(1, 5));
    return 0;
}
