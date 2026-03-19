/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 96
 * Language: c
 * Block ID: 9184983b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 完整的switch结构 */
switch (command) {
    case CMD_START:
        handle_start();
        break;

    case CMD_STOP:
        handle_stop();
        break;

    case CMD_PAUSE:
        handle_pause();
        break;

    default:  /* 总是最后 */
        handle_unknown_command(command);
        break;
}

/* ✅ 合规 - 枚举类型的穷尽检查 */
enum State { STATE_IDLE, STATE_RUNNING, STATE_ERROR };

void handle_state(enum State s)
{
    switch (s) {
        case STATE_IDLE:
            /* 处理 */
            break;
        case STATE_RUNNING:
            /* 处理 */
            break;
        case STATE_ERROR:
            /* 处理 */
            break;

        default:
            /* 防御性：不应到达 */
            log_error("Invalid state: %d", s);
            break;
    }
}

/* ✅ 合规 - 故意fall-through */
switch (packet.type) {
    case TYPE_CONTROL:
        process_control_header();
        /* fall-through - 控制包也需要处理数据 */

    case TYPE_DATA:
        process_data_payload();
        break;

    default:
        drop_packet();
        break;
}
