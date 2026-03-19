/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 205
 * Language: c
 * Block ID: 64692393
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 数组栈结构定义
 * 使用动态数组，支持自动扩容
 */
typedef struct {
    int* data;           // 存储元素的数组
    size_t capacity;     // 数组容量
    size_t top;          // 栈顶索引 (指向下一个空闲位置)
} ArrayStack;

#define STACK_INITIAL_CAPACITY 16
#define STACK_GROWTH_FACTOR 2

// 初始化栈
void array_stack_init(ArrayStack* stack) {
    stack->data = (int*)malloc(STACK_INITIAL_CAPACITY * sizeof(int));
    if (stack->data == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    stack->capacity = STACK_INITIAL_CAPACITY;
    stack->top = 0;
}

// 销毁栈
void array_stack_destroy(ArrayStack* stack) {
    free(stack->data);
    stack->data = nullptr;
    stack->capacity = 0;
    stack->top = 0;
}

// 检查是否需要扩容
static void array_stack_ensure_capacity(ArrayStack* stack) {
    if (stack->top >= stack->capacity) {
        size_t new_capacity = stack->capacity * STACK_GROWTH_FACTOR;
        int* new_data = (int*)realloc(stack->data, new_capacity * sizeof(int));
        if (new_data == nullptr) {
            fprintf(stderr, "Error: Failed to expand stack\n");
            exit(EXIT_FAILURE);
        }
        stack->data = new_data;
        stack->capacity = new_capacity;
        printf("Stack expanded: %zu -> %zu\n", stack->capacity / 2, stack->capacity);
    }
}

// 压栈
bool array_stack_push(ArrayStack* stack, int value) {
    array_stack_ensure_capacity(stack);
    stack->data[stack->top++] = value;
    return true;
}

// 出栈
bool array_stack_pop(ArrayStack* stack, int* out_value) {
    if (stack->top == 0) {
        fprintf(stderr, "Error: Stack underflow\n");
        return false;
    }
    *out_value = stack->data[--stack->top];
    return true;
}

// 查看栈顶
bool array_stack_peek(const ArrayStack* stack, int* out_value) {
    if (stack->top == 0) {
        fprintf(stderr, "Error: Stack is empty\n");
        return false;
    }
    *out_value = stack->data[stack->top - 1];
    return true;
}

// 判断空栈
static inline bool array_stack_is_empty(const ArrayStack* stack) {
    return stack->top == 0;
}

// 获取大小
static inline size_t array_stack_size(const ArrayStack* stack) {
    return stack->top;
}

// 打印栈
void array_stack_print(const ArrayStack* stack) {
    printf("Stack [size=%zu, capacity=%zu]: ", stack->top, stack->capacity);
    printf("bottom [");
    for (size_t i = 0; i < stack->top; i++) {
        printf("%d", stack->data[i]);
        if (i < stack->top - 1) printf(", ");
    }
    printf("] top\n");
}
