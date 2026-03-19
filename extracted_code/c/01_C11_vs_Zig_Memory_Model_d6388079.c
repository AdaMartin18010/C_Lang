/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 1712
 * Language: c
 * Block ID: d6388079
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 引用计数智能指针
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
    _Atomic int count;
    void (*destroy)(void*);
    char data[];
} RcHeader;

typedef struct {
    RcHeader* ptr;
} Rc;

Rc rc_new(size_t data_size, void (*destroy)(void*)) {
    RcHeader* header = malloc(sizeof(RcHeader) + data_size);
    atomic_init(&header->count, 1);
    header->destroy = destroy;
    return (Rc){.ptr = header};
}

void* rc_data(Rc rc) {
    return rc.ptr->data;
}

Rc rc_clone(Rc rc) {
    // Relaxed: 仅计数，不需要顺序保证
    atomic_fetch_add_explicit(&rc.ptr->count, 1, memory_order_relaxed);
    return rc;
}

void rc_drop(Rc* rc) {
    if (rc->ptr == NULL) return;

    // Release: 确保之前的写入在可能销毁前可见
    int old_count = atomic_fetch_sub_explicit(&rc->ptr->count, 1,
                                               memory_order_release);

    if (old_count == 1) {
        // 最后一个引用
        // Acquire: 确保看到 count==0 后，能看到所有 clone 的操作
        atomic_thread_fence(memory_order_acquire);

        if (rc->ptr->destroy) {
            rc->ptr->destroy(rc->ptr->data);
        }
        free(rc->ptr);
    }

    rc->ptr = NULL;
}

// 使用示例
typedef struct {
    int value;
} MyData;

void my_data_destroy(void* data) {
    MyData* d = data;
    printf("Destroying data with value %d\n", d->value);
}

void example(void) {
    Rc rc1 = rc_new(sizeof(MyData), my_data_destroy);
    MyData* data = rc_data(rc1);
    data->value = 42;

    Rc rc2 = rc_clone(rc1);  // 引用计数 = 2

    rc_drop(&rc1);  // 引用计数 = 1，不销毁
    rc_drop(&rc2);  // 引用计数 = 0，销毁
}
