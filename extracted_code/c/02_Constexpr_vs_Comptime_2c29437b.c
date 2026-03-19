/*
 * Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
 * Line: 153
 * Language: c
 * Block ID: 2c29437b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: constexpr 常量声明
#include <stdio.h>
#include <stdint.h>

// 基本常量
constexpr int MAX_SIZE = 1024;
constexpr double PI = 3.14159265358979323846;

// 常量表达式计算
constexpr int BUFFER_SIZE = MAX_SIZE * 2;
constexpr int64_t MASK = (1LL << 32) - 1;

// 数组大小（必须是常量表达式）
int buffer[BUFFER_SIZE];  // OK: BUFFER_SIZE 是 constexpr

// 枚举式常量
constexpr uint32_t FLAGS_READ = 0x01;
constexpr uint32_t FLAGS_WRITE = 0x02;
constexpr uint32_t FLAGS_EXEC = 0x04;
