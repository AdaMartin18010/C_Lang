/*
 * Auto-generated from: 06_Thinking_Representation\06_Index\03_Design_Patterns_Index.md
 * Line: 186
 * Language: c
 * Block ID: 7d085539
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 核心实现
typedef struct {
    TargetInterface target;
    Adaptee* adaptee;
} Adapter;

void adapter_request(Adapter* self);
