/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\README.md
 * Line: 375
 * Language: c
 * Block ID: b2709113
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 格式化前
int foo(int x,int*y){if(x>0){*y=x;return 1;}else{return 0;}}

// 格式化后
int foo(int x, int *y)
{
    if (x > 0) {
        *y = x;
        return 1;
    } else {
        return 0;
    }
}
