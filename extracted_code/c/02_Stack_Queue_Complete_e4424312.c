/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1596
 * Language: c
 * Block ID: e4424312
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 中缀表达式转后缀表达式 (Shunting Yard算法)
 *
 * 算法步骤:
 * 1. 遇到操作数，直接输出
 * 2. 遇到左括号，压栈
 * 3. 遇到右括号，弹出栈顶直到左括号
 * 4. 遇到运算符，弹出优先级≥当前的所有运算符，然后压栈
 * 5. 最后弹出栈中所有运算符
 */

// 获取运算符优先级
int get_priority(char op) {
    switch (op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/': return 2;
        case '^': return 3;
        default: return 0;
    }
}

// 判断是否为运算符
bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// 中缀转后缀
void infix_to_postfix(const char* infix, char* postfix) {
    LinkedStack stack;
    linked_stack_init(&stack);

    int j = 0;
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];

        if (isalnum(c)) {
            // 操作数直接输出
            postfix[j++] = c;
        } else if (c == '(') {
            // 左括号压栈
            linked_stack_push(&stack, c);
        } else if (c == ')') {
            // 右括号：弹出到左括号
            while (!linked_stack_is_empty(&stack)) {
                int top;
                linked_stack_peek(&stack, &top);
                if (top == '(') {
                    linked_stack_pop(&stack, &top);
                    break;
                }
                linked_stack_pop(&stack, &top);
                postfix[j++] = (char)top;
            }
        } else if (is_operator(c)) {
            // 运算符
            while (!linked_stack_is_empty(&stack)) {
                int top;
                linked_stack_peek(&stack, &top);
                if (top == '(' || get_priority((char)top) < get_priority(c)) {
                    break;
                }
                linked_stack_pop(&stack, &top);
                postfix[j++] = (char)top;
            }
            linked_stack_push(&stack, c);
        }
    }

    // 弹出剩余运算符
    while (!linked_stack_is_empty(&stack)) {
        int top;
        linked_stack_pop(&stack, &top);
        postfix[j++] = (char)top;
    }

    postfix[j] = '\0';
    linked_stack_destroy(&stack);
}
