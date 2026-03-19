/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 679
 * Language: c
 * Block ID: 76d7b110
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：组合逻辑中的竞争
Bool output = (a && b) || (!a && c);
// 当a变化时，可能产生毛刺

// 解决方案：添加冗余项或使用触发器同步
Bool output_safe = (a && b) || (!a && c) || (b && c);  // 冗余项消除险态
