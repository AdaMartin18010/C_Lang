/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\02_Arithmetic_Logic_Unit\03_ALU_Design.md
 * Line: 1394
 * Language: c
 * Block ID: 8442572a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 使用位运算代替乘除
int a = x * 8;        // 编译器优化为 x << 3
int b = x / 16;       // 编译器优化为 x >> 4 (无符号)

// 2. 避免数据依赖
// 低效 (数据依赖链):
int a = x + 1;
int b = a + 2;
int c = b + 3;

// 高效 (并行):
int a = x + 1;
int b = y + 2;
int c = z + 3;

// 3. 使用合适的整数类型
// 32位ALU上，32位整数最快
int32_t a = 100;      // 最优
int16_t b = 100;      // 可能需要扩展
int64_t c = 100;      // 需要多个周期

// 4. 注意整数溢出
// 未定义行为，可能被编译器优化掉
int max = INT_MAX;
int bad = max + 1;    // UB! 不要这样做

// 安全的写法:
int max = INT_MAX;
if (max < INT_MAX - 1) {
    int safe = max + 1;
}
