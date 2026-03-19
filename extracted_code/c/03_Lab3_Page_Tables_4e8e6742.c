/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 47
 * Language: c
 * Block ID: 4e8e6742
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/vm.c

// 递归辅助函数
void vmprint_helper(pagetable_t pagetable, int level) {
    // 遍历512个PTE
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];

        // 只打印有效的PTE
        if (pte & PTE_V) {
            // 打印缩进
            for (int j = 0; j < level; j++) {
                printf(" ..");
            }

            // 打印PTE信息
            uint64 child = PTE2PA(pte);
            printf("%d: pte %p pa %p\n", i, pte, child);

            // 如果不是叶子节点(RWX=0)，递归打印
            if ((pte & (PTE_R | PTE_W | PTE_X)) == 0) {
                // 这是一个指向下一级页表的PTE
                vmprint_helper((pagetable_t)child, level + 1);
            }
        }
    }
}

// 主函数
void vmprint(pagetable_t pagetable) {
    printf("page table %p\n", pagetable);
    vmprint_helper(pagetable, 0);
}
