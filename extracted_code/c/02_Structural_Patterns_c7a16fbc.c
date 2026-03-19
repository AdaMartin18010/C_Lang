/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 260
 * Language: c
 * Block ID: c7a16fbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void (*read)(void*, char*, size_t);
    void (*write)(void*, const char*, size_t);
    void (*delete)(void*);
} document_ops_t;

typedef struct {
    document_ops_t ops;
    void* real_doc;
    int user_permission;
} document_proxy_t;

void protected_write(void* self, const char* data, size_t len) {
    document_proxy_t* proxy = self;
    if (!(proxy->user_permission & PERM_WRITE)) {
        printf("Access denied: no write permission\n");
        return;
    }
    /* 转发到真实对象 */
    ((document_ops_t*)proxy->real_doc)->write(proxy->real_doc, data, len);
}
