/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2060
 * Language: c
 * Block ID: 6216ac05
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 未对齐访问（某些架构上崩溃）
void alignment_violation(void) {
    char buf[10];
    int *p = (int*)(buf + 1);  // 1字节偏移，未对齐
    *p = 42;  // ❌ 在ARM/SPARC等严格对齐架构上崩溃
}

// ❌ 错误: 强制转换导致对齐丢失
typedef struct {
    char c;
    // 3字节填充
    int i;  // 4字节对齐
} AlignedStruct;

void alignment_loss(void) {
    char buffer[100];
    AlignedStruct *s = (AlignedStruct*)buffer;
    s->i = 42;  // ❌ buffer可能没有4字节对齐
}

// ❌ 错误: 结构体打包后未对齐访问
#pragma pack(push, 1)
struct Packed {
    char c;
    int i;  // 在打包结构中可能未对齐
};
#pragma pack(pop)

void alignment_packed(void) {
    struct Packed p;
    int *ip = &p.i;
    // *ip 的访问可能在严格对齐架构上出问题
}

// ✅ 正确: 确保对齐
void alignment_safe(void) {
    // 方式1: 使用aligned_alloc
    int *p = aligned_alloc(16, 100 * sizeof(int));
    // 现在p是16字节对齐的

    // 方式2: 编译器对齐修饰
    #ifdef __GNUC__
    char buffer[100] __attribute__((aligned(16)));
    #else
    _Alignas(16) char buffer[100];
    #endif
    int *ip = (int*)buffer;  // 安全对齐

    // 方式3: 手动对齐指针
    char raw[128];
    uintptr_t addr = (uintptr_t)raw;
    uintptr_t aligned_addr = (addr + 15) & ~15;  // 16字节对齐
    int *aligned_ptr = (int*)aligned_addr;

    free(p);
}
