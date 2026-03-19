/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 337
 * Language: c
 * Block ID: 6cbdac3e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 5G Baseband DMA Controller Register Definitions
 * 基地址: 0x4000_0000 (示例)
 * ============================================================================ */

#define DMA_BASE_ADDR           0x40000000UL
#define DMA_REG_OFFSET(x)       (DMA_BASE_ADDR + (x))

/* 全局控制寄存器 */
#define DMA_GLB_CTRL            DMA_REG_OFFSET(0x0000)   /* 全局控制 */
#define DMA_GLB_STATUS          DMA_REG_OFFSET(0x0004)   /* 全局状态 */
#define DMA_GLB_IRQ_EN          DMA_REG_OFFSET(0x0008)   /* 全局中断使能 */
#define DMA_GLB_IRQ_STATUS      DMA_REG_OFFSET(0x000C)   /* 全局中断状态 */

/* 通道寄存器基址 (每通道0x100字节) */
#define DMA_CH_BASE(n)          DMA_REG_OFFSET(0x1000 + ((n) * 0x100))
#define DMA_CH_CTRL(n)          (DMA_CH_BASE(n) + 0x00)  /* 通道控制 */
#define DMA_CH_STATUS(n)        (DMA_CH_BASE(n) + 0x04)  /* 通道状态 */
#define DMA_CH_DESC_ADDR(n)     (DMA_CH_BASE(n) + 0x08)  /* 描述符地址 */
#define DMA_CH_XFER_CNT(n)      (DMA_CH_BASE(n) + 0x10)  /* 传输计数 */
#define DMA_CH_CURR_DESC(n)     (DMA_CH_BASE(n) + 0x18)  /* 当前描述符 */
#define DMA_CH_IRQ_MASK(n)      (DMA_CH_BASE(n) + 0x20)  /* 中断掩码 */

/* 通道控制寄存器位域 */
#define CH_CTRL_ENABLE          (1U << 0)    /* 通道使能 */
#define CH_CTRL_PAUSE           (1U << 1)    /* 通道暂停 */
#define CH_CTRL_RESET           (1U << 2)    /* 通道复位 */
#define CH_CTRL_CHAIN_MODE      (1U << 3)    /* 链模式使能 */
#define CH_CTRL_CIRCULAR        (1U << 4)    /* 环形模式 */
#define CH_CTRL_PRIO_MASK       (0x7U << 8)  /* 优先级掩码 */
#define CH_CTRL_PRIO_SHIFT      8

/* 通道状态寄存器位域 */
#define CH_STAT_ACTIVE          (1U << 0)    /* 传输中 */
#define CH_STAT_ERROR           (1U << 1)    /* 错误状态 */
#define CH_STAT_DESC_DONE       (1U << 2)    /* 描述符完成 */
#define CH_STAT_CHAIN_DONE      (1U << 3)    /* 链表完成 */
#define CH_STAT_FIFO_EMPTY      (1U << 4)    /* FIFO空 */
#define CH_STAT_FIFO_FULL       (1U << 5)    /* FIFO满 */

/* 中断掩码位域 */
#define CH_IRQ_DESC_DONE        (1U << 0)    /* 描述符完成中断 */
#define CH_IRQ_CHAIN_DONE       (1U << 1)    /* 链表完成中断 */
#define CH_IRQ_ERROR            (1U << 2)    /* 错误中断 */
#define CH_IRQ_FIFO_UNDERRUN    (1U << 3)    /* FIFO下溢 */
#define CH_IRQ_FIFO_OVERRUN     (1U << 4)    /* FIFO上溢 */

/* 寄存器访问辅助宏 */
#define REG_READ32(addr)        (*(volatile uint32_t *)(addr))
#define REG_WRITE32(addr, val)  (*(volatile uint32_t *)(addr) = (val))
#define REG_READ64(addr)        (*(volatile uint64_t *)(addr))
#define REG_WRITE64(addr, val)  (*(volatile uint64_t *)(addr) = (val))

/* 内存屏障宏 */
#if defined(__aarch64__)
    #define DMA_MB()            __asm__ __volatile__("dmb sy" ::: "memory")
    #define DMA_RMB()           __asm__ __volatile__("dmb ld" ::: "memory")
    #define DMA_WMB()           __asm__ __volatile__("dmb st" ::: "memory")
#elif defined(__arm__)
    #define DMA_MB()            __asm__ __volatile__("dmb" ::: "memory")
    #define DMA_RMB()           __asm__ __volatile__("dsb" ::: "memory")
    #define DMA_WMB()           __asm__ __volatile__("dsb" ::: "memory")
#else
    #define DMA_MB()            __sync_synchronize()
    #define DMA_RMB()           __sync_synchronize()
    #define DMA_WMB()           __sync_synchronize()
#endif
