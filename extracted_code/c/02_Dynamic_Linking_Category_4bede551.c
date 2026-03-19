/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 221
 * Language: c
 * Block ID: 4bede551
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 环境变量控制
// LD_BIND_NOW=1：立即绑定所有符号
// LD_BIND_NOT：不绑定（用于调试）

// 立即绑定实现
void immediate_binding(struct link_map *l) {
    Elf64_Rela *plt_rel = (Elf64_Rela *)l->l_info[DT_JMPREL];
    size_t count = l->l_info[DT_PLTRELSZ] / sizeof(Elf64_Rela);

    for (size_t i = 0; i < count; i++) {
        _dl_runtime_resolve(l, i);
    }
}

// 延迟绑定优势：
// 1. 启动时间更快（只绑定实际使用的符号）
// 2. 内存效率（未使用函数不占用GOT空间）
// 3. 支持符号插入（interposition）
