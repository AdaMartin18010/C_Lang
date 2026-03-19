/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 489
 * Language: c
 * Block ID: 23a0cd4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 位置无关可执行文件(PIE)

// PIE允许ASLR（地址空间布局随机化）
// 编译选项：-fPIE -pie

// PIE代码示例
void pie_example(void) {
    // 访问全局变量
    extern int global_var;

    // 编译器生成（x86-64）：
    // lea rax, [rip + global_var@GOTPCREL]
    // mov rax, [rax]
    // mov eax, [rax]

    int x = global_var;
}

// 运行时重定位类型
enum DynamicRelocation {
    R_X86_64_RELATIVE = 8,   // 基址+偏移
    R_X86_64_GLOB_DAT = 6,   // 全局数据表
    R_X86_64_JUMP_SLOT = 7,  // PLT跳转槽
};

// 动态链接器重定位循环
void apply_dynamic_relocations(Elf64_Dyn *dynamic, uint64_t base) {
    Elf64_Rela *rel = (Elf64_Rela *)get_dynamic_tag(dynamic, DT_RELA);
    size_t count = get_dynamic_tag(dynamic, DT_RELASZ) / sizeof(Elf64_Rela);

    for (size_t i = 0; i < count; i++) {
        uint64_t *target = (uint64_t *)(base + rel[i].r_offset);

        switch (ELF64_R_TYPE(rel[i].r_info)) {
            case R_X86_64_RELATIVE:
                *target = base + rel[i].r_addend;
                break;
            case R_X86_64_GLOB_DAT:
            case R_X86_64_JUMP_SLOT:
                // 运行时解析符号
                *target = resolve_symbol(dynamic, rel[i]);
                break;
        }
    }
}
