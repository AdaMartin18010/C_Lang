//
// Auto-generated from: Zig\formal_models\Core_Concepts_Definitions.md
// Line: 93
// Language: zig
// Block ID: 656936c6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

定义 2.4 (变量生命周期)
对于变量声明 var x: τ = init:

lifetime(x) = [begin_decl, end_scope)

其中:
- begin_decl: 声明执行时刻
- end_scope: 包含声明的作用域结束时刻

规则 2.1 (栈变量)
若 x 为栈变量，则:
1. lifetime(x) 结束后，&x 变为无效
2. x 的存储在 end_scope 自动释放

规则 2.2 (堆变量)
若 x 为堆分配 (allocator.create)，则:
1. lifetime(x) 与分配器关联
2. 必须显式调用 allocator.destroy(x)
3. defer 确保在 scope 结束时释放
