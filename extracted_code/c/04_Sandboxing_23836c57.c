/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 600
 * Language: c
 * Block ID: 23836c57
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C/C++ 使用WASI SDK
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 标准I/O工作正常
    printf("Hello from WASM!\n");

    // 文件操作（受限于授权目录）
    FILE* f = fopen("/sandbox/data.txt", "r");
    if (f) {
        char buf[256];
        fgets(buf, sizeof(buf), f);
        printf("Read: %s\n", buf);
        fclose(f);
    }

    // 环境变量访问（受限）
    const char* home = getenv("HOME");  // 可能返回NULL

    return 0;
}
