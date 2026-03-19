/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\README.md
 * Line: 1188
 * Language: c
 * Block ID: c9145ff1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BB84协议简化实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define KEY_LENGTH 128

/* 基选择: 0=Rectilinear(|0⟩,|1⟩), 1=Diagonal(|+⟩,|-⟩) */
typedef struct {
    int bits[KEY_LENGTH];       /* 原始比特 */
    int bases[KEY_LENGTH];      /* 选择的基 */
    int measurements[KEY_LENGTH]; /* 测量结果 */
} BB84Participant;

void bb84_protocol_demo(void) {
    printf("=== BB84 量子密钥分发协议 ===\n\n");

    BB84Participant alice = {0};
    BB84Participant bob = {0};

    /* Alice准备 */
    printf("1. Alice随机选择比特和基:\n");
    for (int i = 0; i < KEY_LENGTH; i++) {
        alice.bits[i] = rand() % 2;
        alice.bases[i] = rand() % 2;
    }
    printf("   生成 %d 个随机比特和基\n\n", KEY_LENGTH);

    /* Bob测量 */
    printf("2. Bob随机选择基进行测量:\n");
    for (int i = 0; i < KEY_LENGTH; i++) {
        bob.bases[i] = rand() % 2;
        /* 如果基相同，测量结果正确；否则50%概率 */
        if (bob.bases[i] == alice.bases[i]) {
            bob.measurements[i] = alice.bits[i];
        } else {
            bob.measurements[i] = rand() % 2;
        }
    }
    printf("   完成测量\n\n");

    /* 基比对 */
    printf("3. 公开比对基的选择:\n");
    int matching_bases = 0;
    for (int i = 0; i < KEY_LENGTH; i++) {
        if (alice.bases[i] == bob.bases[i]) {
            matching_bases++;
        }
    }
    printf("   基匹配率: ~50%% (%d/%d)\n\n", matching_bases, KEY_LENGTH);

    /* 密钥筛选 */
    printf("4. 保留基匹配的位置，形成原始密钥\n");
    printf("   预期密钥长度: ~%d 比特\n\n", matching_bases);

    /* 错误检测 */
    printf("5. 公开部分比特进行错误检测\n");
    printf("   如果错误率异常高，可能遭受窃听\n\n");

    /* 隐私放大 */
    printf("6. 隐私放大：从原始密钥提取安全密钥\n");
}

int main(void) {
    srand(time(NULL));
    bb84_protocol_demo();
    return 0;
}
