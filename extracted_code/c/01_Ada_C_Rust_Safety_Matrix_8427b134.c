/*
 * Auto-generated from: 17_Ada_SPARK\08_Comparison_Matrices\01_Ada_C_Rust_Safety_Matrix.md
 * Line: 117
 * Language: c
 * Block ID: 8427b134
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C - 类型不安全
typedef float Meters;
typedef float Seconds;
Meters m = 100.0;
Seconds s = m;  // 允许！单位混淆
