/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 794
 * Language: c
 * Block ID: 54978c0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ZDO服务原语

// 网络管理服务
void zdo_nwk_addr_req(uint64_t ieee_addr, uint8_t request_type, uint8_t start_index);
void zdo_nwk_addr_rsp(zdo_nwk_addr_rsp_t* rsp);
void zdo_ieee_addr_req(nwk_addr_t nwk_addr, uint8_t request_type, uint8_t start_index);
void zdo_ieee_addr_rsp(zdo_ieee_addr_rsp_t* rsp);

// 节点描述符服务
void zdo_node_desc_req(nwk_addr_t nwk_addr_of_interest);
void zdo_node_desc_rsp(zdo_node_desc_rsp_t* rsp);
void zdo_power_desc_req(nwk_addr_t nwk_addr_of_interest);
void zdo_power_desc_rsp(zdo_power_desc_rsp_t* rsp);

// 简单描述符服务
void zdo_simple_desc_req(nwk_addr_t nwk_addr_of_interest, uint8_t endpoint);
void zdo_simple_desc_rsp(zdo_simple_desc_rsp_t* rsp);
void zdo_active_ep_req(nwk_addr_t nwk_addr_of_interest);
void zdo_active_ep_rsp(zdo_active_ep_rsp_t* rsp);

// 绑定服务
void zdo_bind_req(zdo_bind_req_t* req);
void zdo_bind_rsp(zdo_bind_rsp_t* rsp);
void zdo_unbind_req(zdo_unbind_req_t* req);
void zdo_unbind_rsp(zdo_unbind_rsp_t* rsp);

// 设备与服务发现
void zdo_match_desc_req(zdo_match_desc_req_t* req);
void zdo_match_desc_rsp(zdo_match_desc_rsp_t* rsp);

// 网络管理
void zdo_mgmt_lqi_req(nwk_addr_t nwk_addr, uint8_t start_index);
void zdo_mgmt_lqi_rsp(zdo_mgmt_lqi_rsp_t* rsp);
void zdo_mgmt_rtg_req(nwk_addr_t nwk_addr, uint8_t start_index);
void zdo_mgmt_rtg_rsp(zdo_mgmt_rtg_rsp_t* rsp);
void zdo_mgmt_leave_req(zdo_mgmt_leave_req_t* req);
void zdo_mgmt_leave_rsp(zdo_mgmt_leave_rsp_t* rsp);
void zdo_mgmt_permit_join_req(uint8_t permit_duration, uint8_t tc_significance);
void zdo_mgmt_permit_join_rsp(zdo_mgmt_permit_join_rsp_t* rsp);
void zdo_mgmt_nwk_update_req(zdo_mgmt_nwk_update_req_t* req);
void zdo_mgmt_nwk_update_notify(zdo_mgmt_nwk_update_notify_t* notify);
