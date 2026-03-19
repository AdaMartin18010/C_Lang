/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 582
 * Language: c
 * Block ID: 9cb66b6e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 打印页表信息
void debug_pte(pagetable_t pagetable, uint64 va) {
    pte_t *pte = walk(pagetable, va, 0);
    if (pte) {
        printf("PTE at va=%p: %p (flags: ", va, *pte);
        if (*pte & PTE_V) printf("V ");
        if (*pte & PTE_R) printf("R ");
        if (*pte & PTE_W) printf("W ");
        if (*pte & PTE_X) printf("X ");
        if (*pte & PTE_U) printf("U ");
        if (*pte & PTE_COW) printf("COW ");
        printf(")\n");
    }
}

// 打印引用计数
void debug_ref(uint64 pa) {
    printf("ref_count[%p] = %d\n", pa, get_ref(pa));
}
