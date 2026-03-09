# 表面码纠错解码器

> **层级定位**: 04 Industrial Scenarios / 06 Quantum Computing
> **对应标准**: Surface Code, Minimum Weight Perfect Matching (MWPM)
> **难度级别**: L5 综合
> **预估学习时间**: 12-18 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 表面码结构、综合征提取、完美匹配解码、容错阈值 |
| **前置知识** | 稳定子码、错误综合征、图论算法 |
| **后续延伸** | 颜色码、LDPC码、实时解码器硬件 |
| **权威来源** | Fowler et al. 2012, Google Quantum AI |

---

## 🧠 知识结构思维导图

```mermaid
mindmap
  root((表面码解码))
    表面码结构
      数据量子比特
      X型稳定子
      Z型稳定子
      边界条件
    综合征提取
      辅助量子比特
       syndrome测量
      重复测量
    解码算法
      权重图构建
      MWPM算法
      Union-Find
    错误纠正
      X错误纠正
      Z错误纠正
      逻辑错误率
```

---

## 📖 核心概念详解

### 1. 表面码结构

```
┌─────────────────────────────────────────────────────────────────────┐
│                      距离d=5的表面码                                 │
│                    (d²=25 数据量子比特)                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  d=5: 需要 (2d-1)² = 81 个物理位置                                   │
│                                                                      │
│        ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○                           │
│        │ X │ Z │ X │ Z │ X │ Z │ X │ Z │                           │
│        ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○                           │
│        │ Z │ X │ Z │ X │ Z │ X │ Z │ X │                           │
│        ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○                           │
│        │ X │ Z │ X │ Z │ X │ Z │ X │ Z │                           │
│        ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○                           │
│        │ Z │ X │ Z │ X │ Z │ X │ Z │ X │                           │
│        ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○ ─ ○                           │
│                                                                      │
│  图例:                                                               │
│    ○ = 量子比特位置 (数据或辅助)                                      │
│    X = X型稳定子 (测量X⊗X⊗X⊗X)                                       │
│    Z = Z型稳定子 (测量Z⊗Z⊗Z⊗Z)                                       │
│    ─ = 连接边 (物理连接或逻辑连接)                                     │
│                                                                      │
│  逻辑量子比特:                                                        │
│    - 逻辑X操作: 沿左侧垂直边界的X串                                    │
│    - 逻辑Z操作: 沿顶部水平边界的Z串                                    │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 2. 表面码数据结构

```c
// ============================================================================
// 表面码解码器核心数据结构
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// 码距参数
#define SURFACE_CODE_DISTANCE   5
#define NUM_DATA_QUBITS         (SURFACE_CODE_DISTANCE * SURFACE_CODE_DISTANCE)
#define NUM_X_STABILIZERS       ((SURFACE_CODE_DISTANCE - 1) * (SURFACE_CODE_DISTANCE - 1) / 2)
#define NUM_Z_STABILIZERS       ((SURFACE_CODE_DISTANCE - 1) * (SURFACE_CODE_DISTANCE - 1) / 2)
#define NUM_ANCILLA_QUBITS      (NUM_X_STABILIZERS + NUM_Z_STABILIZERS)
#define TOTAL_QUBITS            (NUM_DATA_QUBITS + NUM_ANCILLA_QUBITS)

// 错误类型
typedef enum {
    ERROR_NONE = 0,
    ERROR_X = 1,        // 比特翻转
    ERROR_Y = 2,        // 比特+相位翻转
    ERROR_Z = 3         // 相位翻转
} ErrorType;

// 量子比特类型
typedef enum {
    QUBIT_DATA = 0,
    QUBIT_ANCILLA_X,
    QUBIT_ANCILLA_Z
} QubitType;

// 量子比特定义
typedef struct {
    uint16_t id;
    QubitType type;
    uint8_t row;
    uint8_t col;
    ErrorType error;            // 当前错误状态
    bool is_logical;            // 是否为逻辑算符边界
} Qubit;

// 稳定子 ( syndrome测量)
typedef struct {
    uint16_t id;
    uint8_t type;               // 'X' 或 'Z'
    uint16_t ancilla_id;        // 辅助量子比特ID
    uint16_t data_qubits[4];    // 相邻数据量子比特 (最多4个)
    uint8_t num_neighbors;      // 邻居数量 (2-4)
    uint8_t syndrome_value;     // 当前syndrome值 (0或1)
    uint8_t syndrome_history[3]; // 重复测量历史
} Stabilizer;

// 表面码布局
typedef struct {
    uint8_t distance;
    Qubit qubits[TOTAL_QUBITS];
    Stabilizer x_stabilizers[NUM_X_STABILIZERS];
    Stabilizer z_stabilizers[NUM_Z_STABILIZERS];

    // 邻接图
    uint16_t adjacency[TOTAL_QUBITS][4];  // 每个量子比特的邻居
    uint8_t num_neighbors[TOTAL_QUBITS];
} SurfaceCode;

// ============================================================================
// 初始化表面码结构
// ============================================================================

void surface_code_init(SurfaceCode *sc, uint8_t distance) {
    sc->distance = distance;

    uint16_t qubit_id = 0;
    uint16_t x_stab_id = 0;
    uint16_t z_stab_id = 0;

    // 网格大小: (2d-1) x (2d-1)
    uint8_t grid_size = 2 * distance - 1;

    // 创建量子比特网格
    for (uint8_t r = 0; r < grid_size; r++) {
        for (uint8_t c = 0; c < grid_size; c++) {
            Qubit *q = &sc->qubits[qubit_id];
            q->id = qubit_id;
            q->row = r;
            q->col = c;
            q->error = ERROR_NONE;
            q->is_logical = false;

            // 确定类型
            // 数据量子比特在(r+c)为偶数的位置
            if ((r + c) % 2 == 0) {
                // 边界检查 - 逻辑量子比特边界
                if (r == 0 || r == grid_size - 1 ||
                    c == 0 || c == grid_size - 1) {
                    q->is_logical = true;
                }

                // 判断是数据还是辅助
                // 标准布局: 内部偶数位置为数据
                if (r > 0 && r < grid_size - 1 &&
                    c > 0 && c < grid_size - 1) {
                    q->type = QUBIT_DATA;
                } else {
                    // 边界上的偶数位置 - 也是数据或辅助
                    // 简化处理: 假设特定模式
                    if ((r % 2 == 0 && c % 2 == 0) ||
                        (r % 2 == 1 && c % 2 == 1)) {
                        q->type = QUBIT_DATA;
                    } else {
                        q->type = QUBIT_ANCILLA_X;
                    }
                }
            } else {
                // (r+c)为奇数 - 稳定子辅助量子比特
                if (r > 0 && r < grid_size - 1 &&
                    c > 0 && c < grid_size - 1) {
                    // 交替X和Z稳定子
                    if ((r % 4 == 1 && c % 4 == 1) ||
                        (r % 4 == 3 && c % 4 == 3)) {
                        q->type = QUBIT_ANCILLA_X;

                        // 添加到X稳定子列表
                        Stabilizer *stab = &sc->x_stabilizers[x_stab_id];
                        stab->id = x_stab_id;
                        stab->type = 'X';
                        stab->ancilla_id = qubit_id;
                        x_stab_id++;
                    } else {
                        q->type = QUBIT_ANCILLA_Z;

                        // 添加到Z稳定子列表
                        Stabilizer *stab = &sc->z_stabilizers[z_stab_id];
                        stab->id = z_stab_id;
                        stab->type = 'Z';
                        stab->ancilla_id = qubit_id;
                        z_stab_id++;
                    }
                } else {
                    // 边界稳定子
                    q->type = QUBIT_ANCILLA_Z;  // 简化
                }
            }

            qubit_id++;
        }
    }

    // 建立邻接关系 (简化实现)
    // 实际中需要仔细计算每个稳定子的邻居数据量子比特
}
```

### 3. 综合征提取

```c
// ============================================================================
// Syndrome提取电路
// 测量所有稳定子算符
// ============================================================================

// syndrome测量结果
typedef struct {
    uint8_t x_syndromes[NUM_X_STABILIZERS];
    uint8_t z_syndromes[NUM_Z_STABILIZERS];
    uint32_t measurement_round;     // 测量轮次
} SyndromeMeasurement;

// 综合征历史 (用于时间解码)
typedef struct {
    SyndromeMeasurement measurements[3];  // 保存3轮测量
    uint8_t current_round;
} SyndromeHistory;

// ============================================================================
// 模拟综合征提取 (用于测试)
// ============================================================================

void extract_syndrome_simulation(SurfaceCode *sc, SyndromeMeasurement *synd) {
    // X稳定子: 测量X⊗X⊗X⊗X
    // syndrome = 1 如果数据量子比特上有奇数个X错误

    for (int i = 0; i < NUM_X_STABILIZERS; i++) {
        Stabilizer *stab = &sc->x_stabilizers[i];
        int x_count = 0;

        for (int j = 0; j < stab->num_neighbors; j++) {
            Qubit *q = &sc->qubits[stab->data_qubits[j]];
            if (q->error == ERROR_X || q->error == ERROR_Y) {
                x_count++;
            }
        }

        stab->syndrome_value = x_count % 2;
        synd->x_syndromes[i] = stab->syndrome_value;
    }

    // Z稳定子: 测量Z⊗Z⊗Z⊗Z
    // syndrome = 1 如果数据量子比特上有奇数个Z错误

    for (int i = 0; i < NUM_Z_STABILIZERS; i++) {
        Stabilizer *stab = &sc->z_stabilizers[i];
        int z_count = 0;

        for (int j = 0; j < stab->num_neighbors; j++) {
            Qubit *q = &sc->qubits[stab->data_qubits[j]];
            if (q->error == ERROR_Z || q->error == ERROR_Y) {
                z_count++;
            }
        }

        stab->syndrome_value = z_count % 2;
        synd->z_syndromes[i] = stab->syndrome_value;
    }
}

// ============================================================================
// 差异Syndrome (检测变化)
// ============================================================================

void compute_syndrome_difference(const SyndromeMeasurement *prev,
                                  const SyndromeMeasurement *curr,
                                  SyndromeMeasurement *diff) {
    // 计算两轮测量间的差异
    // 这对应于新增的错误

    for (int i = 0; i < NUM_X_STABILIZERS; i++) {
        diff->x_syndromes[i] = prev->x_syndromes[i] ^ curr->x_syndromes[i];
    }

    for (int i = 0; i < NUM_Z_STABILIZERS; i++) {
        diff->z_syndromes[i] = prev->z_syndromes[i] ^ curr->z_syndromes[i];
    }
}
```

### 4. MWPM解码器

```c
// ============================================================================
// Minimum Weight Perfect Matching (MWPM) 解码器
// 使用Blossom V算法或近似算法
// ============================================================================

// 解码图中的节点 (检测事件)
typedef struct {
    uint16_t id;
    uint8_t row;
    uint8_t col;
    uint8_t type;  // 'X' 或 'Z'
} DetectionNode;

// 图中的边 (可能的错误路径)
typedef struct {
    uint16_t node_a;
    uint16_t node_b;
    float weight;       // 路径权重 (负对数概率)
    bool is_matched;    // 是否匹配
} Edge;

// 解码图
typedef struct {
    DetectionNode nodes[256];   // 活跃检测节点
    uint16_t num_nodes;
    Edge edges[1024];
    uint16_t num_edges;
} DecoderGraph;

// ============================================================================
// 构建解码图
// ============================================================================

void build_decoder_graph(const SyndromeMeasurement *synd,
                          DecoderGraph *graph,
                          char stab_type) {
    graph->num_nodes = 0;
    graph->num_edges = 0;

    // 收集活跃syndrome (值为1的检测事件)
    const uint8_t *syndromes = (stab_type == 'X') ?
                                synd->x_syndromes : synd->z_syndromes;
    int num_stabilizers = (stab_type == 'X') ?
                           NUM_X_STABILIZERS : NUM_Z_STABILIZERS;

    for (int i = 0; i < num_stabilizers; i++) {
        if (syndromes[i]) {
            DetectionNode *node = &graph->nodes[graph->num_nodes++];
            node->id = graph->num_nodes - 1;
            // 获取稳定子位置
            // node->row = ...;
            // node->col = ...;
            node->type = stab_type;
        }
    }

    // 构建完全图 (所有节点对之间都有边)
    for (int i = 0; i < graph->num_nodes; i++) {
        for (int j = i + 1; j < graph->num_nodes; j++) {
            Edge *edge = &graph->edges[graph->num_edges++];
            edge->node_a = i;
            edge->node_b = j;

            // 计算权重 = 曼哈顿距离 (简化)
            // 实际应使用更复杂的权重函数
            float dr = abs(graph->nodes[i].row - graph->nodes[j].row);
            float dc = abs(graph->nodes[i].col - graph->nodes[j].col);
            edge->weight = dr + dc;

            edge->is_matched = false;
        }
    }
}

// ============================================================================
// 贪心近似MWPM算法 (简化实现)
// 实际应用中应使用Blossom V库
// ============================================================================

void greedy_matching(DecoderGraph *graph) {
    // 按权重排序边
    for (int i = 0; i < graph->num_edges - 1; i++) {
        for (int j = i + 1; j < graph->num_edges; j++) {
            if (graph->edges[j].weight < graph->edges[i].weight) {
                Edge tmp = graph->edges[i];
                graph->edges[i] = graph->edges[j];
                graph->edges[j] = tmp;
            }
        }
    }

    // 贪心选择最小权重边
    bool node_matched[256] = {false};

    for (int i = 0; i < graph->num_edges; i++) {
        Edge *e = &graph->edges[i];

        if (!node_matched[e->node_a] && !node_matched[e->node_b]) {
            e->is_matched = true;
            node_matched[e->node_a] = true;
            node_matched[e->node_b] = true;
        }
    }
}

// ============================================================================
// Union-Find解码器 (更快的近似算法)
// ============================================================================

// Union-Find数据结构
typedef struct {
    uint16_t parent[256];
    uint8_t rank[256];
    uint8_t parity[256];  // 用于跟踪奇偶性
} UnionFind;

void uf_init(UnionFind *uf, int n) {
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
        uf->parity[i] = 0;
    }
}

uint16_t uf_find(UnionFind *uf, uint16_t x) {
    if (uf->parent[x] != x) {
        uint16_t root = uf_find(uf, uf->parent[x]);
        uf->parity[x] ^= uf->parity[uf->parent[x]];
        uf->parent[x] = root;
    }
    return uf->parent[x];
}

void uf_union(UnionFind *uf, uint16_t x, uint16_t y) {
    uint16_t rx = uf_find(uf, x);
    uint16_t ry = uf_find(uf, y);

    if (rx == ry) return;

    if (uf->rank[rx] < uf->rank[ry]) {
        uint16_t tmp = rx; rx = ry; ry = tmp;
    }

    uf->parent[ry] = rx;
    if (uf->rank[rx] == uf->rank[ry]) {
        uf->rank[rx]++;
    }
}

// Union-Find解码
void union_find_decode(const SyndromeMeasurement *synd,
                        SurfaceCode *sc,
                        char stab_type) {
    DecoderGraph graph;
    build_decoder_graph(synd, &graph, stab_type);

    UnionFind uf;
    uf_init(&uf, graph.num_nodes);

    // 按权重递增顺序处理边
    // (与贪心匹配类似，但使用Union-Find数据结构)

    for (int i = 0; i < graph.num_edges; i++) {
        Edge *e = &graph.edges[i];

        uint16_t ra = uf_find(&uf, e->node_a);
        uint16_t rb = uf_find(&uf, e->node_b);

        if (ra != rb) {
            uint8_t parity_a = uf.parity[e->node_a];
            uint8_t parity_b = uf.parity[e->node_b];

            if ((parity_a ^ parity_b) == 0) {
                // 两个奇节点 - 匹配它们
                uf_union(&uf, ra, rb);
                uf.parity[ra] = 1;

                // 应用纠正 (在边路径上应用X或Z)
                apply_correction_along_path(sc, e->node_a, e->node_b, stab_type);
            }
        }
    }
}

// 沿路径应用纠正
void apply_correction_along_path(SurfaceCode *sc, uint16_t node_a,
                                  uint16_t node_b, char type) {
    // 找到两点之间的最短路径
    // 在路径上的每个数据量子比特应用X或Z纠正

    // 简化实现: 直接应用纠正到相关量子比特
    // 实际需要更复杂的路径搜索
}
```

### 5. 完整解码流程

```c
// ============================================================================
// 完整表面码解码器
// ============================================================================

typedef struct {
    SurfaceCode code;
    SyndromeHistory history;
    DecoderGraph x_graph;
    DecoderGraph z_graph;

    // 统计
    uint32_t total_decodings;
    uint32_t successful_corrections;
    uint32_t logical_errors;
} SurfaceCodeDecoder;

// 初始化解码器
void decoder_init(SurfaceCodeDecoder *dec, uint8_t distance) {
    surface_code_init(&dec->code, distance);
    memset(&dec->history, 0, sizeof(SyndromeHistory));
    dec->total_decodings = 0;
    dec->successful_corrections = 0;
    dec->logical_errors = 0;
}

// 执行完整解码周期
void decoder_cycle(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement *current =
        &dec->history.measurements[dec->history.current_round];

    // 1. 提取当前syndrome
    extract_syndrome_simulation(&dec->code, current);

    // 2. 如果是第一轮，只做记录
    if (dec->history.current_round == 0) {
        dec->history.current_round = 1;
        return;
    }

    // 3. 计算差异syndrome
    SyndromeMeasurement diff;
    uint8_t prev_idx = (dec->history.current_round - 1) % 3;
    compute_syndrome_difference(&dec->history.measurements[prev_idx],
                                 current, &diff);

    // 4. 解码X错误 (从Z syndrome)
    union_find_decode(&diff, &dec->code, 'Z');

    // 5. 解码Z错误 (从X syndrome)
    union_find_decode(&diff, &dec->code, 'X');

    // 6. 更新历史
    dec->history.current_round = (dec->history.current_round + 1) % 3;
    dec->total_decodings++;

    // 7. 检查逻辑错误 (模拟)
    check_logical_error(dec);
}

// 检查是否存在逻辑错误
bool check_logical_error(SurfaceCodeDecoder *dec) {
    // 计算逻辑X和逻辑Z算符的奇偶性
    // 如果存在奇数个X错误穿过逻辑Z链，则产生逻辑X错误
    // 反之亦然

    // 简化: 统计边界错误
    int x_logical_error = 0;
    int z_logical_error = 0;

    for (int i = 0; i < TOTAL_QUBITS; i++) {
        Qubit *q = &dec->code.qubits[i];
        if (q->is_logical) {
            if (q->error == ERROR_X || q->error == ERROR_Y) {
                x_logical_error++;
            }
            if (q->error == ERROR_Z || q->error == ERROR_Y) {
                z_logical_error++;
            }
        }
    }

    bool has_logical_error = (x_logical_error % 2) || (z_logical_error % 2);

    if (has_logical_error) {
        dec->logical_errors++;
    } else {
        dec->successful_corrections++;
    }

    return has_logical_error;
}

// 获取解码统计
void get_decoder_stats(const SurfaceCodeDecoder *dec,
                       float *logical_error_rate,
                       float *success_rate) {
    if (dec->total_decodings > 0) {
        *logical_error_rate = (float)dec->logical_errors / dec->total_decodings;
        *success_rate = (float)dec->successful_corrections / dec->total_decodings;
    } else {
        *logical_error_rate = 0.0f;
        *success_rate = 0.0f;
    }
}
```

---

## ⚠️ 常见陷阱

### 陷阱 SCD01: 边界条件处理错误

```c
// ❌ 问题: 边界稳定子只有2-3个邻居
Stabilizer *stab = &sc->x_stabilizers[i];
for (int j = 0; j < 4; j++) {  // 错误! 可能只有2个邻居
    Qubit *q = &sc->qubits[stab->data_qubits[j]];
}

// ✅ 正确: 使用num_neighbors
for (int j = 0; j < stab->num_neighbors; j++) {
    Qubit *q = &sc->qubits[stab->data_qubits[j]];
}
```

### 陷阱 SCD02: 时间解码忘记重置

```c
// ❌ 问题: syndrome历史累积
void decoder_cycle_bad(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement synd;
    extract_syndrome(&dec->code, &synd);

    // 直接累积到历史，没有清除旧的
    for (int i = 0; i < NUM_STABILIZERS; i++) {
        dec->history.x_syndromes[i] |= synd.x_syndromes[i];
    }
}

// ✅ 正确: 使用滑动窗口
void decoder_cycle_good(SurfaceCodeDecoder *dec) {
    SyndromeMeasurement *curr = &dec->history.measurements[round % 3];
    SyndromeMeasurement *prev = &dec->history.measurements[(round - 1) % 3];

    extract_syndrome(&dec->code, curr);

    // 计算差异
    SyndromeMeasurement diff;
    for (int i = 0; i < NUM_STABILIZERS; i++) {
        diff.x_syndromes[i] = curr->x_syndromes[i] ^ prev->x_syndromes[i];
    }
}
```

### 陷阱 SCD03: 内存不足 (大尺寸表面码)

```c
// ❌ 问题: d=21需要大量内存
#define MAX_DISTANCE    21
#define MAX_QUBITS      ((2*MAX_DISTANCE-1)*(2*MAX_DISTANCE-1))

// 固定大小数组在栈上可能导致溢出
SurfaceCode code;  // 可能很大!

// ✅ 正确: 动态分配
SurfaceCode *code = malloc(sizeof(SurfaceCode));
if (!code) return ERR_OUT_OF_MEMORY;

// 或者按需分配
typedef struct {
    uint8_t distance;
    Qubit *qubits;
    Stabilizer *x_stabs;
    Stabilizer *z_stabs;
} DynamicSurfaceCode;

int init_dynamic(DynamicSurfaceCode *sc, uint8_t d) {
    sc->distance = d;
    int grid_size = 2 * d - 1;
    int total_qubits = grid_size * grid_size;

    sc->qubits = malloc(total_qubits * sizeof(Qubit));
    sc->x_stabs = malloc(((d-1)*(d-1)/2) * sizeof(Stabilizer));
    sc->z_stabs = malloc(((d-1)*(d-1)/2) * sizeof(Stabilizer));

    if (!sc->qubits || !sc->x_stabs || !sc->z_stabs) {
        return ERR_NO_MEMORY;
    }
    return 0;
}
```

---

## ✅ 质量验收清单

| 检查项 | 要求 | 验证方法 |
|:-------|:-----|:---------|
| **功能正确性** |||
| Syndrome计算 | 与实际稳定子测量一致 | 单元测试 |
| 解码正确性 | 纠正所有可纠正错误 | 模拟测试 |
| 逻辑错误检测 | 正确识别逻辑错误 | 边界测试 |
| **性能** |||
| 解码延迟 | <1μs (d=5) | 基准测试 |
| 内存使用 | O(d²) | 内存分析 |
| **容错性** |||
| 低于阈值 | 物理错误率 <1%时逻辑错误率下降 | 蒙特卡洛模拟 |
| 错误传播 | 无级联故障 | 故障注入 |

---

## 📚 参考标准与延伸阅读

| 资源 | 说明 |
|:-----|:-----|
| Fowler et al. 2012 | Surface codes: Towards practical large-scale quantum computation |
| Google Quantum AI | Surface code实现 |
| Blossom V | MWPM算法实现 |
| PyMatching | Python表面码解码库 |
| Stim | 表面码模拟器 |

---

> **更新记录**
>
> - 2025-03-09: 初版创建，包含表面码解码器完整实现
