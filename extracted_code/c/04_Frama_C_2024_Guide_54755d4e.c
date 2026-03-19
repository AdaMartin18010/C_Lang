/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 438
 * Language: c
 * Block ID: 54755d4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  ┌─────────────────────────────────────────────────────────────────┐
  │                    ACSL 函数合约结构                              │
  ├─────────────────────────────────────────────────────────────────┤
  │  @requires  <precondition>;      // 前置条件 (调用者必须保证)     │
  │  @assigns   <modified_memory>;   // 修改的内存区域               │
  │  @ensures   <postcondition>;     // 后置条件 (实现者必须保证)     │
  │  @behavior  <name>:              // 命名行为 (可选)              │
  │    @assumes <condition>;                                        │
  │    @assigns <modified_memory>;                                  │
  │    @ensures <postcondition>;                                    │
  │  @complete behaviors;            // 行为完整性                  │
  │  @disjoint behaviors;            // 行为互斥性                  │
  └─────────────────────────────────────────────────────────────────┘
*/
