/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 352
 * Language: c
 * Block ID: 3437b186
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 [C:2023延续] - 源代码使用ASCII */
const char *msg = "Temperature: 25C";

/* ✅ 合规 - 本地化通过资源文件 */
const char *localized = get_string(STRING_ID_TEMP);

/* [C23相关] - UTF-8字符 [待官方确认] */
#if __STDC_VERSION__ >= 202311L
    /* C23 char8_t - 使用待定 */
    /* const char8_t *utf8_msg = u8"Hello"; */
#endif

/* ✅ 合规 - 字符串字面量 */
const char *ascii_only = "Safe ASCII string";  /* 推荐 */
