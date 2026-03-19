/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\19_Hardware_Debugging\01_JTAG_Debug.md
 * Line: 1101
 * Language: c
 * Block ID: 1d0460b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 互连测试算法 -  walking ones/zeros
 * 用于检测开路和短路故障
 */

// Walking Ones 测试向量生成
void generate_walking_ones_vectors(uint32_t *vectors, int num_nets) {
    for (int i = 0; i < num_nets; i++) {
        vectors[i] = (1U << i);  // 只有一个1，其余为0
    }
}

// Walking Zeros 测试向量生成
void generate_walking_zeros_vectors(uint32_t *vectors, int num_nets) {
    for (int i = 0; i < num_nets; i++) {
        vectors[i] = ~(1U << i);  // 只有一个0，其余为1
    }
}

// 完整互连测试函数
int interconnect_test(jtag_chain_t *chain,
                      netlist_t *netlist,
                      test_report_t *report) {
    int num_nets = netlist->num_nets;
    uint32_t *drive_vectors = malloc(num_nets * sizeof(uint32_t));
    uint32_t *expected = malloc(num_nets * sizeof(uint32_t));
    uint32_t *captured = malloc(num_nets * sizeof(uint32_t));

    // 生成测试向量
    generate_walking_ones_vectors(drive_vectors, num_nets);

    // 加载 EXTEST 指令到所有芯片
    jtag_ir_scan(chain, EXTEST_OPCODE, NULL);

    // 执行测试
    for (int test_num = 0; test_num < num_nets; test_num++) {
        // 预加载驱动值
        jtag_dr_scan(chain, drive_vectors[test_num], NULL);

        // 应用测试向量（Update-DR 自动完成）
        jtag_run_idle(chain, 1);

        // 捕获响应
        jtag_dr_scan(chain, 0, captured);

        // 计算期望响应
        expected[test_num] = calculate_expected_response(
            netlist, drive_vectors[test_num]
        );

        // 验证结果
        if (captured[test_num] != expected[test_num]) {
            report_fault(report, test_num,
                        drive_vectors[test_num],
                        expected[test_num],
                        captured[test_num]);
        }
    }

    free(drive_vectors);
    free(expected);
    free(captured);

    return report->num_faults;
}
