/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 661
 * Language: c
 * Block ID: abeff664
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 constexpr指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐：编译时常量 */
    constexpr int BUFFER_SIZE = 1024;
    constexpr int MAX_RETRIES = 3;

    /* ✅ 推荐：constexpr数组 */
    constexpr int lookup_table[] = {0, 1, 4, 9, 16};

    /* ⚠️ 注意：constexpr对象不可修改 */
    /* lookup_table[0] = 1; */  /* 编译错误或UB */
#else
    /* C11/C17兼容 */
    #define BUFFER_SIZE 1024
    static const int lookup_table[] = {0, 1, 4, 9, 16};
#endif
