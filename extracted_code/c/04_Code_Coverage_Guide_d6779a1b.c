/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\04_Code_Coverage_Guide.md
 * Line: 696
 * Language: c
 * Block ID: d6779a1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用宏排除不可达代码
#ifdef ENABLE_COVERAGE
#define COVERAGE_EXCLUDE_START
#define COVERAGE_EXCLUDE_END
#else
#define COVERAGE_EXCLUDE_START /* GCOV_EXCL_START */
#define COVERAGE_EXCLUDE_END   /* GCOV_EXCL_STOP */
#endif

// 标记不需要覆盖的代码（如错误处理）
COVERAGE_EXCLUDE_START
void panic(const char* msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();  // 永远不会返回
}
COVERAGE_EXCLUDE_END
