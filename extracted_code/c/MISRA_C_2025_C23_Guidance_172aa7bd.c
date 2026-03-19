/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 240
 * Language: c
 * Block ID: 172aa7bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 编译时常量 */
    constexpr int MAX_SIZE = 1024;
    constexpr double PI = 3.14159265359;

    /* ✅ 推荐 [待官方确认] - 编译时数组 */
    constexpr int lookup_table[] = {0, 1, 4, 9, 16, 25};

    /* ✅ 推荐 [待官方确认] - 用于数组大小 */
    int buffer[MAX_SIZE];  /* 编译时确定大小 */

    /* ⚠️ 注意 [待官方确认] - constexpr不可修改 */
    /* MAX_SIZE = 2048; */  /* 编译错误 */
    /* lookup_table[0] = 1; */  /* 编译错误 */

#else
    /* C11/C17兼容 */
    #define MAX_SIZE 1024
    #define PI 3.14159265359
    static const int lookup_table[] = {0, 1, 4, 9, 16, 25};
#endif
