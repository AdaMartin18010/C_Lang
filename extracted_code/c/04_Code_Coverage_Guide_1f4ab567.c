/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\04_Code_Coverage_Guide.md
 * Line: 203
 * Language: c
 * Block ID: 1f4ab567
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

# main.c.gcov
        -:    1:#include <stdio.h>
        -:    2:
        1:    3:int main() {
        1:    4:    printf("Hello\n");
    #####:    5:    if (0) {           // 从未执行
    #####:    6:        printf("Never\n");
        -:    7:    }
        1:    8:    return 0;
        -:    9:}
