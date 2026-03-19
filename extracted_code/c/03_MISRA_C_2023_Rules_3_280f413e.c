/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 181
 * Language: c
 * Block ID: 280f413e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用块注释跨多行 */
/* This is a comment
 * that spans multiple lines
 * properly */

/* ✅ 合规 - 每行独立的行注释 */
// First part of the comment
// Second part of the comment
// Third part of the comment

/* ✅ 合规 - 使用宏时的正确做法 */
#define SAFE_MACRO do { \
    operation1(); \
    operation2(); \
} while(0)

/* ✅ 合规 - 字符串字面量中的行拼接是允许的 */
const char *msg = "This is a very long string "
                  "that continues on the next line "
                  "and is properly concatenated.";
