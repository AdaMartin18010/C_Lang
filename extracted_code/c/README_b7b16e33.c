/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\README.md
 * Line: 571
 * Language: c
 * Block ID: b7b16e33
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file simple_gc.c
 * @brief 简单标记-清除垃圾回收器
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define HEAP_SIZE 1024
#define MAX_ROOTS 64

/* 对象类型 */
typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

/* 对象结构 */
typedef struct Object {
    ObjectType type;
    bool marked;
    struct Object *next;  /* 链表指针 */

    union {
        int value;              /* OBJ_INT */
        struct {                /* OBJ_PAIR */
            struct Object *head;
            struct Object *tail;
        } pair;
    } data;
} Object;

/* 堆管理器 */
typedef struct {
    Object *first_object;   /* 已分配对象链表 */
    Object *heap;           /* 堆内存 */
    int num_objects;
    int max_objects;        /* GC触发阈值 */

    Object *roots[MAX_ROOTS];
    int num_roots;
} VM;

/* 创建VM */
VM* vm_new(void) {
    VM *vm = malloc(sizeof(VM));
    vm->first_object = NULL;
    vm->heap = malloc(sizeof(Object) * HEAP_SIZE);
    vm->num_objects = 0;
    vm->max_objects = 8;  /* 初始阈值 */
    vm->num_roots = 0;
    return vm;
}

/* 添加根对象 */
void vm_add_root(VM *vm, Object *obj) {
    if (vm->num_roots >= MAX_ROOTS) {
        fprintf(stderr, "Too many roots!\n");
        exit(1);
    }
    vm->roots[vm->num_roots++] = obj;
}

/* 标记对象 */
void mark(Object *obj) {
    if (obj == NULL || obj->marked) return;

    obj->marked = true;

    if (obj->type == OBJ_PAIR) {
        mark(obj->data.pair.head);
        mark(obj->data.pair.tail);
    }
}

/* 标记阶段 */
void mark_all(VM *vm) {
    for (int i = 0; i < vm->num_roots; i++) {
        mark(vm->roots[i]);
    }
}

/* 清除阶段 */
void sweep(VM *vm) {
    Object **obj = &vm->first_object;
    while (*obj != NULL) {
        if (!(*obj)->marked) {
            /* 未标记，回收 */
            Object *unreached = *obj;
            *obj = unreached->next;
            vm->num_objects--;
            /* 实际内存池实现中会返回空闲列表 */
        } else {
            /* 清除标记 */
            (*obj)->marked = false;
            obj = &(*obj)->next;
        }
    }
}

/* 执行GC */
void gc(VM *vm) {
    printf("-- GC: %d objects before --\n", vm->num_objects);

    mark_all(vm);
    sweep(vm);

    printf("-- GC: %d objects after --\n", vm->num_objects);

    /* 调整阈值 */
    vm->max_objects = vm->num_objects * 2;
}

/* 分配对象 */
Object* new_object(VM *vm, ObjectType type) {
    if (vm->num_objects >= vm->max_objects) {
        gc(vm);
    }

    Object *obj = &vm->heap[vm->num_objects++];
    obj->type = type;
    obj->marked = false;
    obj->next = vm->first_object;
    vm->first_object = obj;

    return obj;
}

/* 创建整数 */
Object* new_int(VM *vm, int value) {
    Object *obj = new_object(vm, OBJ_INT);
    obj->data.value = value;
    return obj;
}

/* 创建数对 */
Object* new_pair(VM *vm, Object *head, Object *tail) {
    Object *obj = new_object(vm, OBJ_PAIR);
    obj->data.pair.head = head;
    obj->data.pair.tail = tail;
    return obj;
}

/* 打印对象 */
void print_object(Object *obj) {
    if (obj == NULL) {
        printf("null");
        return;
    }

    switch (obj->type) {
        case OBJ_INT:
            printf("%d", obj->data.value);
            break;
        case OBJ_PAIR:
            printf("(");
            print_object(obj->data.pair.head);
            printf(" . ");
            print_object(obj->data.pair.tail);
            printf(")");
            break;
    }
}

/* 示例程序 */
int main() {
    VM *vm = vm_new();

    printf("Creating objects...\n");

    /* 创建列表: (1 . (2 . (3 . null))) */
    Object *list = new_pair(vm,
        new_int(vm, 1),
        new_pair(vm,
            new_int(vm, 2),
            new_pair(vm,
                new_int(vm, 3),
                NULL)));

    vm_add_root(vm, list);

    printf("List: ");
    print_object(list);
    printf("\n\n");

    /* 创建一些临时对象 */
    printf("Creating temporary objects...\n");
    for (int i = 0; i < 10; i++) {
        Object *temp = new_int(vm, i);
        (void)temp; /* 临时对象，不加入根集合 */
    }

    printf("\nList after GC: ");
    print_object(list);
    printf("\n");

    free(vm->heap);
    free(vm);

    return 0;
}
