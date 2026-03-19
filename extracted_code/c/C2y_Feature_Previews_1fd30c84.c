/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 333
 * Language: c
 * Block ID: 1fd30c84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ GCC扩展 ============
// GCC从4.x开始支持 a ?: b 作为扩展

// GCC行为 (扩展)
int gcc_result = value ?: default_value;
// - 支持所有标量类型
// - 对指针: 检查 != NULL
// - 对整数: 检查 != 0
// - 对浮点: 检查 != 0.0
// - 仅支持简单表达式 (某些复杂情况有问题)

// 编译标志
// gcc -std=gnu11 -Wall  (默认启用)
// gcc -std=c11 -pedantic  (警告扩展)

// ============ C2y提案标准化 ============
// 目标是标准化GCC的行为，但可能有细微调整

// 预期C2y行为
int std_result = value ?: default_value;
// - 与GCC类似，但精确定义
// - 可能的差异:

// 潜在差异点1: 求值顺序
// GCC: 严格从左到右，只求值一次
// C2y: 确保与三元运算符一致的求值

// 潜在差异点2: 类型转换
// GCC: 遵循通常算术转换
// C2y: 可能更严格的类型要求

// 代码示例展示差异场景
void comparison_demo(void) {
    int a = 0;
    int b = 5;

    // 场景1: 副作用
    int x = (a++ ?: b);  // a变为1，返回b(5)
    // GCC: a++求值一次，结果为0，返回b

    // 场景2: 复杂表达式
    int* ptr = NULL;
    int* result = (get_pointer() ?: &default_int);
    // 需要确保get_pointer()只调用一次
}
