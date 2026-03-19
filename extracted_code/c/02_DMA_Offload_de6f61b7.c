/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1690
 * Language: c
 * Block ID: de6f61b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * DMA调试与诊断工具
 * ============================================================================ */

/* 调试级别 */
typedef enum {
    DMA_DBG_NONE = 0,
    DMA_DBG_ERROR,                      /* 仅错误 */
    DMA_DBG_WARN,                       /* 警告 */
    DMA_DBG_INFO,                       /* 信息 */
    DMA_DBG_VERBOSE                     /* 详细 */
} dma_debug_level_t;

static dma_debug_level_t g_dbg_level = DMA_DBG_INFO;

/* 调试宏 */
#define DMA_DBG(level, fmt, ...) \
    do { \
        if (level <= g_dbg_level) { \
            printf("[DMA][%s] " fmt "\n", \
                   #level, ##__VA_ARGS__); \
        } \
    } while(0)

/* 通道状态转储 */
void dma_dump_channel_status(dma_channel_t *ch)
{
    if (!ch) return;

    uint32_t ctrl = REG_READ32(DMA_CH_CTRL(ch->id));
    uint32_t status = REG_READ32(DMA_CH_STATUS(ch->id));
    uint64_t desc_addr = REG_READ64(DMA_CH_DESC_ADDR(ch->id));
    uint32_t xfer_cnt = REG_READ32(DMA_CH_XFER_CNT(ch->id));
    uint64_t curr_desc = REG_READ64(DMA_CH_CURR_DESC(ch->id));

    printf("=== DMA Channel %d Status ===\n", ch->id);
    printf("  Control:     0x%08X\n", ctrl);
    printf("  Status:      0x%08X\n", status);
    printf("  Desc Addr:   0x%016lX\n", desc_addr);
    printf("  Xfer Count:  %u\n", xfer_cnt);
    printf("  Curr Desc:   0x%016lX\n", curr_desc);
    printf("  State:       %s\n",
           ch->state == DMA_CH_STATE_IDLE ? "IDLE" :
           ch->state == DMA_CH_STATE_ACTIVE ? "ACTIVE" :
           ch->state == DMA_CH_STATE_PAUSED ? "PAUSED" :
           ch->state == DMA_CH_STATE_ERROR ? "ERROR" : "UNKNOWN");

    /* 状态位解析 */
    printf("  Flags:\n");
    printf("    ACTIVE:      %s\n", status & CH_STAT_ACTIVE ? "Y" : "N");
    printf("    ERROR:       %s\n", status & CH_STAT_ERROR ? "Y" : "N");
    printf("    DESC_DONE:   %s\n", status & CH_STAT_DESC_DONE ? "Y" : "N");
    printf("    CHAIN_DONE:  %s\n", status & CH_STAT_CHAIN_DONE ? "Y" : "N");
    printf("    FIFO_EMPTY:  %s\n", status & CH_STAT_FIFO_EMPTY ? "Y" : "N");
    printf("    FIFO_FULL:   %s\n", status & CH_STAT_FIFO_FULL ? "Y" : "N");
}

/* 描述符转储 */
void dma_dump_descriptor(dma_descriptor_t *desc)
{
    if (!desc) return;

    printf("=== DMA Descriptor @ %p ===\n", (void *)desc);
    printf("  Control:     0x%08X\n", desc->ctrl);
    printf("  Xfer Size:   %u\n", desc->xfer_size);
    printf("  Xfer Type:   %u\n", desc->xfer_type);
    printf("  Status:      0x%08X\n", desc->status);
    printf("  Src Addr:    0x%016lX\n", desc->src_addr);
    printf("  Dst Addr:    0x%016lX\n", desc->dst_addr);
    printf("  Next Desc:   0x%016lX\n", desc->next_desc_phys);
    printf("  User Data:   0x%016lX\n", desc->user_data);

    printf("  Control Flags:\n");
    printf("    SRC_INC:     %s\n",
           desc->ctrl & DMA_DESC_CTRL_SRC_INC ? "Y" : "N");
    printf("    DST_INC:     %s\n",
           desc->ctrl & DMA_DESC_CTRL_DST_INC ? "Y" : "N");
    printf("    IRQ_EN:      %s\n",
           desc->ctrl & DMA_DESC_CTRL_IRQ_EN ? "Y" : "N");
    printf("    EOL:         %s\n",
           desc->ctrl & DMA_DESC_CTRL_EOL ? "Y" : "N");
}

/* 断言宏 */
#define DMA_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("[DMA ASSERT] %s at %s:%d\n", msg, __FILE__, __LINE__); \
            dma_dump_channel_status(ch); \
            __builtin_trap(); \
        } \
    } while(0)

/* 传输验证 */
int dma_verify_xfer(dma_descriptor_t *desc, void *src_expected,
                     void *dst_expected, size_t len)
{
    int errors = 0;

    /* 检查描述符参数 */
    if (desc->src_addr == 0) {
        DMA_DBG(DMA_DBG_ERROR, "Invalid source address");
        errors++;
    }
    if (desc->dst_addr == 0) {
        DMA_DBG(DMA_DBG_ERROR, "Invalid destination address");
        errors++;
    }
    if (desc->xfer_size == 0 || desc->xfer_size > 0xFFFFFF) {
        DMA_DBG(DMA_DBG_ERROR, "Invalid transfer size: %u", desc->xfer_size);
        errors++;
    }
    if (desc->xfer_size != len) {
        DMA_DBG(DMA_DBG_WARN, "Size mismatch: desc=%u, expected=%zu",
                desc->xfer_size, len);
    }

    return errors;
}
