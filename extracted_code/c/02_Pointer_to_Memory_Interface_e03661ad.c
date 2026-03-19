/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 644
 * Language: c
 * Block ID: e03661ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 随机访问 - 使用缓存优化
void random_access(
    int *data,          // 主存储器
    int *indices,       // 索引数组
    int *results,
    int n
) {
    #pragma HLS INTERFACE mode=m_axi port=data
    #pragma HLS INTERFACE mode=m_axi port=indices

    // 缓存最近访问的数据
    int cache[16][2];  // [地址标签, 数据值]
    #pragma HLS ARRAY_PARTITION variable=cache complete

    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        int idx = indices[i];
        int tag = idx & 0xF;

        // 检查缓存
        int hit = 0;
        int cached_data;
        for (int j = 0; j < 16; j++) {
            #pragma HLS UNROLL
            if (cache[j][0] == tag) {
                cached_data = cache[j][1];
                hit = 1;
            }
        }

        int value = hit ? cached_data : data[idx];

        // 更新缓存
        cache[tag][0] = tag;
        cache[tag][1] = value;

        results[i] = process(value);
    }
}
