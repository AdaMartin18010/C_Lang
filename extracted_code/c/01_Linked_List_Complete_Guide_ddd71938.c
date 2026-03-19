/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 852
 * Language: c
 * Block ID: ddd71938
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    Node* tail;      // 循环链表通常只维护尾指针
    size_t size;
} CircularList;

// 初始化
void clist_init(CircularList* list) {
    list->tail = nullptr;
    list->size = 0;
}

// 头部插入
bool clist_prepend(CircularList* list, int data) {
    Node* new_node = create_node(data);
    if (new_node == nullptr) return false;

    if (list->tail == nullptr) {
        // 空链表
        new_node->next = new_node;  // 指向自己
        list->tail = new_node;
    } else {
        new_node->next = list->tail->next;  // 指向原来的头
        list->tail->next = new_node;        // 尾节点指向新头
    }
    list->size++;
    return true;
}

// 尾部插入
bool clist_append(CircularList* list, int data) {
    if (!clist_prepend(list, data)) return false;
    list->tail = list->tail->next;  // 更新尾指针
    return true;
}

// 打印循环链表
void clist_print(const CircularList* list) {
    if (list->tail == nullptr) {
        printf("CircularList [size=0]: empty\n");
        return;
    }

    printf("CircularList [size=%zu]: ", list->size);
    Node* head = list->tail->next;
    Node* current = head;

    do {
        printf("%d", current->data);
        current = current->next;
        if (current != head) {
            printf(" -> ");
        }
    } while (current != head);

    printf(" -> (back to head: %d)\n", head->data);
}

// 约瑟夫问题求解
/**
 * @brief 约瑟夫问题
 * @param n 总人数
 * @param m 报数到m的人出列
 * @return 最后幸存者的编号 (1-based)
 */
int josephus(int n, int m) {
    if (n <= 0 || m <= 0) return -1;

    // 构建循环链表
    CircularList list;
    clist_init(&list);

    for (int i = 1; i <= n; i++) {
        clist_append(&list, i);
    }

    Node* current = list.tail->next;  // 从第一个开始

    while (list.size > 1) {
        // 报数 m-1 次 (当前是第一个)
        for (int i = 1; i < m - 1; i++) {
            current = current->next;
        }

        // 删除下一个节点
        Node* to_delete = current->next;
        current->next = to_delete->next;

        if (list.tail == to_delete) {
            list.tail = current;  // 更新尾指针
        }

        printf("Eliminated: %d\n", to_delete->data);
        free(to_delete);
        list.size--;

        current = current->next;  // 从下一个人继续
    }

    int survivor = current->data;
    free(current);
    return survivor;
}
