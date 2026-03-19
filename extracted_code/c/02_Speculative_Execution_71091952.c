/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 464
 * Language: c
 * Block ID: 71091952
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GCC/Clang防护选项

// -mspeculative-load-hardening
// 自动为所有加载添加推测屏障

// -mindirect-branch=thunk
// 使用跳转thunk防止Spectre V2

// -mindirect-branch-loop
// 禁用间接分支的推测执行

// 示例：使用编译器属性防护特定函数
__attribute__((speculative_load_hardening))
void critical_function(const uint8_t *data, size_t len, size_t offset) {
    if (offset < len) {
        // 编译器自动插入防护代码
        process(data[offset]);
    }
}
