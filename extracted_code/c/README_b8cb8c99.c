/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 72
 * Language: c
 * Block ID: b8cb8c99
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数据冒险示例: RAW (Read After Write)
int a = 10;      // I1: 写入 a
int b = a + 5;   // I2: 读取 a → 依赖 I1

// 结构冒险: 多个指令同时需要同一硬件资源
// 控制冒险: 分支指令导致流水线冲刷

// 现代 CPU 的乱序执行可以解决数据冒险
void data_hazard_demo() {
    int x = 1;           // 周期 1
    int y = x + 2;       // 周期 2, 依赖 x
    int z = 3;           // 周期 2, 可乱序提前执行!
    int w = y + z;       // 周期 3+, 依赖 y 和 z
}
