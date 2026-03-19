/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 674
 * Language: c
 * Block ID: 1a0567ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 危险: 多级野指针
void dangling_pp_example(void) {
    int **pp;

    {
        int x = 42;
        int *p = &x;
        pp = &p;    // pp指向局部变量p
    }              // p和x都被销毁

    // pp现在是野指针！
    printf("%d\n", **pp);  // ❌ 未定义行为！可能崩溃或打印垃圾值
}

// ❌ 危险: 悬挂指针链
struct node {
    int value;
    struct node **prev_next;  // 指向前一个节点的next指针
};

void remove_node_dangerous(struct node *n) {
    // 未检查n->prev_next是否有效
    *n->prev_next = NULL;  // ❌ 如果n->prev_next是野指针，崩溃！
    free(n);
}

// ✅ 安全做法
void remove_node_safe(struct node *n) {
    if (n && n->prev_next) {
        *n->prev_next = n->next;  // 正确链接
        n->next->prev_next = n->prev_next;  // 更新反向指针
        free(n);
    }
}
