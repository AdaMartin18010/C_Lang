/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 182
 * Language: c
 * Block ID: 325beba9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 天机芯混合计算架构
typedef enum {
    MODE_ANN,    // 人工神经网络模式
    MODE_SNN,    // 脉冲神经网络模式
    MODE_HYBRID  // 混合模式
} TianjicMode;

typedef struct {
    TianjicMode mode;
    union {
        // ANN 部分
        struct {
            float weights[16][16];
            float bias[16];
            int activation_type;
        } ann;

        // SNN 部分
        struct {
            int16_t weights[16][16];
            int16_t threshold;
            int8_t leak;
            int8_t delay;
        } snn;
    };
} TianjicCore;
