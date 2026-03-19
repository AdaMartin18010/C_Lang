/*
 * Auto-generated from: 06_Thinking_Representation\06_Index\03_Design_Patterns_Index.md
 * Line: 137
 * Language: c
 * Block ID: f3639ca4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 核心实现
typedef struct Product {
    void (*operation)(struct Product* self);
} Product;

Product* factory_create_product(const char* type);
