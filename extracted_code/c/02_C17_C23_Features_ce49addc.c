/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 158
 * Language: c
 * Block ID: ce49addc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11/C17: 双下划线属性
void func(void) [[noreturn]];
int x [[deprecated("use y instead")]];

// C23: 单括号属性
void func(void) [noreturn];
int x [deprecated("use y instead")];

// 标准属性列表
[deprecated]          // 废弃
[fallthrough]         // switch fallthrough意图
[maybe_unused]        // 可能未使用
[nodiscard]           // 返回值不应忽略
[noreturn]            // 不返回
[unsequenced]         // 函数无状态、无副作用
[reproducible]        // 函数无状态

// 使用示例
[nodiscard] int important_func(void);
[deprecated("use new_func instead")] void old_func(void);

void process(int x) {
    switch (x) {
        case 1:
            init();
            [fallthrough];  // 意图明确
        case 2:
            process_main();
            break;
    }
}

[maybe_unused] static int debug_counter;
