/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 445
 * Language: c
 * Block ID: cc6f7701
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 常量表达式使用 */

/* ✅ 合规 [待官方确认] */
#define MAX 100
int arr[MAX];  /* 编译时常量 */

/* [C23相关] - constexpr表达式 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    constexpr int SIZE = 100;
    int arr[SIZE];  /* C23编译时常量 */

    /* ✅ 推荐 [待官方确认] - constexpr函数 */
    constexpr int square(int x) {
        return x * x;
    }
    int arr2[square(10)];  /* 编译时计算 */
#else
    static const int SIZE = 100;
    int arr[SIZE];  /* C11/C17 - VLA或错误 */
    /* C11/C17中const int不能用于数组大小 */
#endif
