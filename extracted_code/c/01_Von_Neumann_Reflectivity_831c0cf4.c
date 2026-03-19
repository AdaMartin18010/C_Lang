/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 167
 * Language: c
 * Block ID: 831c0cf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 反射性（Reflectivity）：程序能够观察和修改自身
 *
 * 形式化定义：
 * 程序P是反射的，如果它能访问自己的代码表示
 */

// C语言中的有限反射（通过函数指针）
void reflective_function(void) {
    // 获取当前函数地址（平台相关）
    void *current_func = &&label;  // GCC扩展

    label:
    printf("Current function at: %p\n", current_func);

    // 理论上可以读取/修改自身代码
    // 实际中受限于W^X保护和代码段只读
}

// 通过/proc/self/mem的反射（Linux）
void reflect_via_proc(void) {
    // 打开自身内存映射
    int fd = open("/proc/self/mem", O_RDWR);

    // 获取函数地址
    void *func_addr = (void*)reflect_via_proc;

    // 定位到代码段
    off_t offset = (off_t)func_addr;
    lseek(fd, offset, SEEK_SET);

    // 读取自身代码
    uint8_t code[16];
    read(fd, code, sizeof(code));

    printf("My code: ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", code[i]);
    }
    printf("\n");

    close(fd);
}

// 真正的自修改需要解除保护
void enable_self_modification(void *addr, size_t len) {
    // 获取页边界
    long pagesize = sysconf(_SC_PAGESIZE);
    void *page = (void*)(((unsigned long)addr + pagesize - 1) & ~(pagesize - 1));

    // 修改页保护为可写
    mprotect(page, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
}
