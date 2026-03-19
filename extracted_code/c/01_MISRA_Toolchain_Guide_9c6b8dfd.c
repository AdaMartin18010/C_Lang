/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\tools\01_MISRA_Toolchain_Guide.md
 * Line: 889
 * Language: c
 * Block ID: 9c6b8dfd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// =============================================================================
// 典型MISRA违规报告格式解析
// =============================================================================

// PC-lint Plus格式:
// src/main.c:42:10: Warning 574: Signed-unsigned mix with relational (MISRA C:2023 Rule 10.4)
// ^^^^^^^^^ ^^ ^^ ^^^^^^^ ^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// |         |  |  |       |   |
// |         |  |  |       |   错误描述和MISRA规则引用
// |         |  |  |       消息编号
// |         |  |  严重级别
// |         |  列号
// |         行号
// 文件名

// Cppcheck格式:
// src/main.c:42:10: warning: Variable 'x' is assigned a value that is never used [misra-c2012-2.2]
// ^^^^^^^^^ ^^ ^^ ^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^
// |         |  |  |       |                                                   |
// |         |  |  |       |                                                   MISRA规则ID
// |         |  |  |       错误描述
// |         |  |  严重级别
// |         |  列号
// |         行号
// 文件名

// Clang-Tidy格式:
// /path/to/main.c:42:10: warning: variable 'x' is assigned but never used [clang-analyzer-deadcode.DeadStores]
// ^^^^^^^^^^^^^^^ ^^ ^^ ^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// |               |  |  |         |                                         检查器ID
// |               |  |  |         错误描述
// |               |  |  严重级别
// |               |  列号
// |               行号
// 绝对路径
