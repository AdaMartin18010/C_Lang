/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 136
 * Language: c
 * Block ID: 379a910f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// startup_stm32f4xx.c
// 启动代码负责初始化C运行环境

// 声明外部变量（链接脚本定义）
extern uint32_t _estack;        // 栈顶
extern uint32_t _sidata;        // ROM中数据开始
extern uint32_t _sdata;         // RAM中数据开始
extern uint32_t _edata;         // RAM中数据结束
extern uint32_t _sbss;          // BSS开始
extern uint32_t _ebss;          // BSS结束

// 主函数（应用程序入口）
extern int main(void);

// 复位处理函数
void Reset_Handler(void) {
    // 1. 复制数据段从Flash到RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 2. 清零BSS段
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // 3. 初始化系统时钟（可选）
    SystemInit();

    // 4. 调用主函数
    main();

    // 5. 主函数不应返回，如果返回则陷入死循环
    while (1);
}

// 默认中断处理
void Default_Handler(void) {
    while (1);
}

// 弱定义的异常处理函数（可被覆盖）
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
// ... 其他异常
