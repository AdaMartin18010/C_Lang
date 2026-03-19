/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 196
 * Language: c
 * Block ID: fff3aa4b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 辅助调试的数据结构可视化

// 链表可视化
void list_visualize(ListNode *head) {
    printf("HEAD");
    for (ListNode *p = head; p; p = p->next) {
        printf(" -> [%d]", p->data);
    }
    printf(" -> NULL\n");
}

// 树的可视化（ASCII艺术）
void tree_visualize(TreeNode *root, int depth) {
    if (!root) return;

    tree_visualize(root->right, depth + 1);

    for (int i = 0; i < depth; i++) printf("    ");
    printf("|-- %d\n", root->value);

    tree_visualize(root->left, depth + 1);
}

// 内存布局可视化
void memory_visualize(void *ptr, size_t size) {
    unsigned char *p = ptr;
    printf("Address: %p, Size: %zu\n", ptr, size);
    printf("+--------+--------+--------+--------+\n");
    for (size_t i = 0; i < size; i++) {
        if (i % 16 == 0) printf("|");
        printf(" %02x", p[i]);
        if (i % 16 == 15) printf(" |\n");
    }
    printf("+--------+--------+--------+--------+\n");
}
