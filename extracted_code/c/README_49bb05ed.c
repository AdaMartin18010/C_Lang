/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\README.md
 * Line: 158
 * Language: c
 * Block ID: 49bb05ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NUMA 距离矩阵表示节点间的访问延迟
void print_numa_distance_matrix() {
    int max_node = numa_max_node();
    int num_nodes = max_node + 1;

    printf("\nNUMA Distance Matrix:\n");
    printf("     ");
    for (int j = 0; j < num_nodes; j++) {
        printf("Node%-2d ", j);
    }
    printf("\n");

    for (int i = 0; i < num_nodes; i++) {
        printf("Node%d ", i);
        for (int j = 0; j < num_nodes; j++) {
            // 距离值：10 = 本地，20 = 同插槽，30 = 跨插槽等
            int distance = numa_distance(i, j);
            printf(" %4d  ", distance);
        }
        printf("\n");
    }
}

// 基于距离的最优节点选择
int find_nearest_node_with_memory(int target_node, size_t required_size) {
    int max_node = numa_max_node();
    int best_node = -1;
    int best_distance = INT_MAX;

    for (int node = 0; node <= max_node; node++) {
        long long free_mem, total_mem;
        numa_node_size64(node, &free_mem, &total_mem);

        if (free_mem >= (long long)required_size) {
            int distance = numa_distance(target_node, node);
            if (distance < best_distance) {
                best_distance = distance;
                best_node = node;
            }
        }
    }

    return best_node;
}
