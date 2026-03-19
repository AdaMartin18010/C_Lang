/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\README.md
 * Line: 40
 * Language: c
 * Block ID: 1af1d137
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NUMA 拓扑结构（Linux libnuma 接口）
#include <numa.h>
#include <numaif.h>
#include <stdio.h>

void print_numa_topology() {
    // 检查 NUMA 是否可用
    if (numa_available() < 0) {
        printf("NUMA is not available\n");
        return;
    }

    // 获取 NUMA 节点数量
    int max_node = numa_max_node();
    int num_nodes = max_node + 1;
    printf("Number of NUMA nodes: %d\n", num_nodes);

    // 获取每个节点的信息
    for (int node = 0; node <= max_node; node++) {
        if (numa_bitmask_isbitset(numa_all_nodes_ptr, node)) {
            long long free_mem, total_mem;
            numa_node_size64(node, &free_mem, &total_mem);

            printf("\nNode %d:\n", node);
            printf("  Total memory: %lld MB\n", total_mem / (1024 * 1024));
            printf("  Free memory: %lld MB\n", free_mem / (1024 * 1024));

            // 获取节点的 CPU 掩码
            struct bitmask *cpus = numa_allocate_cpumask();
            numa_node_to_cpus(node, cpus);
            printf("  CPUs: ");
            for (int cpu = 0; cpu < numa_num_configured_cpus(); cpu++) {
                if (numa_bitmask_isbitset(cpus, cpu)) {
                    printf("%d ", cpu);
                }
            }
            printf("\n");
            numa_free_cpumask(cpus);
        }
    }
}

// 获取当前线程的 NUMA 节点
int get_current_numa_node() {
    int cpu = sched_getcpu();
    return numa_node_of_cpu(cpu);
}
