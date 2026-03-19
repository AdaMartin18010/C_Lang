/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 683
 * Language: c
 * Block ID: f58017ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23 typeof指导 [待官方确认] */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐：简化复杂类型声明 */
    typeof(int (*)(int, int)) math_op;

    /* ✅ 推荐：泛型编程 */
    #define max(a, b) ({ \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a > _b ? _a : _b; \
    })

    /* ❌ 避免：过度嵌套 */
    /* typeof(typeof(int (*)())[10]) confusing; */
#else
    /* C11/C17使用typedef */
    typedef int (*math_op_t)(int, int);
    math_op_t op;
#endif
