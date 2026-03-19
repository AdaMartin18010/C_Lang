/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_GPU_Memory_Management.md
 * Line: 382
 * Language: c
 * Block ID: ac5e6b55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

float *h_data = (float*)malloc(size);
float *d_data;
cudaMalloc(&d_data, size);

// 错误：在设备指针上使用主机函数
free(d_data);  // 崩溃！

// 正确
cudaFree(d_data);
free(h_data);
