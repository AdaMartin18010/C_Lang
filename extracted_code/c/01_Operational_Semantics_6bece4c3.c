/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\01_Operational_Semantics.md
 * Line: 166
 * Language: c
 * Block ID: 6bece4c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 扩展配置包含堆
typedef struct {
    Store env;      // 变量环境
    Heap heap;      // 堆内存
    Stack stack;    // 调用栈
} State;

// malloc语义
⟨malloc(E), σ⟩ → ⟨malloc(n), σ'⟩    如果 ⟨E, σ⟩ → ⟨n, σ'⟩
⟨malloc(n), (env, heap)⟩ → (env, heap[a ↦ block(n)])
    其中 a 是新分配地址

// free语义
⟨free(a), (env, heap)⟩ → (env, heap\{a})
    如果 a ∈ dom(heap)

// 内存错误
⟨free(a), σ⟩ → error    如果 a ∉ dom(heap) 或 已释放
