/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 326
 * Language: c
 * Block ID: 451c0179
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 标准属性语法
[[nodiscard]] int important_function(void);
[[deprecated("Use new_function instead")]] void old_function(void);
[[maybe_unused]] int unused_var = 0;
[[noreturn]] void abort_program(void);

// 属性与函数结合
[[nodiscard, gnu::pure]] int calculate(int x);

// 属性在声明中的应用
struct [[packed]] PacketHeader {
    uint16_t magic;
    uint32_t length;
};

// 分支预测提示
[[likely]] if (condition) {
    // 预期经常执行
}

[[unlikely]] if (error_condition) {
    // 预期很少执行
}

// 回退到 GCC/Clang 扩展
#ifndef __STDC_VERSION__
    #define FALLTHROUGH __attribute__((fallthrough))
    #define NODISCARD   __attribute__((warn_unused_result))
#else
    #define FALLTHROUGH [[fallthrough]]
    #define NODISCARD   [[nodiscard]]
#endif
