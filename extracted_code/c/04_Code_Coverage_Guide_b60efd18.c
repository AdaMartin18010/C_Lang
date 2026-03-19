/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\04_Code_Coverage_Guide.md
 * Line: 518
 * Language: c
 * Block ID: b60efd18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int calculate(int a, int b, int op) {   // 函数入口
    int result = 0;                      // 行覆盖率: 1

    if (op == 1) {                       // 分支1: true/false
        result = a + b;                  // 行覆盖率: ?
    } else if (op == 2) {                // 分支2: true/false
        result = a - b;                  // 行覆盖率: ?
    } else if (op == 3 && b != 0) {      // 条件: 4种组合
        result = a / b;                  // 行覆盖率: ?
    } else {
        result = -1;                     // 默认分支
    }

    return result;                       // 行覆盖率: 1
}
