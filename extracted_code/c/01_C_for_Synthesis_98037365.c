/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 735
 * Language: c
 * Block ID: 98037365
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. 使用常量定义数据规模
#define DATA_SIZE 1024
#define FILTER_TAPS 16

// 2. 使用typedef提高可读性
typedef ap_fixed<16,8> data_t;
typedef ap_fixed<32,16> accum_t;

// 3. 函数参数顺序：输出在前，输入在后
void good_function(
    data_t *output,      // 输出参数在前
    const data_t *input, // 输入参数在后，const修饰
    int param            // 配置参数最后
);

// 4. 避免全局变量
// ❌ 不推荐
static int global_state;
void bad_function(int x) {
    global_state += x;
}

// ✅ 推荐：显式传递状态
void good_function(int *state, int x) {
    #pragma HLS INTERFACE mode=ap_vld port=state
    *state = *state + x;
}
