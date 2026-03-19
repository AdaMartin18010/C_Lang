/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\02_Packages_Modules.md
 * Line: 463
 * Language: c
 * Block ID: d59bffee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define DEFINE_STACK(type, name, size) \
    type name##_data[size]; \
    int name##_top = 0; \
    void name##_push(type item) { \
        name##_data[name##_top++] = item; \
    }

DEFINE_STACK(int, mystack, 100);
// 问题：无类型检查，无作用域控制
