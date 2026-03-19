/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 554
 * Language: c
 * Block ID: 51d24fa1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* RISC-V PMU CSR定义 */

/* 标准计数器 */
#define CSR_CYCLE       0xC00    /* 周期计数器 (U-mode RO) */
#define CSR_TIME        0xC01    /* 时间计数器 (U-mode RO) */
#define CSR_INSTRET     0xC02    /* 指令退役计数器 (U-mode RO) */
#define CSR_HPMCOUNTER3 0xC03    /* 通用计数器3 (U-mode RO) */
/* ... CSR_HPMCOUNTER31 0xC1F */

/* 机器模式计数器 */
#define CSR_MCYCLE      0xB00    /* 机器模式周期计数器 */
#define CSR_MINSTRET    0xB02    /* 机器模式指令计数器 */
#define CSR_MHPMCOUNTER3 0xB03   /* 机器模式通用计数器3 */
/* ... CSR_MHPMCOUNTER31 0xB1F */

/* 事件选择寄存器 */
#define CSR_MHPMEVENT3  0x323    /* 事件选择3 */
#define CSR_MHPMEVENT4  0x324    /* 事件选择4 */
/* ... CSR_MHPMEVENT31 0x33F */

/* 计数器控制寄存器 */
#define CSR_MCOUNTEREN  0x306    /* 计数器访问使能 */
#define CSR_SCOUNTEREN  0x106    /* S模式计数器访问使能 */
#define CSR_MCOUNTINHIBIT 0x320  /* 计数器禁止 */

/* mhpmevent 寄存器位定义 */
#define MHPMEVENT_EVENT_MASK    0x000000000000FFFFULL  /* 事件编码 (bits 0-15) */
#define MHPMEVENT_U             (1ULL << 16)           /* 用户模式计数 */
#define MHPMEVENT_S             (1ULL << 17)           /* 监督模式计数 */
#define MHPMEVENT_M             (1ULL << 18)           /* 机器模式计数 */
#define MHPMEVENT_VS            (1ULL << 19)           /* VS模式计数 */
#define MHPMEVENT_VU            (1ULL << 20)           /* VU模式计数 */
#define MHPMEVENT_MINH          (1ULL << 21)           /* 机器模式禁止 */
#define MHPMEVENT_SINH          (1ULL << 22)           /* 监督模式禁止 */
#define MHPMEVENT_UINH          (1ULL << 23)           /* 用户模式禁止 */
#define MHPMEVENT_VSINH         (1ULL << 24)           /* VS模式禁止 */
#define MHPMEVENT_VUINH         (1ULL << 25)           /* VU模式禁止 */
#define MHPMEVENT_OF            (1ULL << 31)           /* 溢出标志 */
