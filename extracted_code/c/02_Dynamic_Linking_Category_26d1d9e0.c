/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 173
 * Language: c
 * Block ID: 26d1d9e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 延迟绑定解析器（简化）

// 运行时解析函数
Elf64_Addr _dl_runtime_resolve(
    struct link_map *l,     // 调用模块
    Elf64_Word reloc_index  // 重定位索引
) {
    // 1. 获取重定位条目
    Elf64_Rela *rel = &l->l_info[DT_JMPREL] + reloc_index;

    // 2. 获取符号
    Elf64_Sym *sym = &l->l_info[DT_SYMTAB][ELF64_R_SYM(rel->r_info)];
    const char *sym_name = l->l_info[DT_STRTAB] + sym->st_name;

    // 3. 查找符号定义
    struct link_map *definer;
    Elf64_Sym *def = lookup_symbol(sym_name, l, &definer);

    // 4. 计算最终地址
    Elf64_Addr value = definer->l_addr + def->st_value;

    // 5. 更新GOT条目
    Elf64_Addr *got_entry = (Elf64_Addr *)(l->l_addr + rel->r_offset);
    *got_entry = value;

    // 6. 跳转到目标
    return value;
}

// 符号查找（全局范围）
Elf64_Sym *lookup_symbol(const char *name,
                         struct link_map *start,
                         struct link_map **definer) {
    // 按加载顺序搜索
    for (struct link_map *l = start; l; l = l->l_next) {
        Elf64_Sym *sym = lookup_in_module(name, l);
        if (sym) {
            *definer = l;
            return sym;
        }
    }
    return NULL;
}
