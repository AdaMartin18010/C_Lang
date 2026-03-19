/*
 * Auto-generated from: 06_Thinking_Representation\06_Index\03_Design_Patterns_Index.md
 * Line: 254
 * Language: c
 * Block ID: 13b3d50a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 核心实现
typedef struct Observer {
    void (*update)(struct Observer* self, void* data);
} Observer;

typedef struct Subject {
    Observer* observers[MAX_OBSERVERS];
    void (*attach)(struct Subject* self, Observer* obs);
    void (*notify)(struct Subject* self, void* data);
} Subject;
