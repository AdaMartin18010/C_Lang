/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 859
 * Language: c
 * Block ID: a33dcb70
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * IEEE 1588 Precision Time Protocol (PTP) 客户端
 * 硬件时间戳支持
 */

#include <linux/ptp_clock.h>

struct ptp_sync_state {
    int ptp_fd;                     /* PTP设备文件描述符 */
    struct timespec last_offset;    /* 上次测量偏移 */
    int64_t drift_ppb;              /* 漂移（十亿分之一） */

    /* 统计 */
    uint64_t sync_count;
    uint64_t max_offset_ns;
    uint64_t mean_offset_ns;
};

/* 初始化PTP */
int ptp_init(struct ptp_sync_state *state, const char *ptp_device)
{
    state->ptp_fd = open(ptp_device, O_RDWR);
    if (state->ptp_fd < 0) {
        perror("Failed to open PTP device");
        return -1;
    }

    /* 获取时钟能力 */
    struct ptp_clock_caps caps;
    if (ioctl(state->ptp_fd, PTP_CLOCK_GETCAPS, &caps) < 0) {
        perror("PTP_CLOCK_GETCAPS failed");
        close(state->ptp_fd);
        return -1;
    }

    printf("PTP caps: max_adj=%d, n_alarm=%d, n_ext_ts=%d, n_per_out=%d, pps=%d\n",
           caps.max_adj, caps.n_alarm, caps.n_ext_ts,
           caps.n_per_out, caps.pps);

    return 0;
}

/* 测量时钟偏移 */
int64_t ptp_measure_offset(struct ptp_sync_state *state)
{
    struct ptp_sys_offset offset;
    struct timespec ts_before, ts_after;
    int64_t delay, offset_ns;

    /* 软件时间戳测量 */
    clock_gettime(CLOCK_REALTIME, &ts_before);

    /* 获取PTP硬件时间戳 */
    if (ioctl(state->ptp_fd, PTP_SYS_OFFSET, &offset) < 0) {
        perror("PTP_SYS_OFFSET failed");
        return 0;
    }

    clock_gettime(CLOCK_REALTIME, &ts_after);

    /* 计算往返延迟 */
    delay = (ts_after.tv_sec - ts_before.tv_sec) * 1000000000LL +
            (ts_after.tv_nsec - ts_before.tv_nsec);

    /* 简单偏移计算（假设对称延迟） */
    offset_ns = ((offset.ts[0].sec - offset.ts[1].sec) * 1000000000LL +
                 (offset.ts[0].nsec - offset.ts[1].nsec)) / 2;

    /* 更新统计 */
    state->sync_count++;
    if (llabs(offset_ns) > state->max_offset_ns) {
        state->max_offset_ns = llabs(offset_ns);
    }

    return offset_ns;
}

/* 调整系统时钟 */
void ptp_adjust_clock(struct ptp_sync_state *state, int64_t offset_ns)
{
    struct timex tx;

    memset(&tx, 0, sizeof(tx));

    /* 相位调整 - 立即修正 */
    if (llabs(offset_ns) > 1000000) {  /* > 1ms */
        /* 步进调整 */
        struct timespec adj;
        adj.tv_sec = offset_ns / 1000000000LL;
        adj.tv_nsec = offset_ns % 1000000000LL;
        clock_settime(CLOCK_REALTIME, &adj);
    } else {
        /* 平滑调整 */
        tx.modes = ADJ_OFFSET | ADJ_NANO;
        tx.offset = offset_ns;  /* 纳秒偏移 */
        adjtimex(&tx);
    }

    /* 频率调整 - 补偿漂移 */
    tx.modes = ADJ_FREQUENCY;
    tx.freq = state->drift_ppb * 65536;  /* 转换为ppm */
    adjtimex(&tx);
}
