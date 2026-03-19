/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\03_Infrastructure_Software.md
 * Line: 852
 * Language: c
 * Block ID: da188e97
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：频繁malloc/free小对象
void bad_fragmentation(void) {
    for (int i = 0; i < 1000000; i++) {
        char* small = malloc(16);  // 碎片!
        use(small);
        free(small);
    }
}

// 正确：使用对象池
typedef struct {
    char data[16];
} SmallObject;

#define POOL_SIZE 1000
static SmallObject pool[POOL_SIZE];
static int pool_index = 0;

void* pool_alloc(void) {
    if (pool_index < POOL_SIZE) {
        return &pool[pool_index++];
    }
    return malloc(sizeof(SmallObject));
}
