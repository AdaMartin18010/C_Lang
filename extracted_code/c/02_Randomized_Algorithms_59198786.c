/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 428
 * Language: c
 * Block ID: 59198786
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 水库采样：从数据流中均匀采样k个元素
#include <stdlib.h>

typedef struct {
    int *sample;
    int k;
    int count;  // 已处理元素数
} ReservoirSampler;

ReservoirSampler *reservoir_create(int k) {
    ReservoirSampler *r = malloc(sizeof(ReservoirSampler));
    r->sample = malloc(k * sizeof(int));
    r->k = k;
    r->count = 0;
    return r;
}

void reservoir_add(ReservoirSampler *r, int item) {
    if (r->count < r->k) {
        // 填满水库
        r->sample[r->count] = item;
    } else {
        // 以k/count概率替换
        int j = rand() % (r->count + 1);
        if (j < r->k) {
            r->sample[j] = item;
        }
    }
    r->count++;
}

int *reservoir_get(ReservoirSampler *r) {
    return r->sample;
}
