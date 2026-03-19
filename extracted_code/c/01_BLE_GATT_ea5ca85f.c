/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\01_BLE_GATT.md
 * Line: 455
 * Language: c
 * Block ID: ea5ca85f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* MTU交换处理 */
void att_handle_exchange_mtu(AttDatabase *db, uint16_t conn_handle,
                             const uint8_t *pdu, uint16_t len) {
    uint16_t client_mtu = (pdu[2] << 8) | pdu[1];
    uint16_t server_mtu = ATT_MAX_MTU;

    /* 取较小值 */
    uint16_t agreed_mtu = (client_mtu < server_mtu) ? client_mtu : server_mtu;
    if (agreed_mtu < ATT_DEFAULT_MTU) agreed_mtu = ATT_DEFAULT_MTU;

    /* 更新连接MTU */
    GattConnection *conn = gatt_find_connection(conn_handle);
    conn->mtu = agreed_mtu;

    /* 响应 */
    uint8_t rsp[3];
    rsp[0] = ATT_OP_EXCHANGE_MTU_RSP;
    rsp[1] = server_mtu & 0xFF;
    rsp[2] = server_mtu >> 8;

    l2cap_send(conn_handle, L2CAP_CID_ATT, rsp, 3);
}

/* 长包读取支持（Read Blob） */
void att_handle_read_blob(AttDatabase *db, uint16_t conn_handle,
                          const uint8_t *pdu, uint16_t len) {
    AttHandle handle = (pdu[2] << 8) | pdu[1];
    uint16_t offset = (pdu[4] << 8) | pdu[3];

    AttAttribute *attr = att_find_by_handle(db, handle);
    uint8_t rsp[ATT_MAX_MTU];

    if (!attr || offset >= attr->value_len) {
        att_error_rsp(pdu[0], handle, ATT_ERR_INVALID_OFFSET, rsp);
    } else {
        rsp[0] = ATT_OP_READ_BLOB_RSP;
        uint16_t to_send = attr->value_len - offset;
        uint16_t max_send = conn->mtu - 1;
        if (to_send > max_send) to_send = max_send;

        memcpy(rsp + 1, attr->value + offset, to_send);
        l2cap_send(conn_handle, L2CAP_CID_ATT, rsp, 1 + to_send);
    }
}
