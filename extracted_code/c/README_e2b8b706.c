/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 429
 * Language: c
 * Block ID: e2b8b706
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 结构体传递规则 (System V AMD64)

typedef struct {
    int x, y;      // 8字节，可放入寄存器
} Point;

typedef struct {
    long a, b, c;  // 24字节，在栈上传递
} BigStruct;

typedef struct {
    float x, y, z, w;  // 16字节，可放入XMM寄存器
} Vector4;

// 编译后的汇编行为:
// void process(Point p)    → 通过EDI传递
// void process(BigStruct s) → 通过栈传递(复制)
// void process(Vector4 v)  → 通过XMM0传递
