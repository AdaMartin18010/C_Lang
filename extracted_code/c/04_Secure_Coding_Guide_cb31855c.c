/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 628
 * Language: c
 * Block ID: cb31855c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 强制初始化宏
#define DECLARE_ARRAY(type, name, size) \
    type name[size] = {0}

#define DECLARE_STRUCT(type, name) \
    type name = {0}

// 安全结构体初始化
typedef struct {
    char name[64];
    int age;
    char *data;
    size_t data_len;
} person_t;

person_t create_person(void) {
    // C23前: 全部初始化为0
    person_t p = {0};
    return p;
}

// C23 designated initializers with zero fill
#if __STDC_VERSION__ >= 202311L
person_t create_person_c23(void) {
    return (person_t){.name = ""};  // 其余自动为0
}
#endif

// 内存清零函数 - C11标准
#ifdef __STDC_LIB_EXT1__
#define SAFE_MEMZERO(ptr, size) \
    memset_s((ptr), (size), 0, (size))
#else
// 自定义安全清零（防止编译器优化）
static volatile void *(*volatile memset_ptr)(void *, int, size_t) = memset;

void secure_zero(void *ptr, size_t len) {
    memset_ptr(ptr, 0, len);
    // 内存屏障防止重排序
    __asm__ __volatile__("" ::: "memory");
}
#define SAFE_MEMZERO(ptr, size) secure_zero((ptr), (size))
#endif
