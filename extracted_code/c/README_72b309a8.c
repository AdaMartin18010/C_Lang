/*
 * Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
 * Line: 225
 * Language: c
 * Block ID: 72b309a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 结构体传递
typedef struct {
    double x, y, z;
} Point3D;

// 结构体大小 <= 16 字节，可能通过寄存器传递
// 结构体大小 > 16 字节，通过栈或指针传递
Point3D transform_point(Point3D p, double matrix[3][3]);
