/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 96
 * Language: c
 * Block ID: 9154f538
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 冯诺依曼架构的基本自修改示例
// 在x86-64 Linux上

#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

// 自修改函数：运行时改变自身行为
void self_modifying_example(void) {
    // 获取页大小
    size_t page_size = sysconf(_SC_PAGESIZE);

    // 分配可执行内存
    uint8_t *code = mmap(NULL, page_size,
                         PROT_READ | PROT_WRITE | PROT_EXEC,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // 初始代码：返回42
    uint8_t initial_code[] = {
        0xB8, 0x2A, 0x00, 0x00, 0x00,  // mov eax, 42
        0xC3                           // ret
    };

    memcpy(code, initial_code, sizeof(initial_code));
    __builtin___clear_cache(code, code + sizeof(initial_code));

    // 执行初始代码
    int (*func)(void) = (int (*)(void))code;
    printf("Before modification: %d\n", func());

    // 自修改：改变返回值为100
    code[1] = 100;  // 修改立即数
    __builtin___clear_cache(code, code + sizeof(initial_code));

    printf("After modification: %d\n", func());

    munmap(code, page_size);
}

// 更复杂的自修改：条件跳转修改
void self_modifying_branch(void) {
    uint8_t *code = mmap_executable(1024);

    // 初始：总是走"else"分支
    // if (condition) { return 1; } else { return 0; }
    // 编译为：test condition; jz else; mov eax, 1; jmp end; else: mov eax, 0; end: ret

    // 通过修改条件跳转指令改变行为
    // jz offset -> jnz offset 翻转条件

    // 0x74 XX = jz rel8
    // 0x75 XX = jnz rel8

    uint8_t branch_code[] = {
        0x48, 0x85, 0xFF,        // test rdi, rdi
        0x74, 0x05,              // jz +5 (else分支)
        0xB8, 0x01, 0x00, 0x00, 0x00,  // mov eax, 1
        0xEB, 0x03,              // jmp +3
        0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0
        0xC3                     // ret
    };

    memcpy(code, branch_code, sizeof(branch_code));

    // 通过修改第4字节（0x74）为0x75，翻转分支逻辑
}
