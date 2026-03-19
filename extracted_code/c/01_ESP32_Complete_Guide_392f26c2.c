/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 1762
 * Language: c
 * Block ID: 392f26c2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ble_gatt_client_example.c
 * ESP32 BLE GATT 客户端示例
 * 功能: 扫描设备、连接、发现服务、读写特征值
 */

#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"

#define GATTC_TAG           "GATTC_DEMO"
#define REMOTE_SERVICE_UUID 0x00FF
#define REMOTE_NOTIFY_UUID  0xFF01
#define PROFILE_NUM         1
#define PROFILE_A_APP_ID    0
#define INVALID_HANDLE      0

/* 扫描参数 */
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_DISABLE
};

/* 连接配置 */
struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
    esp_gatt_if_t gattc_if;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bt_uuid_t remote_filter_service_uuid;
    esp_bt_uuid_t remote_filter_char_uuid;
};

static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

/**
 * @brief GATT 客户端事件处理
 */
static void gattc_profile_event_handler(esp_gattc_cb_event_t event,
                                        esp_gatt_if_t gattc_if,
                                        esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "GATT 客户端注册成功");

        /* 设置扫描参数 */
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret) {
            ESP_LOGE(GATTC_TAG, "设置扫描参数失败: %d", scan_ret);
        }
        break;

    case ESP_GATTC_CONNECT_EVT: {
        ESP_LOGI(GATTC_TAG, "已连接到设备, conn_id = %d", p_data->connect.conn_id);

        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
               p_data->connect.remote_bda, sizeof(esp_bd_addr_t));

        /* 更新连接参数 */
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x30;
        conn_params.min_int = 0x10;
        conn_params.timeout = 500;

        /* 请求 MTU */
        esp_ble_gattc_send_mtu_req(gattc_if, p_data->connect.conn_id);
        break;
    }

    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "打开连接失败, status = %d", param->open.status);
        } else {
            ESP_LOGI(GATTC_TAG, "连接打开成功");
        }
        break;

    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        if (param->dis_srvc_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "发现服务失败");
            break;
        }
        ESP_LOGI(GATTC_TAG, "发现服务完成");

        /* 搜索指定服务 */
        esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id,
                                     &remote_filter_service_uuid);
        break;

    case ESP_GATTC_SEARCH_RES_EVT: {
        ESP_LOGI(GATTC_TAG, "发现服务, UUID = 0x%04x",
                 param->search_res.srvc_id.uuid.uuid.uuid16);

        gl_profile_tab[PROFILE_A_APP_ID].service_start_handle =
            param->search_res.start_handle;
        gl_profile_tab[PROFILE_A_APP_ID].service_end_handle =
            param->search_res.end_handle;
        break;
    }

    case ESP_GATTC_SEARCH_CMPL_EVT:
        if (p_data->search_cmpl.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "搜索服务失败");
            break;
        }

        if (p_data->search_cmpl.searched_service_source == ESP_GATT_SERVICE_FROM_REMOTE_DEVICE) {
            ESP_LOGI(GATTC_TAG, "从远程设备获取服务信息");
        }

        /* 获取特征值 */
        uint16_t count = 0;
        esp_gatt_status_t status = esp_ble_gattc_get_attr_count(
            gattc_if,
            p_data->search_cmpl.conn_id,
            ESP_GATT_DB_CHARACTERISTIC,
            gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
            gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
            INVALID_HANDLE,
            &count);

        if (status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "获取特征值数量失败");
        }

        ESP_LOGI(GATTC_TAG, "找到 %d 个特征值", count);

        /* 查找目标特征值 */
        if (count > 0) {
            esp_gattc_char_elem_t *char_elem_result =
                (esp_gattc_char_elem_t *)malloc(sizeof(esp_gattc_char_elem_t) * count);

            if (!char_elem_result) {
                ESP_LOGE(GATTC_TAG, "内存分配失败");
                break;
            }

            status = esp_ble_gattc_get_char_by_uuid(
                gattc_if,
                p_data->search_cmpl.conn_id,
                gl_profile_tab[PROFILE_A_APP_ID].service_start_handle,
                gl_profile_tab[PROFILE_A_APP_ID].service_end_handle,
                remote_filter_char_uuid,
                char_elem_result,
                &count);

            if (status != ESP_GATT_OK) {
                ESP_LOGE(GATTC_TAG, "获取特征值失败");
                free(char_elem_result);
                break;
            }

            /* 保存特征值句柄 */
            gl_profile_tab[PROFILE_A_APP_ID].char_handle = char_elem_result[0].char_handle;

            /* 注册通知 */
            esp_ble_gattc_register_for_notify(gattc_if,
                gl_profile_tab[PROFILE_A_APP_ID].remote_bda,
                char_elem_result[0].char_handle);

            free(char_elem_result);
        }
        break;

    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        if (p_data->reg_for_notify.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "注册通知失败");
        } else {
            ESP_LOGI(GATTC_TAG, "通知注册成功");

            /* 写入 CCCD 使能通知 */
            uint16_t notify_en = 1;
            esp_gatt_write_type_t write_type = ESP_GATT_WRITE_TYPE_NO_RSP;
            esp_ble_gattc_write_char(gattc_if,
                                     gl_profile_tab[PROFILE_A_APP_ID].conn_id,
                                     gl_profile_tab[PROFILE_A_APP_ID].char_handle,
                                     sizeof(notify_en),
                                     (uint8_t *)&notify_en,
                                     write_type,
                                     ESP_GATT_AUTH_REQ_NONE);
        }
        break;
    }

    case ESP_GATTC_NOTIFY_EVT:
        if (p_data->notify.is_notify) {
            ESP_LOGI(GATTC_TAG, "收到通知:");
        } else {
            ESP_LOGI(GATTC_TAG, "收到指示:");
        }
        ESP_LOG_BUFFER_HEX(GATTC_TAG, p_data->notify.value, p_data->notify.value_len);
        break;

    case ESP_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status != ESP_GATT_OK) {
            ESP_LOGE(GATTC_TAG, "写入描述符失败");
        } else {
            ESP_LOGI(GATTC_TAG, "写入描述符成功");
        }
        break;

    case ESP_GATTC_SRVC_CHG_EVT: {
        esp_bd_addr_t bda;
        memcpy(bda, p_data->srvc_chg.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "服务变化, 地址 = %02x:%02x:%02x:%02x:%02x:%02x",
                 bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }

    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "断开连接, reason = %d", p_data->disconnect.reason);

        /* 开始扫描 */
        esp_ble_gap_start_scanning(30);
        break;

    default:
        break;
    }
}

/**
 * @brief GAP 事件处理
 */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(GATTC_TAG, "扫描参数设置完成");
        /* 开始扫描 (30 秒) */
        esp_ble_gap_start_scanning(30);
        break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "扫描启动失败");
        } else {
            ESP_LOGI(GATTC_TAG, "扫描启动成功");
        }
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;

        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            /* 解析广播数据,查找设备名称 */
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                                ESP_BLE_AD_TYPE_NAME_CMPL,
                                                &adv_name_len);

            /* 打印扫描到的设备 */
            ESP_LOGI(GATTC_TAG, "扫描到设备:");
            ESP_LOGI(GATTC_TAG, "  地址: %02x:%02x:%02x:%02x:%02x:%02x",
                     scan_result->scan_rst.bda[0], scan_result->scan_rst.bda[1],
                     scan_result->scan_rst.bda[2], scan_result->scan_rst.bda[3],
                     scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[5]);
            ESP_LOGI(GATTC_TAG, "  RSSI: %d dBm", scan_result->scan_rst.rssi);

            if (adv_name != NULL) {
                ESP_LOGI(GATTC_TAG, "  名称: %.*s", adv_name_len, adv_name);

                /* 检查是否是目标设备 */
                if (strlen(DEVICE_NAME) == adv_name_len &&
                    strncmp((char *)adv_name, DEVICE_NAME, adv_name_len) == 0) {

                    ESP_LOGI(GATTC_TAG, "找到目标设备!");

                    /* 停止扫描 */
                    esp_ble_gap_stop_scanning();

                    /* 连接设备 */
                    esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if,
                                       scan_result->scan_rst.bda,
                                       scan_result->scan_rst.ble_addr_type, true);
                }
            }
            break;

        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            ESP_LOGI(GATTC_TAG, "扫描完成");
            break;

        default:
            break;
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "扫描停止失败");
        } else {
            ESP_LOGI(GATTC_TAG, "扫描停止成功");
        }
        break;

    default:
        break;
    }
}
