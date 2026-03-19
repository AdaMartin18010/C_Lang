/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\01_Structures_Unions.md
 * Line: 98
 * Language: c
 * Block ID: 1cf1c80a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 硬件寄存器映射
struct ControlRegister {
    unsigned int enable    : 1;   // bit 0
    unsigned int mode      : 3;   // bits 1-3
    unsigned int reserved  : 4;   // bits 4-7
    unsigned int status    : 8;   // bits 8-15
    unsigned int           : 16;  // 填充到32位
};

// 位域布局是实现定义的！
// 使用场景：
// 1. 硬件寄存器映射
// 2. 协议包头解析
// 3. 内存受限的密集存储

// C11匿名结构体/联合体内嵌
struct Packet {
    uint8_t header;
    union {
        struct {
            uint16_t src;
            uint16_t dst;
        };
        uint32_t raw;
    } addr;
};
