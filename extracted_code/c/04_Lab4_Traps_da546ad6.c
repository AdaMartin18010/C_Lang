/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 708
 * Language: c
 * Block ID: da546ad6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 在usertrap中添加调试输出
void usertrap() {
    uint64 cause = r_scause();
    printf("trap: cause=%p epc=%p stval=%p\n",
           cause, r_sepc(), r_stval());
    // ...
}
