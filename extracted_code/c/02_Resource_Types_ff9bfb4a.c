/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 325
 * Language: c
 * Block ID: ff9bfb4a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 生命周期参数模拟
// 'a 表示生命周期的约束

// fn longest<'a>(x: &'a str, y: &'a str) -> &'a str
typedef struct {
    const char *data;
    size_t len;
    int lifetime_id;  // 模拟生命周期
} StrRef;

StrRef longest(StrRef x, StrRef y) {
    // 返回值的生命周期与x和y的交集相同
    if (x.len > y.len) {
        return x;
    } else {
        return y;
    }
}

// 生命周期检查
bool check_lifetime(StrRef result, StrRef input1, StrRef input2) {
    // 结果的生命周期不能超过输入
    return result.lifetime_id <= input1.lifetime_id &&
           result.lifetime_id <= input2.lifetime_id;
}
