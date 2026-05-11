/*
 * MISRA C:2023 控制流规则对照
 * 编译: gcc -Wall -Wextra -std=c11 -o control_flow control_flow.c
 */
#include <stdio.h>
#include <stdlib.h>

/* ================================================================
 * RULE 14.1: 循环计数器不应是浮点类型
 * ================================================================ */

/* ❌ 违规: 浮点循环计数器 */
void rule14_1_violation(void) {
    for (float f = 0.0f; f < 10.0f; f += 0.1f) {  /* 累积误差 */
        /* ... */
    }
}

/* ✅ 合规: 整数计数器 */
void rule14_1_compliant(void) {
    for (int i = 0; i < 100; i++) {
        float f = i * 0.1f;  /* 每次重新计算 */
        (void)f;
    }
}

/* ================================================================
 * RULE 14.4: for 循环应使用单个循环计数器
 * ================================================================ */

/* ❌ 违规: 多个循环计数器修改 */
void rule14_4_violation(void) {
    for (int i = 0, j = 10; i < 10; i++, j--) {  /* 两个计数器 */
        (void)(i + j);
    }
}

/* ✅ 合规: 单一计数器，其余逻辑在循环内 */
void rule14_4_compliant(void) {
    for (int i = 0; i < 10; i++) {
        int j = 10 - i;
        (void)(i + j);
    }
}

/* ================================================================
 * RULE 15.1: goto 仅允许跳转到更深层嵌套
 * ================================================================ */

/* ❌ 违规: 向前跳转 */
void rule15_1_violation(int x) {
    if (x > 0) {
        goto label;  /* 向前跳转 */
    }
    return;
label:
    printf("forward jump\n");
}

/* ✅ 合规: 仅向后跳转到统一退出点 */
void rule15_1_compliant(int x) {
    int result = 0;
    if (x > 0) {
        result = 1;
        goto cleanup;  /* 向后跳转到统一清理 */
    }
    /* 其他处理 */
cleanup:
    (void)result;
    return;
}

/* ================================================================
 * RULE 15.5: switch 语句应有 default 标签
 * ================================================================ */

/* ❌ 违规: 缺少 default */
int rule15_5_violation(int cmd) {
    switch (cmd) {
        case 1: return 1;
        case 2: return 2;
    }
    return 0;  /* 隐式处理其他值 */
}

/* ✅ 合规: 显式 default */
int rule15_5_compliant(int cmd) {
    switch (cmd) {
        case 1: return 1;
        case 2: return 2;
        default:
            /* 记录未预期的命令 */
            return 0;
    }
}

/* ================================================================
 * RULE 15.6: switch 的 case 应以 break 终止
 * ================================================================ */

/* ❌ 违规: 隐式 fall-through */
int rule15_6_violation(int x) {
    switch (x) {
        case 1:
            x = 10;
            /* 缺少 break，fall-through */
        case 2:
            return x + 1;
        default:
            return 0;
    }
}

/* ✅ 合规: 每个 case 以 break/return 终止 */
int rule15_6_compliant(int x) {
    switch (x) {
        case 1:
            x = 10;
            break;  /* 显式终止 */
        case 2:
            return x + 1;
        default:
            return 0;
    }
    return x;
}

/* ================================================================
 * RULE 15.7: 所有 switch 语句应有至少两个 case 标签
 * ================================================================ */

/* ❌ 违规: 仅一个 case + default */
int rule15_7_violation(int x) {
    switch (x) {
        case 1: return 1;
        default: return 0;  /* 技术上两个标签，但逻辑上等价于 if */
    }
}

/* ✅ 合规: 使用 if 替代简单 switch */
int rule15_7_compliant(int x) {
    if (x == 1) {
        return 1;
    }
    return 0;
}

/* 或当有多个有效 case 时 */
int rule15_7_compliant_multi(int x) {
    switch (x) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        default: return 0;
    }
}

/* ================================================================
 * RULE 16.1: 所有函数路径应有单一出口点
 * ================================================================ */

/* ❌ 违规: 多个 return */
int rule16_1_violation(int x) {
    if (x < 0) return -1;
    if (x == 0) return 0;
    return 1;
}

/* ✅ 合规: 单一出口 */
int rule16_1_compliant(int x) {
    int result;
    if (x < 0) {
        result = -1;
    } else if (x == 0) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
}

/* ================================================================
 * RULE 17.2: 函数不应递归调用
 * ================================================================ */

/* ❌ 违规: 递归 */
int rule17_2_violation(int n) {
    if (n <= 1) return 1;
    return n * rule17_2_violation(n - 1);  /* 递归 */
}

/* ✅ 合规: 迭代替代递归 */
int rule17_2_compliant(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* ================================================================
 * 测试主函数
 * ================================================================ */
int main(void) {
    printf("MISRA Control Flow Examples compiled successfully\n");
    rule14_1_compliant();
    rule14_4_compliant();
    rule15_1_compliant(1);
    printf("15.5: %d\n", rule15_5_compliant(2));
    printf("15.6: %d\n", rule15_6_compliant(1));
    printf("15.7: %d\n", rule15_7_compliant_multi(2));
    printf("16.1: %d\n", rule16_1_compliant(-5));
    printf("17.2: %d\n", rule17_2_compliant(5));
    return 0;
}
