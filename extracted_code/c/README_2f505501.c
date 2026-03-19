/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 460
 * Language: c
 * Block ID: 2f505501
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void disassemble_text(const char *filename) {
    FILE *fp = fopen(filename, "rb");

    // 读取ELF头
    Elf64_Ehdr ehdr;
    fread(&ehdr, sizeof(ehdr), 1, fp);

    // 读取节头表
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr *shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    fread(shdrs, ehdr.e_shentsize, ehdr.e_shnum, fp);

    // 读取节名字符串表
    Elf64_Shdr *shstrtab = &shdrs[ehdr.e_shstrndx];
    char *names = malloc(shstrtab->sh_size);
    fseek(fp, shstrtab->sh_offset, SEEK_SET);
    fread(names, 1, shstrtab->sh_size, fp);

    // 查找.text节
    for (int i = 0; i < ehdr.e_shnum; i++) {
        if (strcmp(names + shdrs[i].sh_name, ".text") == 0) {
            // 读取代码
            uint8_t *code = malloc(shdrs[i].sh_size);
            fseek(fp, shdrs[i].sh_offset, SEEK_SET);
            fread(code, 1, shdrs[i].sh_size, fp);

            // 反汇编
            disassemble(code, shdrs[i].sh_size,
                       shdrs[i].sh_addr);

            free(code);
            break;
        }
    }

    free(names);
    free(shdrs);
    fclose(fp);
}
