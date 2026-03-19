/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\01_JIT_Basics.md
 * Line: 695
 * Language: c
 * Block ID: 0751d61f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 代码垃圾回收：回收不再使用的JIT代码

typedef struct {
    GList *all_code_blocks;
    GHashTable *pinned_blocks;  // 被引用的代码块
} CodeGC;

// 标记阶段
void code_gc_mark(CodeGC *gc) {
    // 标记根：当前执行的代码
    mark_executing_code(gc);

    // 从栈遍历标记
    mark_code_from_stack(gc);

    // 从全局句柄标记
    mark_code_from_handles(gc);

    // 传播标记（通过调用图）
    propagate_marks(gc);
}

// 清除阶段
void code_gc_sweep(CodeGC *gc) {
    GList *to_free = NULL;

    for (GList *l = gc->all_code_blocks; l != NULL; l = l->next) {
        CodeBlock *block = l->data;

        if (!block->is_marked) {
            // 取消分配代码块
            to_free = g_list_prepend(to_free, block);
        } else {
            // 清除标记，为下次GC做准备
            block->is_marked = false;
        }
    }

    // 释放未标记的代码块
    for (GList *l = to_free; l != NULL; l = l->next) {
        CodeBlock *block = l->data;

        // 必须先使代码失效（防止正在执行）
        invalidate_code_block(block);

        // 解除内存映射
        munmap(block->start, block->size);

        // 从列表移除
        gc->all_code_blocks = g_list_remove(gc->all_code_blocks, block);
        free(block);
    }

    g_list_free(to_free);
}

// 完整GC周期
void run_code_gc(CodeGC *gc) {
    code_gc_mark(gc);
    code_gc_sweep(gc);
}
