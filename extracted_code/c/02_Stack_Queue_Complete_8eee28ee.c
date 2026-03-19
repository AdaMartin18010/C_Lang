/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1681
 * Language: c
 * Block ID: 8eee28ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 后缀表达式求值
 *
 * 算法步骤:
 * 1. 遇到操作数，压栈
 * 2. 遇到运算符，弹出两个操作数，计算后压栈
 * 3. 最后栈中只剩结果
 */
int evaluate_postfix(const char* postfix) {
    LinkedStack stack;
    linked_stack_init(&stack);

    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];

        if (isdigit(c)) {
            // 操作数压栈
            linked_stack_push(&stack, c - '0');
        } else if (is_operator(c)) {
            // 运算符：弹出两个操作数
            int b, a;
            linked_stack_pop(&stack, &b);
            linked_stack_pop(&stack, &a);

            int result;
            switch (c) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': result = a / b; break;
                default: result = 0;
            }
            linked_stack_push(&stack, result);
        }
    }

    int final_result;
    linked_stack_pop(&stack, &final_result);
    linked_stack_destroy(&stack);
    return final_result;
}
