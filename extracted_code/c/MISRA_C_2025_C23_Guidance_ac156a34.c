/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 266
 * Language: c
 * Block ID: ac156a34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 简单constexpr函数 */
    constexpr int square(int x) {
        return x * x;
    }

    /* ✅ 推荐 [待官方确认] - 编译时计算 */
    int arr[square(10)];  /* 编译时确定大小为100 */

    constexpr int val = square(5);  /* 编译时常量25 */

    /* ✅ 推荐 [待官方确认] - 运行时也可用 */
    int runtime_val = square(some_variable);  /* 运行时计算 */

    /* ⚠️ 限制 [待官方确认] - constexpr函数限制 */
    /* 不能包含： */
    /* - 循环（某些编译器支持） */
    /* - goto */
    /* - 变量声明（除某些情况） */
    /* - 动态内存分配 */

#else
    /* C11/C17 - 使用宏 */
    #define square(x) ((x) * (x))

    /* 或静态函数 */
    static inline int square(int x) {
        return x * x;
    }
#endif
