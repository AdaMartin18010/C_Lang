/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\README.md
 * Line: 842
 * Language: c
 * Block ID: 7b217dd3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* nr_test_vectors.h - 测试向量 */
#ifndef NR_TEST_VECTORS_H
#define NR_TEST_VECTORS_H

/* 3GPP 38.141测试向量 */
extern const cf_t TEST_PSS_SIGNAL[];
extern const cf_t TEST_SSS_SIGNAL[];
extern const uint8_t TEST_PBCH_PAYLOAD[];

/* 信道模型 */
typedef enum {
    CHANNEL_TDLA_30NS = 0,
    CHANNEL_TDLB_100NS,
    CHANNEL_TDLC_300NS,
    CHANNEL_TDLA_30_7,
    CHANNEL_TDLB_100_7,
    CHANNEL_TDLC_300_7
} channel_model_t;

void apply_channel_model(cf_t *signal, uint32_t len, channel_model_t model);

#endif /* NR_TEST_VECTORS_H */
