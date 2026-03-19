/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\00_MIT_Labs_Overview.md
 * Line: 142
 * Language: c
 * Block ID: 5d2b567b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/vm.c
void vmprint(pagetable_t pagetable) {
    printf("page table %p\n", pagetable);
    vmprint_helper(pagetable, 0);
}

void vmprint_helper(pagetable_t pagetable, int level) {
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if (pte & PTE_V) {
            printf(".. ..%d: pte %p pa %p\n", i, pte, PTE2PA(pte));
            if ((pte & (PTE_R|PTE_W|PTE_X)) == 0) {
                vmprint_helper((pagetable_t)PTE2PA(pte), level + 1);
            }
        }
    }
}
