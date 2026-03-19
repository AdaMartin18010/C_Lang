/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 563
 * Language: c
 * Block ID: 2304cadd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 防止不必要的数据翻转传播
 */

// 未优化：所有操作数始终翻转
void power_no_isolation(int a[100], int b[100], int sel, int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int add_result = a[i] + b[i];
        int mul_result = a[i] * b[i];
        // 两个都计算，但只用一个
        out[i] = sel ? add_result : mul_result;
    }
}

// 优化：条件计算
void power_operand_isolation(int a[100], int b[100], int sel, int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        if (sel) {
            out[i] = a[i] + b[i];
        } else {
            out[i] = a[i] * b[i];
        }
    }
}
// 未使用的单元输入保持稳定

// 优化：存储器休眠
void power_memory_sleep(int *dram, int local_buf[1024]) {
    #pragma HLS INTERFACE mode=m_axi port=dram

    int active = 1;

    while (active) {
        #pragma HLS PIPELINE
        // 处理本地缓冲区
        process_local(local_buf);

        // 条件访问DRAM
        if (need_refresh()) {
            refresh_dram(dram, local_buf);
        }
    }
}
