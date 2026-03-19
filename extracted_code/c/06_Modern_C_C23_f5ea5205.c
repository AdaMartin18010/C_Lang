/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 340
 * Language: c
 * Block ID: f5ea5205
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 指定底层类型 (类似C++)
enum Color : uint8_t {  // C23语法
    RED,
    GREEN,
    BLUE
};

// 向前声明
enum Status;  // C23允许

void func(enum Status s);  // 可以先声明函数

// 完整定义在后面
enum Status {
    OK,
    ERROR,
    PENDING
};

// 枚举常量类型
typedef enum {
    VAL = 42
} EnumType;

// VAL的类型是EnumType，不是int (C23)
