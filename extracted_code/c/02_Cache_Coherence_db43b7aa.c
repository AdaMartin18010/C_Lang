/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 116
 * Language: c
 * Block ID: db43b7aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// Linux内核缓存一致性原语
// 基于Linux Kernel 5.x/6.x实现
// ============================================================================

#ifndef _CACHE_COHERENCE_H
#define _CACHE_COHERENCE_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <asm/barrier.h>
#include <asm/cacheflush.h>

// ============================================================================
// 1. 内存屏障 (Memory Barriers)
// ============================================================================

/*
 * 内存屏障类型:
 *
 * mb()  - 全内存屏障: 确保屏障前的读写操作完成后才执行屏障后的操作
 * rmb() - 读内存屏障: 确保屏障前的读操作完成后才执行屏障后的读操作
 * wmb() - 写内存屏障: 确保屏障前的写操作完成后才执行屏障后的写操作
 *
 * smp_mb()  - SMP全屏障 (仅在SMP系统生效)
 * smp_rmb() - SMP读屏障
 * smp_wmb() - SMP写屏障
 */

// 示例: 双缓冲机制中的屏障使用
struct double_buffer {
    u32 buffer[2][1024];
    volatile u32 active_buffer;  // 0或1
    u32 write_index;
};

void write_to_buffer(struct double_buffer *db, u32 data) {
    u32 buf_idx = db->active_buffer ^ 1;  // 写入非活跃缓冲区

    db->buffer[buf_idx][db->write_index++] = data;

    // 确保所有写操作完成后再切换缓冲区
    smp_wmb();

    db->active_buffer = buf_idx;
}

u32 read_from_buffer(struct double_buffer *db, u32 index) {
    u32 buf_idx = db->active_buffer;

    // 确保读取active_buffer后，读取数据前不重排序
    smp_rmb();

    return db->buffer[buf_idx][index];
}

// ============================================================================
// 2. 缓存行对齐与伪共享避免
// ============================================================================

/*
 * 缓存行大小 (典型值64字节，x86和ARM常见)
 */
#define CACHE_LINE_SIZE     64
#define CACHE_LINE_MASK     (~(CACHE_LINE_SIZE - 1))

/*
 * 缓存行对齐宏
 */
#define ____cacheline_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))
#define ____cacheline_aligned_in_smp ____cacheline_aligned

/*
 * 示例1: 每CPU变量对齐
 * 避免不同CPU更新不同变量时的伪共享
 */
struct per_cpu_counter {
    u64 count;
    u64 padding[7];  // 填充到64字节，确保独占缓存行
} ____cacheline_aligned;

struct per_cpu_counter cpu_counters[NR_CPUS];

void increment_counter(int cpu) {
    cpu_counters[cpu].count++;  // 无伪共享风险
}

/*
 * 示例2: 高并发统计结构
 */
struct concurrent_stats {
    // 分离到不同缓存行，避免多核竞争
    struct {
        u64 requests;
        u64 responses;
        u64 errors;
        u64 pad[5];  // 填充
    } ____cacheline_aligned cpu_data[NR_CPUS];
};

/*
 * 示例3: 使用kernel提供的percpu接口
 */
#include <linux/percpu.h>

// 定义每CPU变量
DEFINE_PER_CPU_ALIGNED(u64, packet_count);
DEFINE_PER_CPU_ALIGNED(u64, byte_count);

void count_packet(int bytes) {
    u64 *count = this_cpu_ptr(&packet_count);
    u64 *bytes_count = this_cpu_ptr(&byte_count);

    *count += 1;
    *bytes_count += bytes;
}

u64 get_total_packets(void) {
    u64 total = 0;
    int cpu;

    for_each_possible_cpu(cpu) {
        total += per_cpu(packet_count, cpu);
    }

    return total;
}

// ============================================================================
// 3. 缓存刷新操作
// ============================================================================

/*
 * 缓存刷新API (arch-specific)
 *
 * flush_cache_all()         - 刷新所有缓存
 * flush_cache_mm()          - 刷新指定mm的所有缓存
 * flush_cache_range()       - 刷新指定范围的缓存
 * flush_cache_page()        - 刷新指定页
 * flush_icache_range()      - 刷新指令缓存范围
 */

// 示例: DMA操作前后的缓存一致性处理
void prepare_dma_buffer(void *vaddr, size_t size, enum dma_data_direction dir) {
    unsigned long start = (unsigned long)vaddr;
    unsigned long end = start + size;

    if (dir == DMA_TO_DEVICE || dir == DMA_BIDIRECTIONAL) {
        // DMA从内存读取: 确保CPU写操作已刷到内存
        // 清理数据缓存 (Clean D-cache)
        dmac_clean_range(vaddr, vaddr + size);
    }

    if (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL) {
        // DMA写入内存: 使缓存行无效，避免CPU读到旧数据
        // 无效数据缓存 (Invalidate D-cache)
        dmac_inv_range(vaddr, vaddr + size);
    }
}

void complete_dma_buffer(void *vaddr, size_t size, enum dma_data_direction dir) {
    if (dir == DMA_FROM_DEVICE || dir == DMA_BIDIRECTIONAL) {
        // DMA完成后，使CPU缓存无效以看到新数据
        dmac_inv_range(vaddr, vaddr + size);
    }
}

// ============================================================================
// 4. RCU (Read-Copy-Update) 机制
// ============================================================================

#include <linux/rcupdate.h>
#include <linux/rculist.h>

/*
 * RCU是Linux内核中高效的读多写少同步机制
 * 读者无锁，写者复制更新
 */

struct my_data {
    int value;
    struct rcu_head rcu;
};

struct my_data __rcu *global_ptr;

// 读者 - 无锁，高效
int read_value_rcu(void) {
    int val;
    struct my_data *ptr;

    rcu_read_lock();
    ptr = rcu_dereference(global_ptr);
    val = ptr->value;
    rcu_read_unlock();

    return val;
}

// 写者 - 复制更新
void update_value_rcu(int new_value) {
    struct my_data *new_data = kmalloc(sizeof(*new_data), GFP_KERNEL);
    struct my_data *old_data;

    new_data->value = new_value;

    old_data = rcu_dereference_protected(global_ptr,
                                          lockdep_is_held(&my_mutex));

    // 原子更新指针
    rcu_assign_pointer(global_ptr, new_data);

    // 等待所有读者完成，释放旧数据
    synchronize_rcu();
    kfree(old_data);
}

// ============================================================================
// 5. 原子操作与内存序
// ============================================================================

#include <linux/atomic.h>

/*
 * Linux内核原子操作及其内存序语义
 */

// _relaxed: 无内存序约束，仅原子性
// _release: 之前的读写操作不会重排序到之后
// _acquire: 之后的读写操作不会重排序到之前
// _full:    release + acquire

atomic_t counter = ATOMIC_INIT(0);

void increment_relaxed(void) {
    atomic_fetch_add_relaxed(&counter, 1);
}

void increment_release(void) {
    atomic_fetch_add_release(&counter, 1);
}

void increment_acquire(void) {
    atomic_fetch_add_acquire(&counter, 1);
}

// 典型使用模式: release-acquire配对
void producer(void) {
    data->value = 42;
    // 确保data->value写入完成后才设置flag
    atomic_set_release(&flag, 1);
}

void consumer(void) {
    // 确保flag读取完成后再读取data->value
    while (atomic_read_acquire(&flag) == 0) {
        cpu_relax();
    }
    // 现在可以安全读取data->value
    use(data->value);
}

#endif /* _CACHE_COHERENCE_H */
