/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Semantic_Equivalence.md
 * Line: 304
 * Language: c
 * Block ID: bdea265b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单表达式
〚n〛 = n
〚x〛 = lookup(x)
〚e₁ + e₂〛 = plus(〚e₁〛, 〚e₂〛)

// 赋值表达式
〚x = e〛 = store(x, 〚e〛)
