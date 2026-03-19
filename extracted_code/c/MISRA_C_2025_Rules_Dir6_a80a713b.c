/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir6.md
 * Line: 630
 * Language: c
 * Block ID: a80a713b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C23新类型使用指导 [待官方确认] */

/* _BitInt - 任意精度整数 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* ✅ 合规使用 [待官方确认] */
    _BitInt(17) sensor_id;  /* 17位正好满足需求 */
    _BitInt(24) color_value;  /* 24位颜色值 */

    /* ❌ 避免 [待官方确认] */
    /* _BitInt(8) byte; */  /* 使用uint8_t更清晰 */
    /* _BitInt(32) word; */  /* 使用uint32_t更清晰 */
#endif

/* char8_t - UTF-8字符 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* const char8_t *utf8_str = u8"Hello"; */
#endif

/* nullptr_t - 空指针类型 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* nullptr_t null_ptr = nullptr; */
#endif
