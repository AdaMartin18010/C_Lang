/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 351
 * Language: c
 * Block ID: 37875cd2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 读取ELF文件并识别类型
void identify_elf(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    unsigned char ident[EI_NIDENT];
    fread(ident, 1, EI_NIDENT, fp);

    // ELF魔数: 0x7F 'E' 'L' 'F'
    if (ident[0] != 0x7F || ident[1] != 'E' ||
        ident[2] != 'L' || ident[3] != 'F') {
        printf("Not an ELF file\n");
        return;
    }

    // 字长
    printf("Class: %s\n",
        ident[4] == 1 ? "32-bit" :
        ident[4] == 2 ? "64-bit" : "Invalid");

    // 字节序
    printf("Data: %s\n",
        ident[5] == 1 ? "Little Endian" :
        ident[5] == 2 ? "Big Endian" : "Invalid");

    fclose(fp);
}
