/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 162
 * Language: c
 * Block ID: 0688fe19
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  ┌─────────────────────────────────────────────────────────────────────┐
  │                    MetAcsl 元注解语法                                 │
  ├─────────────────────────────────────────────────────────────────────┤
  │                                                                      │
│  @meta <directive> <arguments>                                       │
│                                                                      │
│  常用指令:                                                            │
│  ─────────────────────────────────────────                          │
│  @meta auto_requires                  // 自动生成前置条件            │
│  @meta auto_ensures                   // 自动生成后置条件            │
│  @meta auto_loop_inv                  // 自动生成循环不变量          │
│  @meta template <name>                // 应用命名模板                │
│  @meta infer                          // 推断缺失的规范              │
│  @meta validate                       // 验证规范一致性              │
│                                                                      │
│  条件元注解:                                                          │
│  @meta if <condition> <directive>    // 条件执行                     │
│  @meta forall <pattern> <directive>  // 批量应用                     │
│                                                                      │
│  示例:                                                                │
│  @meta auto_requires pointer_validity                               │
│  @meta auto_ensures bounds_checking                                 │
│  @meta forall function with_prefix("safe_") auto_contract           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/
