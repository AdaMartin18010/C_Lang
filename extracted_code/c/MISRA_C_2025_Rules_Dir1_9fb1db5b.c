/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir1.md
 * Line: 182
 * Language: c
 * Block ID: 9fb1db5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 合规 [C:2023延续] - 文档化的扩展使用
 * 扩展: GCC packed属性
 * 理由: 硬件寄存器映射需要精确内存布局
 * 范围: drivers/registers.h
 */
struct __attribute__((packed)) Packet {
    uint8_t header;
    uint32_t data;
};

/*
 * 合规 [C23相关] - C23标准特性不需要偏离
 * 这是标准C23，不是扩展
 */
#if __STDC_VERSION__ >= 202311L
    typeof(int) my_int;  /* C23标准特性 */
#else
    __typeof__(int) my_int;  /* GCC扩展，需文档化 */
#endif
