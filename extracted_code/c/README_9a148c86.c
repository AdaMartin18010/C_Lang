/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 338
 * Language: c
 * Block ID: 9a148c86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 主要修复：
 * 1. _Static_assert 可以在任何作用域使用
 * 2. 放宽了 restrict 关键字的某些限制
 * 3. 修复了泛型选择的一些边界情况
 * 4. 线程和原子操作的若干修正
 */

/* _Static_assert 现在可以在函数内部使用 */
void check_sizes(void) {
    _Static_assert(sizeof(void*) == 8, "64-bit only");
}
