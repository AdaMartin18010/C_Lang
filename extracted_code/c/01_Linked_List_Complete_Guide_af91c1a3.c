/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 709
 * Language: c
 * Block ID: af91c1a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建双链表节点
[[nodiscard]]
DNode* dnode_create(int data) {
    DNode* node = (DNode*)malloc(sizeof(DNode));
    if (node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return nullptr;
    }
    node->data = data;
    node->prev = nullptr;
    node->next = nullptr;
    return node;
}

// 初始化双链表
void dlist_init(DoublyLinkedList* list) {
    list->head = nullptr;
    list->tail = nullptr;
    list->size = 0;
}

// 头部插入
bool dlist_prepend(DoublyLinkedList* list, int data) {
    DNode* new_node = dnode_create(data);
    if (new_node == nullptr) return false;

    if (list->head == nullptr) {
        // 空链表
        list->head = list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
    list->size++;
    return true;
}

// 尾部插入
bool dlist_append(DoublyLinkedList* list, int data) {
    DNode* new_node = dnode_create(data);
    if (new_node == nullptr) return false;

    if (list->tail == nullptr) {
        list->head = list->tail = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->size++;
    return true;
}

// 删除节点 (双链表优势: O(1)删除已知节点)
bool dlist_delete_node(DoublyLinkedList* list, DNode* node) {
    if (node == nullptr || list->size == 0) return false;

    // 更新前驱的后继
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;  // 删除头节点
    }

    // 更新后继的前驱
    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;  // 删除尾节点
    }

    free(node);
    list->size--;
    return true;
}

// 前向遍历
void dlist_print_forward(const DoublyLinkedList* list) {
    printf("DList Forward [size=%zu]: NULL <=> ", list->size);
    DNode* current = list->head;
    while (current != nullptr) {
        printf("%d", current->data);
        if (current->next != nullptr) {
            printf(" <=> ");
        }
        current = current->next;
    }
    printf(" <=> NULL\n");
}

// 后向遍历
void dlist_print_backward(const DoublyLinkedList* list) {
    printf("DList Backward [size=%zu]: NULL <=> ", list->size);
    DNode* current = list->tail;
    while (current != nullptr) {
        printf("%d", current->data);
        if (current->prev != nullptr) {
            printf(" <=> ");
        }
        current = current->prev;
    }
    printf(" <=> NULL\n");
}
