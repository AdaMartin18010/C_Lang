/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1022
 * Language: c
 * Block ID: 361de33f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 _Generic: 需要表达式
typeof(int) x = _Generic(0, int: 1, default: 0);  // 使用字面量

// C2y提案 N3260: 支持类型名
typeof(int) y = _Generic(int, int: 1, default: 0);  // 直接使用类型

// 用途: 创建类型特征
#define IS_INT(T) _Generic(T, int: true, default: false)

// 示例: 类型安全的通用打印
#define print_value(x) _Generic((x), \
    int: print_int, \
    double: print_double, \
    char*: print_string \
)(x)

// C2y: 基于类型的泛型函数选择
#define type_trait(T) _Generic(T, \
    int: int_traits, \
    float: float_traits, \
    default: generic_traits \
)
