/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2806
 * Language: c
 * Block ID: 9d6096f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 使用注释明确所有权语义
 */

// [OWNERSHIP: caller] - 调用者负责释放返回的内存
char *strdup_owned(const char *src);

// [OWNERSHIP: callee] - 函数接管传入内存的所有权
void list_push_owned(List *list, /* transfer */ char *data);

// [OWNERSHIP: borrowed] - 函数只借用，不转移所有权
void process_borrowed(const char *data);  // 函数内不保存指针

// [OWNERSHIP: shared] - 引用计数管理
Buffer *buffer_ref(Buffer *buf);
void buffer_unref(Buffer *buf);

/*
 * 示例使用
 */
typedef struct {
    char *name;  // [OWNERSHIP: owned by struct]
    Data *data;  // [OWNERSHIP: owned by struct]
} Object;

// 构造函数 - 接管或复制所有权
Object *object_create(const char *name, /* transfer */ Data *data);

// 析构函数 - 释放所有拥有资源
void object_destroy(Object *obj);

// 访问器 - 返回借用引用
const char *object_get_name(const Object *obj);  // [OWNERSHIP: borrowed]
