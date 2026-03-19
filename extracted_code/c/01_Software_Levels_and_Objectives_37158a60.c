/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\DO_178C\01_Software_Levels_and_Objectives.md
 * Line: 225
 * Language: c
 * Block ID: 37158a60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * DAL A要求进行数据流分析，确保：
 * - 所有变量在使用前定义
 * - 无死存储
 * - 无内存泄漏
 */

/* ✅ 良好 - 明确定义和使用 */
int good_example(void)
{
    int a = 10;  /* d(definition) */
    int b = a + 5;  /* a被使用(r), b被定义(d) */
    return b;  /* b被使用(r) */
}

/* ❌ 问题 - 未定义使用 */
int bad_example(void)
{
    int a;  /* 未初始化 */
    int b = a + 5;  /* a被使用但未定义！ */
    return b;
}

/* ❌ 问题 - 死存储 */
int dead_store(void)
{
    int a = 10;  /* d */
    a = 20;  /* 死存储 - 上一个值未被使用 */
    return a;
}
