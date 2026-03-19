/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 950
 * Language: c
 * Block ID: a52d5045
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 信任中心（协调器）安全功能

typedef struct {
    uint8_t active;                       // 信任中心使能
    uint8_t allow_rejoin;                 // 允许重新加入
    uint8_t allow_remote_tc_policy_change; // 允许远程TC策略更改
    uint8_t network_key[16];              // 当前网络密钥
    uint32_t network_key_seq_num;         // 网络密钥序列号
    uint8_t network_key_type;             // 密钥类型
    uint32_t frame_counter;               // 帧计数器
} trust_center_t;

// 设备加入流程（安全）
void tc_device_join_security(uint64_t ext_addr, nwk_addr_t nwk_addr) {
    // 1. 验证设备是否允许加入
    if (!is_device_allowed(ext_addr)) {
        return;
    }

    // 2. 获取或生成链接密钥
    uint8_t link_key[16];
    if (have_preconfigured_link_key(ext_addr)) {
        get_preconfigured_link_key(ext_addr, link_key);
    } else {
        // 使用默认全局链接密钥（不安全，仅用于测试）
        memcpy(link_key, default_global_link_key, 16);
    }

    // 3. 使用链接密钥加密传输网络密钥
    uint8_t encrypted_nwk_key[18];
    encrypt_transport_key(link_key, tc.network_key, encrypted_nwk_key);

    // 4. 发送APSME-TRANSPORT-KEY.indication
    send_transport_key(nwk_addr, KEY_TYPE_NWK, encrypted_nwk_key);

    // 5. 更新设备表
    add_device_to_network(ext_addr, nwk_addr);
}
