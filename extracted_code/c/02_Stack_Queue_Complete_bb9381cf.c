/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 304
 * Language: c
 * Block ID: bb9381cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 链表栈节点定义
 */
typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;      // 栈顶指针
    size_t size;
} LinkedStack;

// 初始化
void linked_stack_init(LinkedStack* stack) {
    stack->top = nullptr;
    stack->size = 0;
}

// 销毁
void linked_stack_destroy(LinkedStack* stack) {
    StackNode* current = stack->top;
    while (current != nullptr) {
        StackNode* temp = current;
        current = current->next;
        free(temp);
    }
    stack->top = nullptr;
    stack->size = 0;
}

// 压栈 - 链表头部插入
bool linked_stack_push(LinkedStack* stack, int value) {
    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));
    if (new_node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return false;
    }
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
    return true;
}

// 出栈 - 链表头部删除
bool linked_stack_pop(LinkedStack* stack, int* out_value) {
    if (stack->top == nullptr) {
        fprintf(stderr, "Error: Stack underflow\n");
        return false;
    }
    StackNode* temp = stack->top;
    *out_value = temp->data;
    stack->top = temp->next;
    free(temp);
    stack->size--;
    return true;
}

// 查看栈顶
bool linked_stack_peek(const LinkedStack* stack, int* out_value) {
    if (stack->top == nullptr) {
        fprintf(stderr, "Error: Stack is empty\n");
        return false;
    }
    *out_value = stack->top->data;
    return true;
}

// 打印栈
void linked_stack_print(const LinkedStack* stack) {
    printf("LinkedStack [size=%zu]: top -> ", stack->size);
    StackNode* current = stack->top;
    while (current != nullptr) {
        printf("[%d]", current->data);
        if (current->next != nullptr) printf(" -> ");
        current = current->next;
    }
    printf(" -> NULL\n");
}
