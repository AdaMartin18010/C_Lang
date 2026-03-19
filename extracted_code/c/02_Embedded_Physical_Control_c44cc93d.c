/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 893
 * Language: c
 * Block ID: c44cc93d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// CAN: 控制器局域网，汽车和工业控制标准
// 帧格式: 标识符 + 数据长度码 + 数据(0-8字节)
// ============================================================

typedef struct {
    uint32_t id;        // 标识符 (11位标准或29位扩展)
    uint8_t is_ext;     // 是否扩展帧
    uint8_t is_rtr;     // 是否远程帧
    uint8_t dlc;        // 数据长度码 (0-8)
    uint8_t data[8];    // 数据
} CAN_Frame;

// 发送CAN帧
int can_send(CAN_TypeDef *can, const CAN_Frame *frame) {
    // 找到空的发送邮箱
    uint32_t tsr = can->TSR;
    uint32_t mailbox;

    if (tsr & CAN_TSR_TME0) {
        mailbox = 0;
    } else if (tsr & CAN_TSR_TME1) {
        mailbox = 1;
    } else if (tsr & CAN_TSR_TME2) {
        mailbox = 2;
    } else {
        return -1;  // 无空邮箱
    }

    volatile uint32_t *tir = &can->sTxMailBox[mailbox].TIR;
    volatile uint32_t *tdtr = &can->sTxMailBox[mailbox].TDTR;
    volatile uint32_t *tdlr = &can->sTxMailBox[mailbox].TDLR;
    volatile uint32_t *tdhr = &can->sTxMailBox[mailbox].TDHR;

    // 配置标识符
    uint32_t id_reg;
    if (frame->is_ext) {
        id_reg = (frame->id << 3) | CAN_ID_EXT;
    } else {
        id_reg = (frame->id << 21);
    }
    if (frame->is_rtr) {
        id_reg |= CAN_RTR_REMOTE;
    }

    *tir = id_reg;
    *tdtr = frame->dlc;

    // 填充数据 (小端)
    uint32_t low = frame->data[0] | (frame->data[1] << 8) |
                   (frame->data[2] << 16) | (frame->data[3] << 24);
    uint32_t high = frame->data[4] | (frame->data[5] << 8) |
                    (frame->data[6] << 16) | (frame->data[7] << 24);
    *tdlr = low;
    *tdhr = high;

    // 请求发送
    *tir |= CAN_TI0R_TXRQ;

    return 0;
}

// CAN过滤器配置 (接收特定ID)
void can_filter_config(CAN_TypeDef *can, uint8_t bank, uint32_t id, uint32_t mask) {
    // 进入过滤器初始化模式
    can->FMR |= CAN_FMR_FINIT;

    // 选择过滤器模式: 掩码模式
    can->FS1R |= (1U << bank);  // 32位模式
    can->FM1R &= ~(1U << bank); // 掩码模式 (非列表)

    // 配置ID和掩码
    can->sFilterRegister[bank].FR1 = (id << 21);      // 标准ID左移21位
    can->sFilterRegister[bank].FR2 = (mask << 21);    // 掩码

    // 使能过滤器
    can->FA1R |= (1U << bank);

    // 退出初始化模式
    can->FMR &= ~CAN_FMR_FINIT;
}
