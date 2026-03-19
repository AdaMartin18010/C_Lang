/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 690
 * Language: c
 * Block ID: f3b729d1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 跨时钟域信号传输的问题
// 当信号在目标时钟沿附近变化时，触发器可能进入亚稳态

// 解决方案：双触发器同步器
Bool sync_stage1 = async_input;  // 第一级（可能亚稳态）
Bool sync_output = sync_stage1;   // 第二级（大概率已稳定）
