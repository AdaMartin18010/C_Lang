/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\04_Storage_Duration_Lifetime.md
 * Line: 37
 * Language: c
 * Block ID: aa1f1e49
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 存储类别说明符与存储期关系
auto           → 自动存储期 (默认)
static         → 静态存储期
extern         → 静态存储期 (声明外部定义)
register       → 自动存储期 (建议寄存器存储)
_Thread_local  → 线程存储期 (C11)
