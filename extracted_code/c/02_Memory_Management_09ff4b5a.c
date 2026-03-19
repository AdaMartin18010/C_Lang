/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 784
 * Language: c
 * Block ID: 09ff4b5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 模式1: 基本单对象分配
 * 适用: 单个结构体或变量
 */
typedef struct {
    int id;
    char name[64];
    double value;
} Record;

Record *create_record(int id, const char *name, double value) {
    Record *r = malloc(sizeof(Record));  // 正确: sizeof对象，非类型指针
    if (r == NULL) {
        return NULL;
    }

    r->id = id;
    strncpy(r->name, name, sizeof(r->name) - 1);
    r->name[sizeof(r->name) - 1] = '\0';
    r->value = value;

    return r;
}

void destroy_record(Record *r) {
    free(r);  // NULL安全: free(NULL)是安全的无操作
}
