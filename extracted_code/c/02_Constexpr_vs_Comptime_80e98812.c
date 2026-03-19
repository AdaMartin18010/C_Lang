/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 1776
 * Language: c
 * Block ID: 80e98812
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 有限的反射能力
#include <stdio.h>
#include <stddef.h>

// ========== 基本类型信息 ==========

// 类型大小和对齐
constexpr size_t INT_SIZE = sizeof(int);
constexpr size_t INT_ALIGN = alignof(int);

// offsetof 获取字段偏移
struct Point {
    float x;
    float y;
    float z;
};

constexpr size_t OFFSET_X = offsetof(struct Point, x);
constexpr size_t OFFSET_Y = offsetof(struct Point, y);

// ========== 类型特征（C23 新增）=============

// typeof 获取表达式类型
int x = 10;
typeof(x) y = 20;  // y 是 int
typeof_unqual(const int) z = 30;  // z 是 int（去掉 const）

// ❌ C23 无法遍历结构体字段
// ❌ C23 无法获取字段名称
// ❌ C23 无法查询类型是否有特定方法

// ========== 手动反射（宏技巧）=============

// 手动定义反射信息
#define REFLECT_FIELD(type, name) { #name, offsetof(type, name), sizeof(((type*)0)->name) }

struct FieldInfo {
    const char* name;
    size_t offset;
    size_t size;
};

// 手动列出所有字段
const struct FieldInfo POINT_FIELDS[] = {
    REFLECT_FIELD(struct Point, x),
    REFLECT_FIELD(struct Point, y),
    REFLECT_FIELD(struct Point, z),
};

// 使用反射信息
void print_point_fields(void) {
    for (size_t i = 0; i < sizeof(POINT_FIELDS)/sizeof(POINT_FIELDS[0]); i++) {
        printf("Field %s: offset=%zu, size=%zu\n",
               POINT_FIELDS[i].name,
               POINT_FIELDS[i].offset,
               POINT_FIELDS[i].size);
    }
}
