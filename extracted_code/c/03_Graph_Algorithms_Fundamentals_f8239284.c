/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 1784
 * Language: c
 * Block ID: f8239284
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 简化版OSPF路由计算
 */
typedef struct {
    int dest;
    int next_hop;
    int cost;
} RouteEntry;

void calculate_routing_table(int network[MAX_VERTICES][MAX_VERTICES],
                             int n, int router, RouteEntry table[]) {
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];

    dijkstra_basic(network, n, router, dist, parent);

    for (int i = 0; i < n; i++) {
        table[i].dest = i;
        table[i].cost = dist[i];

        // 计算下一跳
        if (i == router) {
            table[i].next_hop = -1;
        } else if (parent[i] == router) {
            table[i].next_hop = i;
        } else {
            int curr = i;
            while (parent[curr] != router) {
                curr = parent[curr];
            }
            table[i].next_hop = curr;
        }
    }
}
