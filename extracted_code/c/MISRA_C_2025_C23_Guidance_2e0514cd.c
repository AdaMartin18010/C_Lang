/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 557
 * Language: c
 * Block ID: 2e0514cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* [待官方确认] - UTF-8支持 */

    /* char8_t用于UTF-8字符 */
    /* const char8_t *utf8_str = u8"Hello 世界"; */

    /* 与char的区别 */
    /* char可能是有符号或无符号 */
    /* char8_t始终是无符号，用于UTF-8 */

    /* 使用场景 [待确认] */
    /* - 国际化应用 */
    /* - 网络协议处理 */
    /* - 文件格式处理 */

#else
    /* C11/C17 - 使用char和UTF-8编码 */
    const char *utf8_str = "Hello \xE4\xB8\x96\xE7\x95\x8C";  /* Hello世界 */
#endif
