/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\08_RISCV_CPU_Complete\04_RISCV_Simulator_in_C.md
 * Line: 1152
 * Language: c
 * Block ID: dea31281
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C模拟器: 单周期执行 (简化)
void cpu_step(CPUT *cpu) {
    // 1. 取指 (内存读取)
    uint32_t inst = memory[cpu->pc];

    // 2. 译码 (软件解析)
    decode(inst, &decoded);

    // 3. 执行 (C运算)
    uint32_t result = alu(decoded.op, rs1, rs2);

    // 4. 访存 (条件执行)
    if (is_load) result = memory[addr];
    if (is_store) memory[addr] = rs2;

    // 5. 写回
    registers[decoded.rd] = result;
    cpu->pc += 4;
}
