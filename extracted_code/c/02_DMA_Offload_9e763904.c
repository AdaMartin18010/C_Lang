/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 1513
 * Language: c
 * Block ID: 9e763904
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * DMA性能优化 - 批量传输和双缓冲流水线
 * ============================================================================ */

/* 批量传输配置 */
#define BATCH_SIZE              16      /* 每批描述符数 */
#define PIPELINE_DEPTH          2       /* 流水线深度 */

/* 流水线缓冲区 */
typedef struct pipeline_buffer {
    void *buf[PIPELINE_DEPTH];          /* 缓冲区指针 */
    atomic_uint write_idx;              /* 写入索引 */
    atomic_uint read_idx;               /* 读取索引 */
    atomic_flag buf_lock[PIPELINE_DEPTH]; /* 缓冲区锁 */
} pipeline_buffer_t;

/* 批量提交传输 */
int dma_submit_batch(dma_channel_t *ch, sg_xfer_request_t **reqs,
                      uint32_t count)
{
    if (!ch || !reqs || count == 0 || count > ch->desc_pool_size) {
        return -1;
    }

    /* 批量分配描述符 */
    dma_descriptor_t *descs[BATCH_SIZE];
    uint32_t alloced = desc_alloc_batch(&g_desc_pool, descs, count);

    if (alloced < count) {
        /* 释放已分配的描述符 */
        for (uint32_t i = 0; i < alloced; i++) {
            desc_free(&g_desc_pool, descs[i]);
        }
        return -1;
    }

    /* 构建批量描述符链 */
    for (uint32_t i = 0; i < count; i++) {
        /* 构建单个请求的描述符 */
        dma_descriptor_t *chain = dma_build_sg_desc_chain(
            reqs[i], &descs[i], 1);

        if (i < count - 1) {
            /* 链接到下一个描述符 */
            descs[i]->next_desc_phys = (uint64_t)descs[i + 1];
            descs[i]->ctrl &= ~DMA_DESC_CTRL_EOL;
        }
    }

    /* 启动批量传输 */
    return dma_start_xfer_chain(ch, descs[0]);
}

/* 双缓冲流水线 - 用于连续数据流 */
typedef struct dma_pipeline {
    dma_channel_t *ch;
    pipeline_buffer_t ping_buf;
    pipeline_buffer_t pong_buf;
    atomic_flag *current_buf;
    void (*process_cb)(void *buf, size_t len);
} dma_pipeline_t;

/* 流水线处理循环 */
void *dma_pipeline_thread(void *arg)
{
    dma_pipeline_t *pipe = (dma_pipeline_t *)arg;
    pipeline_buffer_t *ping = &pipe->ping_buf;
    pipeline_buffer_t *pong = &pipe->pong_buf;
    pipeline_buffer_t *current = ping;

    uint32_t idx = 0;

    while (1) {
        /* 等待当前缓冲区DMA完成 */
        while (atomic_flag_test_and_set(&current->buf_lock[idx])) {
            __asm__ __volatile__("yield");
        }

        /* 处理已完成的缓冲区 */
        if (pipe->process_cb) {
            pipe->process_cb(current->buf[idx], BUFFER_SIZE);
        }

        /* 提交新的DMA传输到刚处理完的缓冲区 */
        dma_descriptor_t desc = {
            .src_addr = (uint64_t)current->buf[idx],
            .dst_addr = PERIPH_ADDR,
            .xfer_size = BUFFER_SIZE,
            .ctrl = DMA_DESC_CTRL_DST_FIXED | DMA_DESC_CTRL_IRQ_EN
        };

        /* 切换到另一个缓冲区 (双缓冲) */
        current = (current == ping) ? pong : ping;
        idx = (idx + 1) % PIPELINE_DEPTH;
    }

    return NULL;
}

/* 性能监控统计 */
typedef struct perf_stats {
    atomic_ulong total_xfers;           /* 总传输数 */
    atomic_ulong total_bytes;           /* 总字节数 */
    atomic_uint64_t total_cycles;       /* 总周期数 */
    atomic_uint min_latency;            /* 最小延迟 */
    atomic_uint max_latency;            /* 最大延迟 */
    atomic_uint64_t sum_latency;        /* 延迟总和 */
} perf_stats_t;

static perf_stats_t g_perf_stats;

/* 记录传输性能 */
void perf_record_xfer(uint32_t bytes, uint32_t cycles)
{
    atomic_fetch_add_explicit(&g_perf_stats.total_xfers, 1,
                              memory_order_relaxed);
    atomic_fetch_add_explicit(&g_perf_stats.total_bytes, bytes,
                              memory_order_relaxed);
    atomic_fetch_add_explicit(&g_perf_stats.total_cycles, cycles,
                              memory_order_relaxed);

    /* 更新延迟统计 */
    uint32_t min = atomic_load_explicit(&g_perf_stats.min_latency,
                                        memory_order_relaxed);
    uint32_t max = atomic_load_explicit(&g_perf_stats.max_latency,
                                        memory_order_relaxed);

    if (cycles < min || min == 0) {
        atomic_store_explicit(&g_perf_stats.min_latency, cycles,
                              memory_order_relaxed);
    }
    if (cycles > max) {
        atomic_store_explicit(&g_perf_stats.max_latency, cycles,
                              memory_order_relaxed);
    }

    atomic_fetch_add_explicit(&g_perf_stats.sum_latency, cycles,
                              memory_order_relaxed);
}

/* 计算有效带宽 (MB/s) */
double perf_calc_throughput(void)
{
    uint64_t bytes = atomic_load_explicit(&g_perf_stats.total_bytes,
                                          memory_order_relaxed);
    uint64_t cycles = atomic_load_explicit(&g_perf_stats.total_cycles,
                                           memory_order_relaxed);

    if (cycles == 0) return 0.0;

    /* 假设CPU频率2GHz */
    double seconds = (double)cycles / 2000000000.0;
    double throughput = (double)bytes / seconds / (1024.0 * 1024.0);

    return throughput;
}
