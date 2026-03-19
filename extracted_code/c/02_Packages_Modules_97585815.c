/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\02_Packages_Modules.md
 * Line: 53
 * Language: c
 * Block ID: 97585815
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// stack.h
#ifndef STACK_H
#define STACK_H
void push(int item);
int pop(void);
int is_empty(void);
#endif

// stack.c
#include "stack.h"
static int stack_array[100];  // "静态"但非真正隐藏
static int top = 0;

void push(int item) {
    if (top >= 100) { /* 溢出处理 */ }
    stack_array[top++] = item;
}

// main.c
#include "stack.h"
extern int top;  // 恶意代码可直接访问！C无法阻止
