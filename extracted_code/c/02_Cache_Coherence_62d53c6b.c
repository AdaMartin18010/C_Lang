/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 494
 * Language: c
 * Block ID: 62d53c6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// NUMA (Non-Uniform Memory Access) 优化
// ============================================================================

#include <linux/topology.h>
#include <linux/mmzone.h>
#include <linux/mempolicy.h>
#include <linux/migrate.h>

/*
 * NUMA节点信息
 */
struct numa_info {
    int node_id;
    unsigned long total_pages;
    unsigned long free_pages;
    int nr_cpus;
};

// 获取当前CPU的NUMA节点
static inline int get_current_numa_node(void) {
    return numa_node_id();
}

// 获取指定地址的NUMA节点
int get_node_of_address(void *addr) {
    struct page *page = virt_to_page(addr);
    return page_to_nid(page);
}

// ============================================================================
// NUMA感知的内存分配
// ============================================================================

/*
 * 在指定NUMA节点分配内存
 */
void *numa_alloc_on_node(size_t size, int node) {
    struct page *page;
    void *addr;

    // 使用GFP_THISNODE强制在指定节点分配
    page = alloc_pages_node(node, GFP_KERNEL | GFP_THISNODE,
                             get_order(size));
    if (!page)
        return NULL;

    addr = page_address(page);
    return addr;
}

/*
 * 本地优先分配
 */
void *numa_alloc_local(size_t size) {
    int node = get_current_numa_node();
    return numa_alloc_on_node(size, node);
}

/*
 * 交错分配 (Interleave) - 分散到多个节点
 */
void *numa_alloc_interleaved(size_t size) {
    // 使用MPOL_INTERLEAVE策略
    struct mempolicy *old, *new;
    nodemask_t nodes;
    void *addr;

    nodes_fill(&nodes);  // 所有节点
    new = mpolicy_new(MPOL_INTERLEAVE, &nodes);
    if (!new)
        return NULL;

    old = current->mempolicy;
    current->mempolicy = new;

    addr = kmalloc(size, GFP_KERNEL);

    current->mempolicy = old;
    mpolicy_put(new);

    return addr;
}

// ============================================================================
// 页面迁移
// ============================================================================

/*
 * 将页面迁移到指定NUMA节点
 */
int migrate_to_node(void *addr, size_t size, int target_node) {
    struct migration_target_control mtc = {
        .nid = target_node,
        .gfp_mask = GFP_KERNEL,
    };

    unsigned long start = (unsigned long)addr;
    unsigned long end = start + size;

    return migrate_pages(start, end, &mtc, MIGRATE_SYNC);
}

/*
 * 自动页面临近性迁移
 */
void auto_migrate_pages(void) {
    // 内核自动将页面迁移到访问最频繁的CPU所在节点
    // 通过/proc/sys/kernel/numa_balancing控制

    // 手动触发扫描
    // echo 1 > /proc/sys/kernel/numa_balancing_scan_period_min_ms
}

// ============================================================================
// NUMA感知的线程绑定
// ============================================================================

#include <linux/cpuset.h>
#include <linux/sched.h>

/*
 * 将任务绑定到特定NUMA节点的CPU
 */
int bind_task_to_numa_node(struct task_struct *task, int node) {
    const struct cpumask *node_cpumask = cpumask_of_node(node);

    return sched_setaffinity(task->pid, node_cpumask);
}

/*
 * 将内存绑定到特定节点
 */
int bind_memory_to_node(int node) {
    nodemask_t nodes;

    nodes_clear(nodes);
    node_set(node, nodes);

    return set_mempolicy(MPOL_BIND, &nodes);
}
