/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\00_MIT_Labs_Overview.md
 * Line: 113
 * Language: c
 * Block ID: 3fa29ff8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysproc.c
uint64 sys_trace(void) {
    int mask;
    if (argint(0, &mask) < 0)
        return -1;
    myproc()->trace_mask = mask;
    return 0;
}

// kernel/syscall.c
void syscall(void) {
    int num = p->trapframe->a7;
    p->trapframe->a0 = syscalls[num]();

    // 新增: 如果设置了trace，打印系统调用
    if ((p->trace_mask >> num) & 1) {
        printf("%d: syscall %s -> %d\n",
               p->pid, syscall_names[num], p->trapframe->a0);
    }
}
