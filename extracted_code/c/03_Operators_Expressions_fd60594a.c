/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 313
 * Language: c
 * Block ID: fd60594a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// && 和 || 短路求值

// 如果ptr为NULL，不会解引用
if (ptr != NULL && ptr->value > 0) {
    // 安全访问
}

// 如果成功，不会调用error_handler
if (do_something() || error_handler()) {
    // do_something返回0才调用error_handler
}

// 利用短路实现条件执行
// 等效于 if (condition) action();
condition && action();

// 等效于 if (!condition) alternative();
condition || alternative();
