/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 514
 * Language: c
 * Block ID: 7c391fff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// nodiscard: 忽略返回值时警告
[[nodiscard]] int allocate_resource(void);

// maybe_unused: 抑制未使用警告
[[maybe_unused]] int debug_var = 0;

// noreturn: 函数不返回
[[noreturn]] void fatal_error(const char *msg);

// deprecated: 标记废弃
[[deprecated("Use new_api() instead")]] void old_api(void);

// fallthrough: 有意case穿透
switch (x) {
    case 1:
        do_1();
        [[fallthrough]];
    case 2:
        do_2();
        break;
}

// likely/unlikely: 分支预测提示
if ([[likely]] condition) {
    // 预期经常为真
} else {
    // 预期很少执行
}

// unsequenced: 函数调用顺序无关
[[unsequenced]] int pure_func(int x);

// reproducible: 可重现结果
[[reproducible]] double compute(double x);
