/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\04_Modern_C_Quick_Reference.md
 * Line: 21
 * Language: c
 * Block ID: 86d63a30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 属性语法
[[nodiscard]] int important_func(void);  // 忽略返回值警告
[[maybe_unused]] int x;                   // 可能未使用
[[noreturn]] void abort(void);            // 不返回
[[deprecated("使用 new_func 替代")]] void old_func(void);
