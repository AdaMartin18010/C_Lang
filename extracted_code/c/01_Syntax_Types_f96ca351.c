/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\01_Syntax_Types.md
 * Line: 104
 * Language: c
 * Block ID: f96ca351
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef float Meters;
typedef float Kilometers;
typedef float Seconds;

Meters dist_m = 100.0f;
Seconds t = 10.0f;

// C允许隐式转换，危险！
float speed = dist_m / t;  // 编译通过，但单位可能错误
