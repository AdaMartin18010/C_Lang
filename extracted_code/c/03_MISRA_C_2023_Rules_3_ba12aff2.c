/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 121
 * Language: c
 * Block ID: ba12aff2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 方案1: URL放在字符串常量中 */
const char *docs = "http://docs.example.com/api";

/* 方案2: 删除协议前缀 */
/* Documentation: docs.example.com/api/v1 */

/* 方案3: 使用特殊注释格式 */
/**
 * @url https://example.com
 * 使用Doxygen格式，工具能正确处理
 */
