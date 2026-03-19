/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\07_C_Language_Formal_Specification.md
 * Line: 278
 * Language: c
 * Block ID: 5b8fd8b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

D ::= τ x;                        (变量声明)
  |   τ x = e;                    (带初始化)
  |   τ f(τ₁ x₁, ..., τₙ xₙ) { S }  (函数定义)
  |   typedef τ T;                (类型别名)
  |   struct T { ... };           (结构体)
  |   union T { ... };            (联合体)
  |   enum T { ... };             (枚举)
