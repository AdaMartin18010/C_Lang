/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 359
 * Language: c
 * Block ID: 6ba43c99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 重定位信息输出工具

void print_relocation_info(const char *filename) {
    Elf64_Ehdr *ehdr = parse_elf(filename);

    printf("重定位信息：\n");
    printf("%-16s %-10s %-20s %-10s\n",
           "偏移", "类型", "符号", "加数");

    for (int i = 0; i < num_relocations(ehdr); i++) {
        Elf64_Rela *rel = get_relocation(ehdr, i);

        const char *type_name = relocation_type_name(ELF64_R_TYPE(rel->r_info));
        const char *sym_name = symbol_name(ehdr, ELF64_R_SYM(rel->r_info));

        printf("0x%014lx %-10s %-20s %10ld\n",
               rel->r_offset,
               type_name,
               sym_name,
               rel->r_addend);
    }
}

// 地址空间可视化
void visualize_address_space(Elf64_Ehdr *ehdr) {
    printf("地址空间布局：\n");
    printf("┌─────────────────────────────────────┐\n");

    uint64_t base = 0x400000;  // 典型Linux基址

    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = get_program_header(ehdr, i);
        if (phdr->p_type != PT_LOAD) continue;

        printf("│ 0x%012lx-0x%012lx ",
               phdr->p_vaddr,
               phdr->p_vaddr + phdr->p_memsz);

        // 权限指示
        printf("[%c%c%c] ",
               (phdr->p_flags & PF_R) ? 'R' : '-',
               (phdr->p_flags & PF_W) ? 'W' : '-',
               (phdr->p_flags & PF_X) ? 'X' : '-');

        // 段类型
        if (phdr->p_flags & PF_X) printf("代码\n");
        else if (phdr->p_flags & PF_W) printf("数据\n");
        else printf("只读\n");
    }

    printf("└─────────────────────────────────────┘\n");
}
