/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 378
 * Language: c
 * Block ID: 8eadcfc3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 术语混乱：同一概念不同命名
void create_user();      // 创建
void add_account();      // 添加
void register_member();  // 注册
void new_customer();     // 新
// 这些都表示同一个概念！

// 修复：统一术语
typedef enum {
    USER_TYPE_REGULAR,
    USER_TYPE_ADMIN,
    USER_TYPE_GUEST
} UserType;

Result user_create(const char *name, UserType type);
Result user_delete(UserID id);
Result user_update(UserID id, UserAttributes *attrs);
