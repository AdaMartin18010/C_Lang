/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\04_C17_C23_Library.md
 * Line: 446
 * Language: c
 * Block ID: 2b1b015a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 静态断言消息可选
static_assert(sizeof(int) == 4);
static_assert(sizeof(void*) == 8, "64-bit required");

// 在更多上下文中可用
struct Header {
    uint32_t magic;
    uint32_t size;
};
static_assert(sizeof(struct Header) == 8);

// 数组大小检查
int buffer[100];
static_assert(sizeof(buffer) / sizeof(buffer[0]) == 100);
