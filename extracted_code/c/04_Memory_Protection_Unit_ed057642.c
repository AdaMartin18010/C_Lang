/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 1323
 * Language: c
 * Block ID: ed057642
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * MPU如何防止常见的C语言内存安全问题
 */

/*
 * 1. 缓冲区溢出保护
 * MPU可以在栈和堆之间设置guard page
 */
void stack_overflow_protection(void) {
    /*
     * 栈布局:
     * 高地址
     * ┌─────────────┐
     * │  Heap       │  <- 堆向上增长
     * ├─────────────┤
     * │  Guard Page │  <- MPU保护，无访问权限
     * ├─────────────┤
     * │  Stack      │  <- 栈向下增长
     * └─────────────┘
     * 低地址
     *
     * 当栈溢出时会访问Guard Page，触发异常
     */

    char buffer[100];
    // 如果写入超过100字节，会触发MPU异常
    // 而不是静默破坏堆数据
}

/*
 * 2. 空指针解引用保护
 * 将地址0设置为不可访问
 */
void null_pointer_protection(void) {
    // MPU配置: 地址0-4KB无访问权限

    int *ptr = NULL;
    *ptr = 42;  // 触发MPU异常，而不是写入随机位置
}

/*
 * 3. 代码注入防护
 * 数据区设置为不可执行
 */
void code_injection_protection(void) {
    /*
     * MPU配置:
     * - 代码区 (Flash): 可读可执行
     * - 数据区 (SRAM): 可读可写，不可执行 (XN=1)
     * - 堆栈: 可读可写，不可执行
     *
     * 这样攻击者无法执行注入到数据区的shellcode
     */

    char shellcode[] = "\x90\x90\x90...";  // NOP sled
    void (*func)() = (void (*)())shellcode;
    // func();  // 会触发MPU异常，因为数据区不可执行
}

/*
 * 4. 堆损坏检测
 * 在堆块之间设置guard区域
 */
void heap_corruption_detection(void) {
    /*
     * 堆布局带MPU保护:
     *
     * ┌──────────────┐
     * │  Allocated   │
     * │  Block 1     │
     * ├──────────────┤
     * │  Guard Page  │  <- 防止Block 1溢出到Block 2
     * ├──────────────┤
     * │  Allocated   │
     * │  Block 2     │
     * ├──────────────┤
     * │  Guard Page  │
     * ├──────────────┤
     * │  Allocated   │
     * │  Block 3     │
     * └──────────────┘
     *
     * 缺点: 每个guard page消耗4KB内存
     * 实际系统可能只在关键边界使用
     */
}

/*
     * 5. 特权分离
 * 用户模式代码只能访问受限区域
 */
void privilege_separation(void) {
    /*
     * MPU配置示例:
     *
     * 特权模式 (内核):
     * - 可以访问所有区域
     *
     * 用户模式 (应用程序):
     * - 代码区: 只读执行
     * - 数据区: 读写
     * - 系统区域: 无访问
     *
     * 这样可以防止用户程序:
     * - 修改内核代码
     * - 直接访问硬件寄存器
     * - 访问其他进程的数据
     */
}

/*
 * 6. 防止使用未初始化指针
 * 保留特定地址范围，访问时触发异常
 */
void uninitialized_pointer_protection(void) {
    /*
     * 很多未初始化指针值为0或随机值
     * 将低地址空间(0-4KB)设置为不可访问
     * 可以捕获大部分未初始化指针使用
     */

    int *ptr;  // 未初始化
    // ...
    *ptr = 42;  // 如果ptr是0或低地址，触发异常
}
