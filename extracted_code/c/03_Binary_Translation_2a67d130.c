/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Binary_Translation.md
 * Line: 309
 * Language: c
 * Block ID: 2a67d130
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Pin工具示例：指令计数
#include "pin.H"

UINT64 ins_count = 0;

// 每条指令执行前的回调
VOID PIN_FAST_ANALYSIS_CALL docount(UINT32 c) {
    ins_count += c;
}

// 指令级别的插桩
VOID Instruction(INS ins, VOID* v) {
    // 在每个基本块前插入计数代码
    INS_InsertCall(ins, IPOINT_BEFORE,
                   (AFUNPTR)docount,
                   IARG_FAST_ANALYSIS_CALL,
                   IARG_UINT32, 1,
                   IARG_END);
}

// 程序结束时输出结果
VOID Fini(INT32 code, VOID* v) {
    fprintf(stderr, "指令计数: %llu\n", ins_count);
}

int main(int argc, char* argv[]) {
    PIN_Init(argc, argv);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
}
