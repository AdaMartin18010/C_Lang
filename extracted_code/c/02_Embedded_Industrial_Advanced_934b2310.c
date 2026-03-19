/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 703
 * Language: c
 * Block ID: 934b2310
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// EtherCAT: 工业以太网，实时性极高 (<100us周期)
// 使用ESC芯片 (如LAN9252) + MCU
// ============================================================

#include "esc.h"  // SOES (Simple Open EtherCAT Slave)库

// EtherCAT对象字典 (CoE - CANopen over EtherCAT)
// 定义了过程数据 (PDO) 和 服务数据 (SDO)

// 过程数据映射
volatile struct {
    uint16_t control_word;      // 0x6040
    int32_t  target_position;   // 0x607A
    int32_t  target_velocity;   // 0x60FF
    int16_t  target_torque;     // 0x6071
} RPDO;  // 接收PDO (主站→从站)

volatile struct {
    uint16_t status_word;       // 0x6041
    int32_t  actual_position;   // 0x6064
    int32_t  actual_velocity;   // 0x606C
    int16_t  actual_torque;     // 0x6077
} TPDO;  // 发送PDO (从站→主站)

// ESC配置
void esc_init(void) {
    // 初始化SPI接口到ESC芯片
    spi_init(ESC_SPI_PORT, ESC_SPI_BAUDRATE);

    // 复位ESC
    esc_reset();

    // 等待ESC就绪
    while (!esc_ready());

    // 配置PDI (Process Data Interface)
    esc_write_reg(ESC_PDI_CONTROL, PDI_SPI_MODE);

    // 配置同步管理器 (Sync Manager)
    // SM0: 邮箱接收 (主站→从站)
    // SM1: 邮箱发送 (从站→主站)
    // SM2: 过程数据输出 (PDO)
    // SM3: 过程数据输入 (PDO)

    esc_configure_sm(2, SM2_PHYS_ADDR, SM2_LENGTH, SM2_CONTROL);
    esc_configure_sm(3, SM3_PHYS_ADDR, SM3_LENGTH, SM3_CONTROL);

    // 配置FMMU (Fieldbus Memory Management Unit)
    // 将逻辑地址映射到物理地址
    esc_configure_fmmu(0, FMMU0_LOGIC_ADDR, FMMU0_LENGTH, FMMU0_PHYS_ADDR);
    esc_configure_fmmu(1, FMMU1_LOGIC_ADDR, FMMU1_LENGTH, FMMU1_PHYS_ADDR);
}

// EtherCAT主循环
void esc_main_loop(void) {
    while (1) {
        // 读取ESC状态
        uint16_t al_status = esc_read_reg(ESC_AL_STATUS);

        switch (al_status & 0x0F) {
            case ESC_STATE_INIT:
                // 初始化状态，等待配置
                break;

            case ESC_STATE_PREOP:
                // 预操作状态，配置PDO映射
                esc_configure_pdo();
                esc_set_state(ESC_STATE_SAFEOP);
                break;

            case ESC_STATE_SAFEOP:
                // 安全操作状态，可以传输PDO，但不控制
                break;

            case ESC_STATE_OP:
                // 操作状态，完全控制
                // 读取RPDO (主站命令)
                esc_read_pdo(&RPDO, sizeof(RPDO));

                // 执行控制算法...
                motor_control(RPDO.target_position, RPDO.target_velocity);

                // 更新TPDO (反馈数据)
                TPDO.actual_position = encoder_read();
                TPDO.actual_velocity = velocity_calc();
                TPDO.status_word = build_status_word();

                // 写回TPDO
                esc_write_pdo(&TPDO, sizeof(TPDO));
                break;
        }

        // 处理邮箱通信 (SDO)
        esc_process_mailbox();
    }
}

// DC (Distributed Clock) 同步
// EtherCAT的分布式时钟允许所有从站精确同步 (<1us)
void esc_dc_sync(void) {
    // 读取本地时间
    uint64_t local_time = esc_read_dc_time();

    // 读取接收时间戳
    uint64_t rx_time = esc_read_reg(ESC_RECEIVE_TIME);

    // 计算偏移并调整
    int64_t offset = calculate_offset(local_time, rx_time);
    esc_adjust_dc_clock(offset);
}
