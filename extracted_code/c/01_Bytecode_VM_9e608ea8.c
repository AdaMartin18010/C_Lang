/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 352
 * Language: c
 * Block ID: 9e608ea8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 标记-清除垃圾回收器

typedef enum {
    OBJ_INT,
    OBJ_STRING,
    OBJ_ARRAY,
    OBJ_FUNCTION,
} ObjType;

typedef struct Obj {
    ObjType type;
    bool marked;           // GC标记
    struct Obj *next;      // GC链表
    size_t size;           // 对象大小
} Obj;

typedef struct {
    Obj *head;             // GC链表头
    size_t allocated;      // 已分配内存
    size_t threshold;      // GC触发阈值
} GC;

// 创建对象
Obj *gc_alloc(GC *gc, size_t size, ObjType type) {
    Obj *obj = calloc(1, sizeof(Obj) + size);
    obj->type = type;
    obj->size = size;
    obj->next = gc->head;
    gc->head = obj;
    gc->allocated += sizeof(Obj) + size;

    // 检查是否需要GC
    if (gc->allocated > gc->threshold) {
        gc_collect(gc);
    }

    return obj;
}

// 标记对象
void gc_mark(Obj *obj) {
    if (obj == NULL || obj->marked) return;
    obj->marked = true;

    // 递归标记引用
    switch (obj->type) {
        case OBJ_ARRAY: {
            // 标记数组元素...
            break;
        }
        case OBJ_FUNCTION: {
            // 标记闭包变量...
            break;
        }
        default:
            break;
    }
}

// 收集垃圾
void gc_collect(GC *gc) {
    // 1. 标记阶段：从根开始标记
    // mark_roots();

    // 2. 清除阶段：回收未标记对象
    Obj **obj = &gc->head;
    while (*obj != NULL) {
        if (!(*obj)->marked) {
            // 未标记，回收
            Obj *unreached = *obj;
            *obj = unreached->next;
            gc->allocated -= sizeof(Obj) + unreached->size;
            free(unreached);
        } else {
            // 已标记，保留，清除标记
            (*obj)->marked = false;
            obj = &(*obj)->next;
        }
    }

    // 调整阈值
    gc->threshold = gc->allocated * 2;
}
