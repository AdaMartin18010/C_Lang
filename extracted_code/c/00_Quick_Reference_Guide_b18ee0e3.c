/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 289
 * Language: c
 * Block ID: b18ee0e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 错误 */
void process(Data *p)
{
    p->value = 0;  /* 可能空指针 */
}

/* ✅ 修复 */
void process(Data *p)
{
    if (p == NULL) {
        return;  /* 或错误处理 */
    }
    p->value = 0;
}
