/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 587
 * Language: c
 * Block ID: f546daf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 在多种条件下测试WCET
void wcet_validation_test(void) {
    uint32_t max_cycles = 0;

    // 测试输入范围边界
    q7_t test_inputs[][3] = {
        {0, 0, 0},           // 全零
        {127, 127, 127},     // 最大正值
        {-128, -128, -128},  // 最大负值
        {127, -128, 0},      // 混合
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1000; j++) {  // 多次运行
            uint32_t start = DWT_CYCCNT;
            deterministic_inference(test_inputs[i], output);
            uint32_t cycles = DWT_CYCCNT - start;

            if (cycles > max_cycles) {
                max_cycles = cycles;
            }
        }
    }

    printf("Measured WCET: %u cycles (%u us @ 480MHz)\n",
           max_cycles, max_cycles / 480);
}
