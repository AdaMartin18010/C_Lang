/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 243
 * Language: c
 * Block ID: 8131639c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// true/false 成为关键字
bool flag = true;   // 不再需要stdbool.h

// 枚举改进
enum Color {
    RED   = 0xFF0000,
    GREEN = 0x00FF00,
    BLUE  = 0x0000FF,
};
// 枚举底层类型可以指定（实现定义）

// 改进的变长数组
// VLA仍然是可选的，但支持得更好

// _BitInt - 任意宽度整数
_BitInt(17) b17;   // 17位整数
unsigned _BitInt(128) u128;  // 128位无符号整数

// 空初始化器
int arr[10] = {};  // 全部初始化为0
