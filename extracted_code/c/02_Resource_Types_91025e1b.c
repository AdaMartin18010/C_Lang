/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 357
 * Language: c
 * Block ID: 91025e1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 唯一指针（模拟unique_ptr）
typedef struct {
    void *ptr;
    void (*destructor)(void *);
} UniquePtr;

UniquePtr *unique_ptr(void *ptr, void (*dtor)(void *)) {
    UniquePtr *u = malloc(sizeof(UniquePtr));
    u->ptr = ptr;
    u->destructor = dtor;
    return u;
}

void *unique_ptr_release(UniquePtr *u) {
    void *ptr = u->ptr;
    u->ptr = NULL;
    return ptr;  // 转移所有权
}

void unique_ptr_free(UniquePtr *u) {
    if (u->ptr && u->destructor) {
        u->destructor(u->ptr);
    }
    free(u);
}

// 共享指针（模拟shared_ptr）
typedef struct {
    void *ptr;
    void (*destructor)(void *);
    atomic_int *refcount;
} SharedPtr;

SharedPtr *shared_ptr(void *ptr, void (*dtor)(void *)) {
    SharedPtr *s = malloc(sizeof(SharedPtr));
    s->ptr = ptr;
    s->destructor = dtor;
    s->refcount = malloc(sizeof(atomic_int));
    atomic_init(s->refcount, 1);
    return s;
}

SharedPtr *shared_ptr_clone(SharedPtr *s) {
    atomic_fetch_add(s->refcount, 1);
    return s;  // 共享所有权
}

void shared_ptr_free(SharedPtr *s) {
    if (atomic_fetch_sub(s->refcount, 1) == 1) {
        // 最后一个引用
        if (s->destructor) s->destructor(s->ptr);
        free(s->refcount);
    }
    free(s);
}
