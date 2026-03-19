/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 147
 * Language: c
 * Block ID: 207b18d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 行拼接（Line-splicing）使用反斜杠\在行尾 */
/* 将物理上的多行连接成逻辑上的一行 */

/* 示例：宏定义中使用 */
#define LONG_MACRO \
    (very_long_expression + \
     another_expression)

/* 问题：在 // 注释中使用行拼接 */
