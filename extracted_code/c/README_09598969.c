/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\README.md
 * Line: 467
 * Language: c
 * Block ID: 09598969
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：计算圈复杂度

// 复杂度 = 1 (基本路径)
int simple_function(int x) {
    return x * 2;
}

// 复杂度 = 3 (1 + if + else)
int medium_complexity(int x, int y) {
    if (x > 0) {
        if (y > 0) {
            return x + y;
        }
        return x;
    }
    return 0;
}

// 复杂度 = 5 (1 + switch 4 cases)
int high_complexity(int cmd) {
    switch (cmd) {
        case 1: return 10;
        case 2: return 20;
        case 3: return 30;
        case 4: return 40;
        default: return 0;
    }
}
