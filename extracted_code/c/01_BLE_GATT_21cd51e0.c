/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\01_BLE_GATT.md
 * Line: 384
 * Language: c
 * Block ID: 21cd51e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 发送Notification - 无需确认 */
int gatt_notify(GattConnection *conn, GattCharacteristic *chr,
                const uint8_t *data, uint16_t len) {
    /* 检查客户端是否订阅了notify */
    if (!(chr->cccd_value & 0x01)) {
        return -1;  /* 客户端未订阅 */
    }

    /* 查找特征值句柄 */
    AttAttribute *attr = att_find_by_uuid(gatt_db, chr->uuid16);
    if (!attr) return -1;

    /* 构建ATT PDU */
    uint8_t pdu[ATT_DEFAULT_MTU];
    pdu[0] = ATT_OP_HANDLE_VALUE_NTF;
    pdu[1] = attr->handle & 0xFF;
    pdu[2] = attr->handle >> 8;

    /* 限制MTU */
    uint16_t max_data = conn->mtu - 3;
    if (len > max_data) len = max_data;

    memcpy(pdu + 3, data, len);

    return l2cap_send(conn->handle, L2CAP_CID_ATT, pdu, 3 + len);
}

/* 发送Indication - 需要确认 */
int gatt_indicate(GattConnection *conn, GattCharacteristic *chr,
                  const uint8_t *data, uint16_t len) {
    if (!(chr->cccd_value & 0x02)) {
        return -1;
    }

    /* 检查是否有待确认的indicate */
    if (conn->indicate_pending) {
        return -2;  /* 等待上一个确认 */
    }

    AttAttribute *attr = att_find_by_uuid(gatt_db, chr->uuid16);
    if (!attr) return -1;

    uint8_t pdu[ATT_DEFAULT_MTU];
    pdu[0] = ATT_OP_HANDLE_VALUE_IND;
    pdu[1] = attr->handle & 0xFF;
    pdu[2] = attr->handle >> 8;

    uint16_t max_data = conn->mtu - 3;
    if (len > max_data) len = max_data;
    memcpy(pdu + 3, data, len);

    conn->indicate_pending = true;
    conn->indicate_handle = attr->handle;

    return l2cap_send(conn->handle, L2CAP_CID_ATT, pdu, 3 + len);
}

/* 处理Indication确认 */
void att_handle_value_cfm(GattConnection *conn) {
    conn->indicate_pending = false;

    /* 通知应用层 */
    if (conn->on_indicate_cfm) {
        conn->on_indicate_cfm(conn, conn->indicate_handle);
    }
}
