/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 433
 * Language: c
 * Block ID: 36f3bf29
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// defer: 延迟执行语句直到当前作用域退出

defer statement;

// 语义:
// - statement在当前作用域结束时执行
// - 无论作用域如何退出(正常、return、break、goto)
// - 多个defer按LIFO顺序执行(栈行为)
// - defer本身不产生新作用域
