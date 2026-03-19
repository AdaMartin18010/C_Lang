/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1123
 * Language: c
 * Block ID: e314f230
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux vDSO (virtual Dynamic Shared Object) 实现
 * 内核: arch/x86/entry/vdso/
 */

// vDSO数据结构 (内核更新)
struct vdso_data {
    __u32 seq;              // 序列号 (用于一致性检查)
    __s32 clock_mode;
    __u64 cycle_last;
    __u64 mask;
    __u32 mult;
    __u32 shift;

    // 实际时间数据
    union {
        struct vdso_timestamp clock;
        struct vdso_timestamp clock_raw;
        struct vdso_timestamp clock_tai;
    };

    // 时区信息
    __s32 tz_minuteswest;
    __s32 tz_dsttime;
} __attribute__((aligned(32)));

// 内核更新vDSO数据
timekeeper_update_vsyscall(struct timekeeper *tk)
{
    struct vdso_data *vdata = &__vdso_data;

    // 增加序列号 (奇数表示正在更新)
    vdata->seq++;
    smp_wmb();

    // 更新时间数据
    vdata->clock_mode = tk->tkr_mono.clock->id;
    vdata->cycle_last = tk->tkr_mono.cycle_last;
    vdata->mask = tk->tkr_mono.mask;
    vdata->mult = tk->tkr_mono.mult;
    vdata->shift = tk->tkr_mono.shift;

    // 更新wall time
    vdata->clock.sec = tk->xtime_sec;
    vdata->clock.nsec = tk->tkr_mono.xtime_nsec;

    smp_wmb();
    // 增加序列号 (偶数表示更新完成)
    vdata->seq++;
}

// vDSO中的gettimeofday (用户态执行)
notrace int __vdso_gettimeofday(struct __kernel_old_timeval *tv,
                                 struct timezone *tz)
{
    const struct vdso_data *vdata = __arch_get_vdso_data();
    u32 seq, msec;

    // 循环直到序列号稳定 (偶数)
    do {
        seq = READ_ONCE(vdata->seq);

        // 检查是否在更新中
        if (unlikely(seq & 1))
            continue;

        smp_rmb();  // 内存屏障

        // 复制时间数据
        if (tv) {
            tv->tv_sec = vdata->clock.sec;
            tv->tv_usec = vdata->clock.nsec / 1000;
        }

        if (tz) {
            tz->tz_minuteswest = vdata->tz_minuteswest;
            tz->tz_dsttime = vdata->tz_dsttime;
        }

        smp_rmb();

    } while (unlikely(seq != READ_ONCE(vdata->seq)));

    return 0;
}

// 获取CPU ID (快速，无需系统调用)
notrace int __vdso_getcpu(unsigned *cpu, unsigned *node,
                           struct getcpu_cache *tcache)
{
    unsigned int p;

    // 从TLS直接读取CPU ID
    p = __getcpu();

    if (cpu)
        *cpu = p & VDSO_CPUNODE_MASK;
    if (node)
        *node = p >> VDSO_CPUNODE_SHIFT;

    return 0;
}
