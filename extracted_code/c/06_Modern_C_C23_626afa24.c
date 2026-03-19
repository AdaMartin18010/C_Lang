/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 127
 * Language: c
 * Block ID: 626afa24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 旧语法 (GCC扩展)
__attribute__((noreturn)) void exit(int);

// C23标准语法
[[noreturn]] void exit(int);

// 更多属性
[[nodiscard]] int open_file(const char *path);
[[maybe_unused]] int debug_var = 0;
[[deprecated("Use new_func instead")]] void old_func(void);

// 多个属性
[[nodiscard, deprecated]] int legacy_api(void);
