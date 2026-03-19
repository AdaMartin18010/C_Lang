/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\01_Relocation_Group_Action.md
 * Line: 564
 * Language: c
 * Block ID: f6f66fc2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：重定位顺序影响结果
void wrong_reloc_order(Elf64_Rela *relocs, int n) {
    // 随机顺序应用重定位
    for (int i = 0; i < n; i++) {
        apply_reloc(&relocs[rand() % n]);  // ❌ 非确定性！
    }
}

// 正确：拓扑排序后按依赖顺序重定位
void correct_reloc_order(Elf64_Rela *relocs, int n) {
    // 构建依赖图
    Graph deps = build_dependency_graph(relocs, n);
    // 拓扑排序
    int *order = topological_sort(deps);
    // 按依赖顺序重定位
    for (int i = 0; i < n; i++) {
        apply_reloc(&relocs[order[i]]);
    }
}
