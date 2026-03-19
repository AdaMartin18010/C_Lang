/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 104
 * Language: c
 * Block ID: e6d9455b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// WebAssembly 内存以页为单位管理
// 1 页 = 64 KB = 65,536 字节

#define WASM_PAGE_SIZE 65536

// 内存配置计算
void print_memory_info() {
    // 初始内存页数
    int initial_pages = 256;  // 256 * 64KB = 16MB

    // 最大内存页数
    int max_pages = 32768;    // 32768 * 64KB = 2GB

    printf("WASM Page Size: %d bytes (64 KB)\n", WASM_PAGE_SIZE);
    printf("Initial memory: %d pages = %d MB\n", initial_pages, initial_pages * 64 / 1024);
    printf("Maximum memory: %d pages = %d MB\n", max_pages, max_pages * 64 / 1024);
}

// 编译选项对应关系
/*
-sINITIAL_MEMORY=16MB    → 256 pages
-sINITIAL_MEMORY=64MB    → 1024 pages
-sINITIAL_MEMORY=256MB   → 4096 pages
-sINITIAL_MEMORY=1GB     → 16384 pages
-sMAXIMUM_MEMORY=2GB     → 32768 pages
*/
