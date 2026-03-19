/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 185
 * Language: c
 * Block ID: 59e60b21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef float Meters;
typedef float Seconds;

Meters distance = 100.0f;
Seconds time = 10.0f;

// 危险！单位不匹配但编译通过
float speed = distance / time;  // 单位混乱
