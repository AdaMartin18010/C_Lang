/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_GPU_Memory_Management.md
 * Line: 361
 * Language: c
 * Block ID: 6b736528
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误
float *d_data;
cudaMalloc(&d_data, size);  // 失败时无提示
cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

// 正确
#define cudaCheckError(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line) {
    if (code != cudaSuccess) {
        fprintf(stderr, "GPUassert: %s %s %d\n",
                cudaGetErrorString(code), file, line);
        exit(code);
    }
}

cudaCheckError(cudaMalloc(&d_data, size));
