/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 329
 * Language: c
 * Block ID: 5f1a71b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传播常量值
int const_prop() {
    int x = 10;     // x = 10
    int y = x + 5;  // y = 15 (常量折叠)
    if (y > 20) {   // 假，此分支不可达
        // ...
    }
    return y * 2;   // return 30
}
