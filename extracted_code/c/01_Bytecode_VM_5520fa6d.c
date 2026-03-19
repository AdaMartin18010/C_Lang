/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 463
 * Language: c
 * Block ID: 5520fa6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未检查边界
uint8_t op = vm->code[vm->pc++];  // 可能越界

// ✅ 边界检查
if (vm->pc >= vm->code_size) {
    fprintf(stderr, "PC out of bounds\n");
    return;
}
uint8_t op = vm->code[vm->pc++];
