/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 85
 * Language: c
 * Block ID: 77eb532d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/exec.c
int exec(char *path, char **argv) {
    // ... 原有代码 ...

    // 添加在返回前
    if (p->pid == 1) {
        vmprint(p->pagetable);
    }

    return argc;
}
