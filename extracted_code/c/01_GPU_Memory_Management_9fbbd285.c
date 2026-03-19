/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_GPU_Memory_Management.md
 * Line: 397
 * Language: c
 * Block ID: 9fbbd285
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void leaky_function() {
    float *d_temp;
    cudaMalloc(&d_temp, 1024 * sizeof(float));
    // 忘记 cudaFree(d_temp)!
}

// 使用RAII模式或确保配对
void clean_function() {
    float *d_temp;
    cudaMalloc(&d_temp, 1024 * sizeof(float));
    // ... 使用 ...
    cudaFree(d_temp);  // 确保释放
}
