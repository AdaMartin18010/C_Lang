/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 259
 * Language: c
 * Block ID: c26c4b52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码中的显式栈帧管理（GCC）
void explicit_frame_function(void) {
    register void *rbp asm("rbp");
    register void *rsp asm("rsp");

    void *old_rbp = rbp;
    void *old_rsp = rsp;

    // 手动分配栈空间
    char local_buffer[256];

    // 使用局部变量
    snprintf(local_buffer, sizeof(local_buffer), "RBP=%p, RSP=%p",
             old_rbp, old_rsp);

    // 自动恢复
}
