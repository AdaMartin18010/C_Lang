/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 1931
 * Language: c
 * Block ID: 37b94243
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 嵌入式启动代码 - 使用C89确保最大兼容性 */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

/* 向量表初始化 */
void reset_handler(void);
void default_handler(void);

void (* const vector_table[])(void) = {
    (void (*)(void))0x20000000,  /* 栈顶 */
    reset_handler,                /* Reset */
    default_handler,              /* NMI */
    /* ... */
};

void reset_handler(void) {
    /* 复制.data段 */
    extern uint32_t _sidata, _sdata, _edata;
    uint32_t* src = &_sidata;
    uint32_t* dst = &_sdata;

    while (dst < &_edata) {
        *dst++ = *src++;
    }

    /* 清零.bss段 */
    extern uint32_t _sbss, _ebss;
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();
    while (1);
}
