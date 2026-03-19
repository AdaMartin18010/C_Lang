/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 128
 * Language: c
 * Block ID: c7c96403
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 5G Baseband DMA Descriptor Structure
 * 适用于Scatter-Gather DMA操作
 * 对齐要求: 64字节边界 (Cache Line对齐)
 * ============================================================================ */

#include <stdint.h>
#include <stddef.h>
#include <stdalign.h>

/* DMA描述符控制标志 */
#define DMA_DESC_CTRL_SRC_INC       (1U << 0)   /* 源地址递增 */
#define DMA_DESC_CTRL_DST_INC       (1U << 1)   /* 目的地址递增 */
#define DMA_DESC_CTRL_SRC_FIXED     (1U << 2)   /* 源地址固定 (外设) */
#define DMA_DESC_CTRL_DST_FIXED     (1U << 3)   /* 目的地址固定 (外设) */
#define DMA_DESC_CTRL_IRQ_EN        (1U << 4)   /* 传输完成中断使能 */
#define DMA_DESC_CTRL_EOL           (1U << 5)   /* 链表结束标记 */
#define DMA_DESC_CTRL_5G_CRC_EN     (1U << 6)   /* 5G CRC校验使能 */
#define DMA_DESC_CTRL_BYPASS_CACHE  (1U << 7)   /* 缓存旁路 */

/* 描述符状态标志 */
#define DMA_DESC_STAT_DONE          (1U << 0)   /* 传输完成 */
#define DMA_DESC_STAT_ERROR         (1U << 1)   /* 传输错误 */
#define DMA_DESC_STAT_CRC_ERR       (1U << 2)   /* CRC错误 */
#define DMA_DESC_STAT_AXI_ERR       (1U << 3)   /* AXI总线错误 */

/* 传输类型枚举 - 5G基带专用 */
typedef enum {
    DMA_XFER_MEM_TO_MEM = 0,        /* 内存到内存 */
    DMA_XFER_MEM_TO_PERIPH,         /* 内存到外设 (DAC) */
    DMA_XFER_PERIPH_TO_MEM,         /* 外设到内存 (ADC) */
    DMA_XFER_LDPC_DEC_INPUT,        /* LDPC解码器输入 */
    DMA_XFER_LDPC_DEC_OUTPUT,       /* LDPC解码器输出 */
    DMA_XFER_FFT_INPUT,             /* FFT引擎输入 */
    DMA_XFER_FFT_OUTPUT,            /* FFT引擎输出 */
    DMA_XFER_HARQ_SOFT_BITS         /* HARQ软比特 */
} dma_xfer_type_t;

/* 数据宽度枚举 */
typedef enum {
    DMA_WIDTH_8BIT  = 0,            /* 字节传输 */
    DMA_WIDTH_16BIT = 1,            /* 半字传输 */
    DMA_WIDTH_32BIT = 2,            /* 字传输 */
    DMA_WIDTH_64BIT = 3,            /* 双字传输 */
    DMA_WIDTH_128BIT = 4            /* 128位 (5G基带) */
} dma_data_width_t;

/* DMA描述符结构 - 64字节对齐 */
typedef struct dma_descriptor {
    /* 控制字段 (16字节) */
    volatile uint32_t ctrl;             /* 控制寄存器 */
    volatile uint32_t xfer_size;        /* 传输字节数 */
    volatile uint32_t xfer_type;        /* 传输类型 */
    volatile uint32_t status;           /* 状态寄存器 */

    /* 地址字段 (16字节) */
    volatile uint64_t src_addr;         /* 源物理地址 */
    volatile uint64_t dst_addr;         /* 目的物理地址 */

    /* 5G扩展字段 (16字节) */
    volatile uint32_t cb_index;         /* Code Block索引 */
    volatile uint32_t rv_index;         /* Redundancy Version */
    volatile uint32_t harq_pid;         /* HARQ进程ID */
    volatile uint32_t reserved0;

    /* 链表字段 (16字节) */
    volatile uint64_t next_desc_phys;   /* 下一描述符物理地址 */
    volatile uint64_t user_data;        /* 用户上下文 */
} __attribute__((aligned(64))) dma_descriptor_t;

/* 描述符池统计 */
typedef struct {
    uint32_t total_desc;                /* 总描述符数 */
    uint32_t free_desc;                 /* 空闲描述符数 */
    uint32_t pending_desc;              /* 待处理描述符数 */
    uint32_t completed_desc;            /* 已完成描述符数 */
} desc_pool_stats_t;
