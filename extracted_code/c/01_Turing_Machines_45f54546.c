/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\07_Computability_Theory\01_Turing_Machines.md
 * Line: 229
 * Language: c
 * Block ID: 45f54546
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 识别 {0^n1^n | n ≥ 0} 的图灵机
// 算法：用X替换0，用Y替换1，交叉匹配

TuringMachine* create_0n1n_machine() {
    TuringMachine *tm = calloc(1, sizeof(TuringMachine));

    // 状态定义：
    // 0: 初始，寻找第一个0
    // 1: 找到0，替换为X，向右寻找1
    // 2: 找到1，替换为Y，向左返回
    // 3: 返回完成，再次寻找0
    // 4: 检查是否还有未匹配的1
    // 5: 接受状态
    // 6: 拒绝状态

    tm->num_states = 7;
    tm->state_types[5] = STATE_ACCEPT;
    tm->state_types[6] = STATE_REJECT;

    // 状态0：初始状态
    tm_add_transition(tm, 0, '0', 1, 'X', RIGHT);  // 找到0，标记X
    tm_add_transition(tm, 0, 'Y', 4, 'Y', RIGHT);  // 已匹配部分，检查剩余
    tm_add_transition(tm, 0, '_', 5, '_', RIGHT);  // 空输入，接受

    // 状态1：寻找对应的1
    tm_add_transition(tm, 1, '0', 1, '0', RIGHT);  // 跳过其他0
    tm_add_transition(tm, 1, 'Y', 1, 'Y', RIGHT);  // 跳过已匹配的1
    tm_add_transition(tm, 1, '1', 2, 'Y', LEFT);   // 找到1，标记Y，返回
    tm_add_transition(tm, 1, '_', 6, '_', RIGHT);  // 未找到1，拒绝

    // 状态2：返回寻找下一个0
    tm_add_transition(tm, 2, '0', 2, '0', LEFT);
    tm_add_transition(tm, 2, 'Y', 2, 'Y', LEFT);
    tm_add_transition(tm, 2, 'X', 0, 'X', RIGHT);  // 回到状态0

    // 状态4：验证没有剩余的0
    tm_add_transition(tm, 4, 'Y', 4, 'Y', RIGHT);  // 跳过Y
    tm_add_transition(tm, 4, '_', 5, '_', RIGHT);  // 到达末尾，接受
    tm_add_transition(tm, 4, '0', 6, '0', RIGHT);  // 发现0，拒绝（0在1后）

    return tm;
}

// 测试
void test_0n1n() {
    TuringMachine *tm = create_0n1n_machine();

    const char *test_cases[] = {
        "",           // ε - 接受
        "01",         // n=1 - 接受
        "0011",       // n=2 - 接受
        "000111",     // n=3 - 接受
        "0",          // 拒绝
        "1",          // 拒绝
        "10",         // 拒绝
        "001",        // 拒绝
        "011",        // 拒绝
    };

    for (int i = 0; i < 9; i++) {
        tm_init(tm, test_cases[i]);
        int result = tm_run(tm, 1000);
        printf("Input: %-8s -> %s\n",
               test_cases[i],
               result == 1 ? "ACCEPT" :
               result == -1 ? "REJECT" : "UNKNOWN");
    }

    free(tm);
}
