/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 340
 * Language: c
 * Block ID: 0ba96adb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

C内存模型（简化）：
─────────────────────────────────────────
Memory = Address → Value ∪ {⊥}          // 地址到值的部分函数
Pointer = Address ∪ {NULL} ∪ {⊥}        // 指针是地址，可能无效

操作语义：
• malloc(n): 返回新分配的地址块，或NULL
• free(p): 释放地址p，之后p成为悬挂指针
• *p: 解引用，如果p未分配或未初始化 → 未定义行为(UB)

问题：Pointer类型没有生命周期信息，
      无法静态区分有效指针和悬挂指针
