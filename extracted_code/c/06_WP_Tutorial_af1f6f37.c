/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 199
 * Language: c
 * Block ID: af1f6f37
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  ┌─────────────────────────────────────────────────────────────────────┐
  │                    完整函数合约模板                                   │
  ├─────────────────────────────────────────────────────────────────────┤
  │  // 前置条件 (调用者必须保证)                                         │
  │  @requires Precondition1;                                             │
  │  @requires Precondition2;                                             │
  │                                                                        │
  │  // 内存修改声明                                                      │
  │  @assigns MemoryLocation1, MemoryLocation2;                           │
  │  @assigns \nothing;  // 不修改任何内存                                 │
  │                                                                        │
  │  // 分配/释放声明                                                     │
  │  @allocates MemoryLocation;                                           │
  │  @frees MemoryLocation;                                               │
  │                                                                        │
  │  // 后置条件 (函数返回时必须成立)                                     │
  │  @ensures Postcondition1;                                             │
  │  @ensures Postcondition2;                                             │
  │                                                                        │
  │  // 异常行为 (当函数可能不返回时)                                     │
  │  @exits ExitCondition;                                                │
  │                                                                        │
  │  // 命名行为分支                                                      │
  │  @behavior Name1:                                                     │
  │    @assumes Condition1;  // 此行为的假设                               │
  │    @assigns ...;                                                       │
  │    @ensures ...;                                                       │
  │                                                                        │
  │  @behavior Name2:                                                     │
  │    @assumes Condition2;                                                │
  │    ...                                                                 │
  │                                                                        │
  │  // 行为完整性                                                        │
  │  @complete behaviors Name1, Name2;                                    │
  │                                                                        │
  │  // 行为互斥性                                                        │
  │  @disjoint behaviors Name1, Name2;                                    │
  └─────────────────────────────────────────────────────────────────────┘
*/
