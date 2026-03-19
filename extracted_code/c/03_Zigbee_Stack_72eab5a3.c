/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 410
 * Language: c
 * Block ID: 72eab5a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// MAC数据服务
void mcps_data_request(mac_data_req_t* req);
void mcps_data_confirm(mac_status_t status);
void mcps_data_indication(mac_data_ind_t* ind);
void mcps_purge_request(uint8_t msdu_handle);

// MAC管理服务
void mlme_associate_request(mac_assoc_req_t* req);
void mlme_associate_confirm(mac_assoc_conf_t* conf);
void mlme_associate_indication(mac_assoc_ind_t* ind);
void mlme_associate_response(mac_assoc_rsp_t* rsp);

void mlme_disassociate_request(mac_disassoc_req_t* req);
void mlme_disassociate_confirm(mac_status_t status);
void mlme_disassociate_indication(mac_disassoc_ind_t* ind);

void mlme_beacon_notify_indication(mac_beacon_ind_t* ind);
void mlme_get_request(mac_pib_attr_t attr);
void mlme_get_confirm(mac_status_t status, mac_pib_attr_t attr, void* value);
void mlme_set_request(mac_pib_attr_t attr, void* value);
void mlme_set_confirm(mac_status_t status, mac_pib_attr_t attr);

void mlme_scan_request(mac_scan_req_t* req);
void mlme_scan_confirm(mac_scan_conf_t* conf);

void mlme_start_request(mac_start_req_t* req);
void mlme_start_confirm(mac_status_t status);

void mlme_sync_request(uint8_t logical_channel);
void mlme_sync_loss_indication(mac_sync_loss_reason_t reason);
