/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\README.md
 * Line: 272
 * Language: c
 * Block ID: 59a5eb58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file uds_services.c
 * @brief UDS诊断服务实现
 */

/* UDS服务SID */
#define UDS_SID_DIAGNOSTIC_SESSION_CONTROL      0x10
#define UDS_SID_ECU_RESET                       0x11
#define UDS_SID_READ_DATA_BY_IDENTIFIER         0x22
#define UDS_SID_WRITE_DATA_BY_IDENTIFIER        0x2E
#define UDS_SID_ROUTINE_CONTROL                 0x31
#define UDS_SID_REQUEST_DOWNLOAD                0x34
#define UDS_SID_TRANSFER_DATA                   0x36

/* DIDs */
#define DID_VIN                                 0xF190
#define DID_ECU_SOFTWARE_VERSION                0xF189

static uint8_t g_session = 0x01;  /* Default Session */
static uint8_t g_security_level = 0;

/**
 * @brief 诊断会话控制 $10
 */
UdsResult_t Uds_SessionControl(uint8_t *req, uint16_t req_len,
                                uint8_t *resp, uint16_t *resp_len)
{
    if (req_len < 2) return UDS_ERROR_INCORRECT_MESSAGE_LENGTH;

    uint8_t session_type = req[1];

    switch (session_type) {
        case 0x01:  /* Default Session */
            g_session = 0x01;
            g_security_level = 0;
            break;
        case 0x03:  /* Extended Diagnostic Session */
            g_session = 0x03;
            break;
        default:
            return UDS_ERROR_SUB_FUNCTION_NOT_SUPPORTED;
    }

    resp[0] = 0x50;  /* Positive Response SID */
    resp[1] = session_type;
    resp[2] = 0x00; resp[3] = 0x32;  /* P2_max = 50ms */
    resp[4] = 0x01; resp[5] = 0xF4;  /* P2*_max = 5000ms */
    *resp_len = 6;

    return UDS_OK;
}

/**
 * @brief 通过DID读取数据 $22
 */
UdsResult_t Uds_ReadDataById(uint8_t *req, uint16_t req_len,
                              uint8_t *resp, uint16_t *resp_len)
{
    if (req_len < 3) return UDS_ERROR_INCORRECT_MESSAGE_LENGTH;

    uint16_t did = ((uint16_t)req[1] << 8) | req[2];

    resp[0] = 0x62;  /* Positive Response SID */
    resp[1] = req[1];
    resp[2] = req[2];

    switch (did) {
        case DID_VIN:
            memcpy(&resp[3], "1HGCM82633A123456", 17);
            *resp_len = 20;
            break;
        case DID_ECU_SOFTWARE_VERSION:
            memcpy(&resp[3], "SW_1.2.3", 8);
            *resp_len = 11;
            break;
        default:
            return UDS_ERROR_REQUEST_OUT_OF_RANGE;
    }

    return UDS_OK;
}

/**
 * @brief 处理UDS请求主函数
 */
void Uds_ProcessRequest(uint8_t *request, uint16_t req_len,
                         uint8_t *response, uint16_t *resp_len)
{
    UdsResult_t result;
    uint8_t sid = request[0];

    switch (sid) {
        case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            result = Uds_SessionControl(request, req_len, response, resp_len);
            break;
        case UDS_SID_READ_DATA_BY_IDENTIFIER:
            result = Uds_ReadDataById(request, req_len, response, resp_len);
            break;
        default:
            result = UDS_ERROR_SERVICE_NOT_SUPPORTED;
            break;
    }

    /* 负响应处理 */
    if (result != UDS_OK) {
        response[0] = 0x7F;  /* Negative Response SID */
        response[1] = sid;
        response[2] = result;
        *resp_len = 3;
    }
}
