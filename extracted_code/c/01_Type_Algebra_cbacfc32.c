/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\01_Type_Algebra.md
 * Line: 189
 * Language: c
 * Block ID: cbacfc32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C的 struct 没有子类型关系
struct Animal { int legs; };
struct Dog { int legs; char *breed; };

// 虽然 Dog "包含" Animal，但 C 不认为是子类型
// struct Animal a = dog;  // 错误！
