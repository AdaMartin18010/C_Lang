/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 187
 * Language: c
 * Block ID: ab63df4d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 泛型max宏 */
    #define max(a, b) ({ \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b; \
    })

    int x = 5, y = 10;
    int m = max(x++, y++);  /* 安全：只递增一次 */

    /* ✅ 推荐 [待官方确认] - 函数指针类型 */
    typedef int (*CompareFunc)(const void *, const void *);
    typeof(CompareFunc) my_compare = strcmp_wrapper;

    /* ✅ 推荐 [待官方确认] - 临时变量类型匹配 */
    int array[100];
    typeof(array[0]) temp = array[0];  /* 类型匹配 */

    /* ✅ 推荐 [待官方确认] - 指针类型 */
    int *ptr = get_data();
    typeof(ptr) ptr_copy = ptr;  /* 相同类型 */

    /* ❌ 避免 [待官方确认] - 过度复杂 */
    /* typeof(typeof(int (*)())[10]) confusing; */

    /* ❌ 避免 [待官方确认] - 函数调用副作用 */
    /* typeof(func()) x = func(); */  /* 两次调用！ */

#else
    /* C11/C17 - 使用typedef */
    typedef int my_type;

    /* 或使用GCC扩展 */
    #ifdef __GNUC__
        #define typeof __typeof__
    #else
        #define typeof /* 不支持时回退 */
    #endif
#endif
