/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\01_BLE_GATT.md
 * Line: 155
 * Language: c
 * Block ID: 1daef49a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ATT操作码 */
typedef enum {
    ATT_OP_ERROR_RSP        = 0x01,
    ATT_OP_EXCHANGE_MTU_REQ = 0x02,
    ATT_OP_EXCHANGE_MTU_RSP = 0x03,
    ATT_OP_FIND_INFO_REQ    = 0x04,
    ATT_OP_FIND_INFO_RSP    = 0x05,
    ATT_OP_READ_BY_TYPE_REQ = 0x08,
    ATT_OP_READ_BY_TYPE_RSP = 0x09,
    ATT_OP_READ_REQ         = 0x0A,
    ATT_OP_READ_RSP         = 0x0B,
    ATT_OP_READ_BLOB_REQ    = 0x0C,
    ATT_OP_READ_BLOB_RSP    = 0x0D,
    ATT_OP_READ_MULTI_REQ   = 0x0E,
    ATT_OP_READ_MULTI_RSP   = 0x0F,
    ATT_OP_READ_BY_GRP_REQ  = 0x10,
    ATT_OP_READ_BY_GRP_RSP  = 0x11,
    ATT_OP_WRITE_REQ        = 0x12,
    ATT_OP_WRITE_RSP        = 0x13,
    ATT_OP_PREPARE_WRITE_REQ = 0x16,
    ATT_OP_EXECUTE_WRITE_REQ = 0x18,
    ATT_OP_HANDLE_VALUE_NTF = 0x1B,
    ATT_OP_HANDLE_VALUE_IND = 0x1D,
    ATT_OP_HANDLE_VALUE_CFM = 0x1E,
} ATT_OpCode;

/* ATT错误码 */
typedef enum {
    ATT_ERR_INVALID_HANDLE       = 0x01,
    ATT_ERR_READ_NOT_PERMITTED   = 0x02,
    ATT_ERR_WRITE_NOT_PERMITTED  = 0x03,
    ATT_ERR_INVALID_PDU          = 0x04,
    ATT_ERR_INSUFFICIENT_AUTH    = 0x05,
    ATT_ERR_REQ_NOT_SUPPORTED    = 0x06,
    ATT_ERR_INVALID_OFFSET       = 0x07,
    ATT_ERR_INSUFFICIENT_AUTHOR  = 0x08,
    ATT_ERR_PREPARE_QUEUE_FULL   = 0x09,
    ATT_ERR_ATTR_NOT_FOUND       = 0x0A,
    ATT_ERR_ATTR_NOT_LONG        = 0x0B,
    ATT_ERR_INSUFFICIENT_ENC     = 0x0F,
} ATT_ErrorCode;

/* 处理Read请求 */
static void att_handle_read_req(AttDatabase *db, uint16_t conn_handle,
                                const uint8_t *pdu, uint16_t len) {
    AttHandle handle = (pdu[1] << 8) | pdu[2];

    /* 查找属性 */
    AttAttribute *attr = att_find_by_handle(db, handle);
    uint8_t rsp[ATT_DEFAULT_MTU];
    uint16_t rsp_len;

    if (!attr) {
        rsp_len = att_error_rsp(pdu[0], handle, ATT_ERR_INVALID_HANDLE, rsp);
    } else if (!(attr->permissions & ATT_PERM_READ)) {
        rsp_len = att_error_rsp(pdu[0], handle, ATT_ERR_READ_NOT_PERMITTED, rsp);
    } else {
        /* 执行读取 */
        rsp[0] = ATT_OP_READ_RSP;
        uint16_t value_len;

        if (attr->on_read) {
            value_len = attr->on_read(attr, 0, rsp + 1, sizeof(rsp) - 1);
        } else {
            value_len = attr->value_len;
            if (value_len > sizeof(rsp) - 1) value_len = sizeof(rsp) - 1;
            memcpy(rsp + 1, attr->value, value_len);
        }
        rsp_len = 1 + value_len;
    }

    l2cap_send(conn_handle, L2CAP_CID_ATT, rsp, rsp_len);
}

/* 处理Write请求 */
static void att_handle_write_req(AttDatabase *db, uint16_t conn_handle,
                                 const uint8_t *pdu, uint16_t len) {
    AttHandle handle = (pdu[1] << 8) | pdu[2];
    const uint8_t *data = pdu + 3;
    uint16_t data_len = len - 3;

    AttAttribute *attr = att_find_by_handle(db, handle);
    uint8_t rsp[5];

    if (!attr) {
        att_error_rsp(pdu[0], handle, ATT_ERR_INVALID_HANDLE, rsp);
    } else if (!(attr->permissions & ATT_PERM_WRITE)) {
        att_error_rsp(pdu[0], handle, ATT_ERR_WRITE_NOT_PERMITTED, rsp);
    } else if (data_len > attr->value_max) {
        att_error_rsp(pdu[0], handle, ATT_ERR_INVALID_ATTRIBUTE_LEN, rsp);
    } else {
        /* 执行写入 */
        if (attr->on_write) {
            attr->on_write(attr, 0, data, data_len);
        } else {
            memcpy(attr->value, data, data_len);
            attr->value_len = data_len;
        }

        /* 发送确认 */
        rsp[0] = ATT_OP_WRITE_RSP;
        l2cap_send(conn_handle, L2CAP_CID_ATT, rsp, 1);
        return;
    }

    l2cap_send(conn_handle, L2CAP_CID_ATT, rsp, 5);
}
