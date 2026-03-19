/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\06_WP_Tutorial.md
 * Line: 165
 * Language: c
 * Block ID: 7da25d4f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  ┌─────────────────────────────────────────────────────────────────────┐
  │                    ACSL 谓词逻辑                                      │
  ├─────────────────────────────────────────────────────────────────────┤
  │                                                                      │
  │  量词 (Quantifiers):                                                  │
  │  ─────────────────────────────────────────                          │
  │  \forall type x; P(x)      // 全称量词                              │
  │  \exists type x; P(x)      // 存在量词                              │
  │                                                                      │
  │  逻辑连接词:                                                          │
  │  ─────────────────────────────────────────                          │
  │  ==>     蕴含 (implication)                                          │
  │  <==>    等价 (equivalence)                                          │
  │  &&      逻辑与                                                      │
  │  ||      逻辑或                                                      │
  │  !       逻辑非                                                      │
  │                                                                      │
  │  特殊运算符:                                                          │
  │  ─────────────────────────────────────────                          │
  │  ? :     条件表达式                                                   │
  │  \let x = E; P       局部绑定                                         │
  │  \old(E)             函数入口时的值                                   │
  │  \at(E, Label)       特定标签处的值                                   │
  │                                                                      │
  └─────────────────────────────────────────────────────────────────────┘
*/
