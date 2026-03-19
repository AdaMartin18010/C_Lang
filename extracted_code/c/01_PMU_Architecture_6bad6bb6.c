/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 227
 * Language: c
 * Block ID: 6bad6bb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* AMD性能监控MSR定义 */
#define MSR_AMD_PERFEVTSEL0     0xC0010000  /* 事件选择寄存器0 */
#define MSR_AMD_PERFEVTSEL1     0xC0010001  /* 事件选择寄存器1 */
#define MSR_AMD_PERFEVTSEL2     0xC0010002  /* 事件选择寄存器2 */
#define MSR_AMD_PERFEVTSEL3     0xC0010003  /* 事件选择寄存器3 */
#define MSR_AMD_PMC0            0xC0010004  /* 性能计数器0 */
#define MSR_AMD_PMC1            0xC0010005  /* 性能计数器1 */
#define MSR_AMD_PMC2            0xC0010006  /* 性能计数器2 */
#define MSR_AMD_PMC3            0xC0010007  /* 性能计数器3 */

/* AMD事件选择寄存器位定义 */
#define AMD_EVTSEL_EVENT_MASK   0x000000FF  /* 事件选择 (bits 0-7) */
#define AMD_EVTSEL_UNIT_MASK    0x0000FF00  /* 单元掩码 (bits 8-15) */
#define AMD_EVTSEL_USR          (1ULL << 16) /* 用户态 */
#define AMD_EVTSEL_OS           (1ULL << 17) /* 内核态 */
#define AMD_EVTSEL_EDGE         (1ULL << 18) /* 边沿检测 */
#define AMD_EVTSEL_INT          (1ULL << 20) /* 中断使能 */
#define AMD_EVTSEL_ENABLE       (1ULL << 22) /* 计数器使能 */
#define AMD_EVTSEL_INV          (1ULL << 23) /* 反转 */
#define AMD_EVTSEL_CMASK_MASK   0xFF000000  /* 比较掩码 (bits 24-31) */
