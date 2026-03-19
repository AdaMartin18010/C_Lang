/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 320
 * Language: c
 * Block ID: 980342ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux内核链表: include/linux/list.h
// 使用二级指针简化链表头插入

struct list_head {
    struct list_head *next, *prev;
};

// 传统一级指针方式（需要处理头节点特殊情况）
void list_add_old(struct list_head *new, struct list_head *head) {
    struct list_head *next = head->next;
    next->prev = new;
    new->next = next;
    new->prev = head;
    head->next = new;
}

// 二级指针技巧: 遍历并删除节点
// include/linux/list.h: list_del_init

// 使用二级指针的链表删除（优雅处理头节点）
void list_remove_pp(struct list_head **head, struct list_head *entry) {
    struct list_head **indirect = head;

    while (*indirect != entry) {
        indirect = &(*indirect)->next;
    }

    *indirect = entry->next;  // 跳过entry节点
    entry->next = NULL;
    entry->prev = NULL;
}
