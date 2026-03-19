/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 701
 * Language: c
 * Block ID: ca4b6ed6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 一个门驱动过多负载会导致延迟增加
// 需要插入缓冲器

// 错误：一个反相器驱动20个门
Bool signal = !input;  // 扇出=20，延迟大

// 正确：使用缓冲树
Bool buffered = !input;
Bool final1 = buffered;  // 扇出=10
Bool final2 = buffered;  // 扇出=10
