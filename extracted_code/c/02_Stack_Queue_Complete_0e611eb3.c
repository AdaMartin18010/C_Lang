/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 454
 * Language: c
 * Block ID: 0e611eb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 浏览器前进后退功能实现
 * 使用两个栈: back_stack 和 forward_stack
 */
typedef struct {
    LinkedStack back_stack;      // 后退栈
    LinkedStack forward_stack;   // 前进栈
    char* current_page;          // 当前页面
} BrowserHistory;

void browser_init(BrowserHistory* browser, const char* homepage) {
    linked_stack_init(&browser->back_stack);
    linked_stack_init(&browser->forward_stack);
    browser->current_page = strdup(homepage);
}

void browser_visit(BrowserHistory* browser, const char* url) {
    // 访问新页面: 当前页入后退栈，清空前进栈
    linked_stack_push(&browser->back_stack, (int)(intptr_t)browser->current_page);

    // 清空前进栈
    while (!linked_stack_is_empty(&browser->forward_stack)) {
        int temp;
        linked_stack_pop(&browser->forward_stack, &temp);
        free((char*)(intptr_t)temp);
    }

    browser->current_page = strdup(url);
    printf("Visit: %s\n", url);
}

const char* browser_back(BrowserHistory* browser) {
    if (linked_stack_is_empty(&browser->back_stack)) {
        printf("No back history\n");
        return browser->current_page;
    }

    // 当前页入前进栈
    linked_stack_push(&browser->forward_stack, (int)(intptr_t)browser->current_page);

    // 后退栈出栈
    int prev;
    linked_stack_pop(&browser->back_stack, &prev);
    browser->current_page = (char*)(intptr_t)prev;

    printf("Back to: %s\n", browser->current_page);
    return browser->current_page;
}

const char* browser_forward(BrowserHistory* browser) {
    if (linked_stack_is_empty(&browser->forward_stack)) {
        printf("No forward history\n");
        return browser->current_page;
    }

    // 当前页入后退栈
    linked_stack_push(&browser->back_stack, (int)(intptr_t)browser->current_page);

    // 前进栈出栈
    int next;
    linked_stack_pop(&browser->forward_stack, &next);
    browser->current_page = (char*)(intptr_t)next;

    printf("Forward to: %s\n", browser->current_page);
    return browser->current_page;
}
