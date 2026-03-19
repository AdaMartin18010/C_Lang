/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\README.md
 * Line: 122
 * Language: c
 * Block ID: 3f68dc0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 良好的结构体设计示例
typedef struct {
    uint32_t id;           // 4字节，自然对齐
    char name[32];         // 32字节
    double score;          // 8字节，可能对齐到8字节边界
    uint8_t flags;         // 1字节
    // 7字节填充（如果score需要8字节对齐）
} Student;

// 位域在嵌入式中的应用
typedef struct {
    uint32_t enabled : 1;   // 1位
    uint32_t mode    : 3;   // 3位
    uint32_t priority: 4;   // 4位
    uint32_t reserved: 24;  // 保留
} ControlReg;
