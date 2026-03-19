/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\IEC_61508\01_SIL_Implementation.md
 * Line: 269
 * Language: c
 * Block ID: 3f03f8d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SIL 3 1oo2架构实现
 */

typedef struct {
    bool output;
    bool error;
    uint32_t diag_status;
} ChannelOutput;

/* 两个独立通道 */
ChannelOutput channel_1_process(bool input)
{
    ChannelOutput out = {0};

    /* 通道1独立处理 */
    if (diagnostic_self_test()) {
        out.output = process_logic_a(input);
        out.diag_status = DIAG_OK;
    } else {
        out.error = true;
        out.diag_status = DIAG_FAULT;
    }

    return out;
}

ChannelOutput channel_2_process(bool input)
{
    ChannelOutput out = {0};

    /* 通道2独立处理（不同算法） */
    if (diagnostic_self_test_b()) {
        out.output = process_logic_b(input);
        out.diag_status = DIAG_OK;
    } else {
        out.error = true;
        out.diag_status = DIAG_FAULT;
    }

    return out;
}

/* 输出比较和表决 */
bool vote_1oo2(bool input)
{
    ChannelOutput ch1 = channel_1_process(input);
    ChannelOutput ch2 = channel_2_process(input);

    /* 如果任一通道有故障 */
    if (ch1.error || ch2.error) {
        log_fault(ch1.error ? CHANNEL_1_FAULT : CHANNEL_2_FAULT);
    }

    /* 比较输出 */
    if (ch1.output == ch2.output) {
        return ch1.output;  /* 一致 */
    }

    /* 不一致 - 进入安全状态 */
    enter_safe_state();
    return SAFE_STATE_OUTPUT;
}
