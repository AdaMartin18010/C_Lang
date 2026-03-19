/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\05_Computational_Complexity\01_Complexity_Classes.md
 * Line: 156
 * Language: c
 * Block ID: a7ccfeb6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 策略1: 近似算法 (Approximation)
// 顶点覆盖的2-近似算法

typedef struct {
    int edges[MAX_EDGES][2];
    int n_edges;
} Graph;

// 贪心2-近似
int approximate_vertex_cover(const Graph* g, bool cover[]) {
    bool used[MAX_EDGES] = {false};
    int cover_size = 0;

    for (int i = 0; i < g->n_edges; i++) {
        if (used[i]) continue;

        int u = g->edges[i][0];
        int v = g->edges[i][1];

        cover[u] = true;
        cover[v] = true;
        cover_size += 2;

        // 标记所有与u或v相连的边
        for (int j = 0; j < g->n_edges; j++) {
            if (g->edges[j][0] == u || g->edges[j][0] == v ||
                g->edges[j][1] == u || g->edges[j][1] == v) {
                used[j] = true;
            }
        }
    }

    return cover_size;  // ≤ 2 × 最优解
}

// 策略2: 固定参数可解 (FPT)
// 顶点覆盖参数化为k

bool fpt_vertex_cover(Graph* g, int k, bool cover[]) {
    if (g->n_edges == 0) return true;
    if (k == 0) return false;

    // 高 degree 顶点必须选
    int high_deg = find_high_degree_vertex(g);
    if (degree(g, high_deg) > k) {
        cover[high_deg] = true;
        remove_vertex(g, high_deg);
        return fpt_vertex_cover(g, k - 1, cover);
    }

    // 分支: 选择边的任一端点
    int u = g->edges[0][0];
    int v = g->edges[0][1];

    // 分支1: 选u
    Graph g1 = *g;
    bool cover1[MAX_VERTICES] = {0};
    cover1[u] = true;
    remove_vertex(&g1, u);
    if (fpt_vertex_cover(&g1, k - 1, cover1)) {
        memcpy(cover, cover1, sizeof(cover1));
        return true;
    }

    // 分支2: 选v
    Graph g2 = *g;
    cover[v] = true;
    remove_vertex(&g2, v);
    return fpt_vertex_cover(&g2, k - 1, cover);
}
// 时间: O(2^k × poly(n)) - 对小的k是可接受的

// 策略3: 启发式/元启发式
// 模拟退火求解TSP

double simulated_annealing_tSP(const double dist[MAX_CITIES][MAX_CITIES],
                                int n, int tour[]) {
    // 初始解
    for (int i = 0; i < n; i++) tour[i] = i;

    double T = 1000.0;  // 温度
    double best = tour_length(dist, tour, n);

    while (T > 1e-8) {
        // 随机2-opt移动
        int i = rand() % n;
        int j = rand() % n;

        double current = tour_length(dist, tour, n);
        swap(&tour[i], &tour[j]);
        double new_len = tour_length(dist, tour, n);

        double delta = new_len - current;
        if (delta < 0 || (double)rand() / RAND_MAX < exp(-delta / T)) {
            // 接受移动
            if (new_len < best) best = new_len;
        } else {
            // 撤销移动
            swap(&tour[i], &tour[j]);
        }

        T *= 0.995;  // 降温
    }

    return best;
}
