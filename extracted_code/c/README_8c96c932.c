/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\README.md
 * Line: 242
 * Language: c
 * Block ID: 8c96c932
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用capstone反汇编库
#include <capstone/capstone.h>

void disassemble(const uint8_t *code, size_t size, uint64_t addr) {
    csh handle;
    cs_insn *insn;
    size_t count;

    // 初始化: x86-64架构
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return;

    // 启用详细输出
    cs_option(handle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);

    // 反汇编
    count = cs_disasm(handle, code, size, addr, 0, &insn);

    for (size_t i = 0; i < count; i++) {
        printf("0x%lx: %s %s\n",
               insn[i].address,
               insn[i].mnemonic,
               insn[i].op_str);
    }

    cs_free(insn, count);
    cs_close(&handle);
}
