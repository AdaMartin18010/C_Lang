/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\04_Cache_Array.md
 * Line: 970
 * Language: c
 * Block ID: 252d78db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// Cache友好的C代码示例
// ============================================================

// 示例1: 数组访问顺序
#define N 1024
int A[N][N];

// Cache不友好的代码 (行优先 vs 列优先)
void bad_access() {
    // 列优先访问，Cache行利用率低
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            A[i][j] = i + j;  // 每次访问间隔N×4字节
        }
    }
}

// Cache友好的代码
void good_access() {
    // 行优先访问，充分利用Cache行
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;  // 连续访问，一次加载Cache行
        }
    }
}
/*
 * 性能差异：
 * - Cache行大小64字节，可存储16个int
 * - 行优先：每16次访问才产生1次Cache Miss
 * - 列优先：几乎每次访问都Miss
 * - 性能差距可达10-100倍
 */

// 示例2: 循环分块 (Loop Tiling)
void matrix_multiply_naive(int C[N][N], int A[N][N], int B[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
}

void matrix_multiply_tiled(int C[N][N], int A[N][N], int B[N][N]) {
    const int BLOCK = 64;  // 适应Cache大小
    for (int ii = 0; ii < N; ii += BLOCK)
        for (int jj = 0; jj < N; jj += BLOCK)
            for (int kk = 0; kk < N; kk += BLOCK)
                // 处理一个BLOCK×BLOCK的小块
                for (int i = ii; i < min(ii+BLOCK, N); i++)
                    for (int j = jj; j < min(jj+BLOCK, N); j++)
                        for (int k = kk; k < min(kk+BLOCK, N); k++)
                            C[i][j] += A[i][k] * B[k][j];
}
/*
 * 分块优势：
 * - 小块可以全部装入L1 Cache
 * - 减少Cache冲突
 * - 提高数据复用
 */

// 示例3: 数据结构对齐
struct bad_struct {
    char a;      // 1字节
    // 3字节填充
    int b;       // 4字节 (地址4对齐)
    char c;      // 1字节
    // 7字节填充
    double d;    // 8字节 (地址8对齐)
};  // 总大小: 24字节

struct good_struct {
    double d;    // 8字节
    int b;       // 4字节
    char a;      // 1字节
    char c;      // 1字节
    // 2字节填充
};  // 总大小: 16字节
/*
 * 优化效果：
 * - 减少内存占用
 * - 更好的Cache行对齐
 * - 可能的向量化优化
 */

// 示例4: 避免伪共享
struct alignas(64) padded_counter {  // C++17
    int value;
    char pad[60];  // 填充到64字节
};

padded_counter counters[4];  // 每个counter在不同Cache行

// 或者使用C语言方式
struct padded_counter_c {
    int value;
    char pad[60];
} __attribute__((aligned(64)));
/*
 * 伪共享消除：
 * - 确保多线程修改的不同变量在不同Cache行
 * - 避免不必要的Cache一致性流量
 */
