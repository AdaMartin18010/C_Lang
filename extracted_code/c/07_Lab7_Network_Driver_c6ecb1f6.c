/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 626
 * Language: c
 * Block ID: c6ecb1f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 打印寄存器状态
void e1000_debug(void) {
    printf("E1000 Status:\n");
    printf("  STATUS: %08x\n", e1000_regs[E1000_STATUS / 4]);
    printf("  RCTL:   %08x\n", e1000_regs[E1000_RCTL / 4]);
    printf("  TCTL:   %08x\n", e1000_regs[E1000_TCTL / 4]);
    printf("  RDH:    %d, RDT: %d\n",
           e1000_regs[E1000_RDH / 4],
           e1000_regs[E1000_RDT / 4]);
    printf("  TDH:    %d, TDT: %d\n",
           e1000_regs[E1000_TDH / 4],
           e1000_regs[E1000_TDT / 4]);
}
