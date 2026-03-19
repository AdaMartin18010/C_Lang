/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 713
 * Language: c
 * Block ID: dc3f99d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// APS信息库
typedef struct {
    uint8_t aps_designated_coordinator;  // 是否指定为协调器
    uint8_t aps_channel_mask[8];         // 信道掩码
    uint8_t aps_extended_pan_id[8];      // 扩展PAN ID
    uint8_t aps_use_insecure_join;       // 是否允许不安全加入
    uint8_t aps_interframe_delay;        // 帧间延迟
    uint8_t aps_last_channel_energy;     // 上次信道能量
    uint8_t aps_last_channel_fail_rate;  // 上次信道失败率
    uint8_t aps_channel_timer;           // 信道定时器
    uint8_t aps_device_key_pair_count;   // 设备密钥对数量
} aib_t;

// APS数据服务原语
void apsde_data_request(apsde_data_req_t* req);
void apsde_data_confirm(aps_status_t status);
void apsde_data_indication(apsde_data_ind_t* ind);

// APS管理服务原语
void apsme_bind_request(apsme_bind_req_t* req);
void apsme_bind_confirm(aps_status_t status);
void apsme_unbind_request(apsme_unbind_req_t* req);
void apsme_unbind_confirm(aps_status_t status);

void apsme_get_request(aps_attribute_t attr);
void apsme_get_confirm(aps_status_t status, aps_attribute_t attr, void* value);
void apsme_set_request(aps_attribute_t attr, void* value);
void apsme_set_confirm(aps_status_t status, aps_attribute_t attr);
