/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 640
 * Language: c
 * Block ID: ee64bf38
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 缓存性能监控与调试
// ============================================================================

#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>

/*
 * 使用perf监控缓存事件
 */

struct cache_perf_monitor {
    struct perf_event *llc_miss_event;
    struct perf_event *llc_ref_event;
    struct perf_event *dTLB_miss_event;
    u64 llc_miss_count;
    u64 llc_ref_count;
};

/*
 * 初始化缓存性能监控
 */
int init_cache_monitor(struct cache_perf_monitor *mon) {
    struct perf_event_attr attr = {
        .type = PERF_TYPE_HARDWARE,
        .size = sizeof(struct perf_event_attr),
        .disabled = 1,
        .exclude_kernel = 0,
        .exclude_hv = 1,
    };

    // LLC (Last Level Cache) misses
    attr.config = PERF_COUNT_HW_CACHE_MISSES;
    mon->llc_miss_event = perf_event_create_kernel_counter(&attr, -1,
                                                            NULL, NULL, NULL);
    if (!mon->llc_miss_event)
        return -ENOMEM;

    // LLC references
    attr.config = PERF_COUNT_HW_CACHE_REFERENCES;
    mon->llc_ref_event = perf_event_create_kernel_counter(&attr, -1,
                                                           NULL, NULL, NULL);
    if (!mon->llc_ref_event)
        goto err_miss;

    return 0;

err_miss:
    perf_event_release_kernel(mon->llc_miss_event);
    return -ENOMEM;
}

/*
 * 开始/停止监控
 */
void start_cache_monitor(struct cache_perf_monitor *mon) {
    perf_event_enable(mon->llc_miss_event);
    perf_event_enable(mon->llc_ref_event);
}

void stop_cache_monitor(struct cache_perf_monitor *mon) {
    perf_event_disable(mon->llc_miss_event);
    perf_event_disable(mon->llc_ref_event);

    mon->llc_miss_count = perf_event_read_value(mon->llc_miss_event,
                                                 &mon->llc_miss_count, NULL);
    mon->llc_ref_count = perf_event_read_value(mon->llc_ref_event,
                                                &mon->llc_ref_count, NULL);
}

/*
 * 获取缓存命中率
 */
float get_cache_hit_rate(struct cache_perf_monitor *mon) {
    if (mon->llc_ref_count == 0)
        return 0.0f;

    return (float)(mon->llc_ref_count - mon->llc_miss_count) /
           mon->llc_ref_count;
}

// ============================================================================
// 调试接口
// ============================================================================

/*
 * 导出缓存行状态 (x86使用PAT/PCD位)
 */
#ifdef CONFIG_X86
#include <asm/cpufeature.h>
#include <asm/processor.h>

void print_cache_info(void) {
    struct cpuinfo_x86 *c = &boot_cpu_data;

    printk(KERN_INFO "Cache Information:\n");
    printk(KERN_INFO "  L1d cache: %dK\n", c->x86_cache_size_l1d);
    printk(KERN_INFO "  L1i cache: %dK\n", c->x86_cache_size_l1i);
    printk(KERN_INFO "  L2 cache: %dK\n", c->x86_cache_size_l2);
    printk(KERN_INFO "  L3 cache: %dK\n", c->x86_cache_size_l3);
    printk(KERN_INFO "  Cache line size: %d bytes\n",
           c->x86_clflush_size);
}
#endif

/*
 * 检测伪共享 (使用perf c2c)
 */
void detect_false_sharing(void) {
    // 使用perf c2c工具检测缓存行竞争
    // perf c2c record ./your_program
    // perf c2c report
}
