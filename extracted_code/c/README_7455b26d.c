/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\README.md
 * Line: 146
 * Language: c
 * Block ID: 7455b26d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 结构体
// user.h
typedef struct {
    uint64_t id;
    char name[256];
    int age;
} User;

User create_user(uint64_t id, const char* name, int age);
