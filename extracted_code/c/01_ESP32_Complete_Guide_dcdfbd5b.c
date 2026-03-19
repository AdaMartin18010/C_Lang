/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 1359
 * Language: c
 * Block ID: dcdfbd5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ble_gatt_server_example.c
 * ESP32 BLE GATT 服务器完整示例
 * 功能: 创建 BLE 服务、特征值读写、通知发送
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#define GATTS_TAG               "GATTS_DEMO"
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_CHAR_UUID         0xFF01
#define GATTS_DESCR_UUID        0x3333
#define GATTS_NUM_HANDLE        4

#define DEVICE_NAME             "ESP32_GATTS"
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40

/* 应用程序 ID */
#define PROFILE_A_APP_ID        0
#define PROFILE_B_APP_ID        1

/* 属性表索引 */
enum {
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,
    IDX_NB,
};

/* 全局变量 */
static uint8_t char1_str[] = {0x11, 0x22, 0x33};
static esp_gatt_char_prop_t a_property = 0;
static uint8_t adv_config_done = 0;

/* GATT 属性表 */
static esp_gatts_attr_db_t gatt_db[IDX_NB];

/* 特性值 */
static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len     = sizeof(char1_str),
    .attr_value   = char1_str,
};

/* GATT 接口 */
static esp_gatt_if_t gatts_if_for_notify = ESP_GATT_IF_NONE;
static uint16_t notify_conn_id = 0;
static uint16_t notify_handle = 0;

/* 广播参数 */
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* 广播数据 */
static uint8_t adv_service_uuid128[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data = NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

/**
 * @brief 准备写入响应
 */
void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_TAG, "准备写入, handle = %d, len = %d",
             param->write.handle, param->write.len);

    if (param->write.offset > prepare_write_env->prepare_len) {
        return;
    }

    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
    }

    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);

    prepare_write_env->prepare_len += param->write.len;
}

/**
 * @brief 执行写入响应
 */
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param)
{
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC) {
        ESP_LOG_BUFFER_HEX(GATTS_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    } else {
        ESP_LOGI(GATTS_TAG, "取消写入");
    }

    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

/**
 * @brief GATT 事件处理
 */
static void gatts_profile_a_event_handler(esp_gatts_cb_event_t event,
                                          esp_gatt_if_t gatts_if,
                                          esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(GATTS_TAG, "GATT 注册事件, status = %d, app_id = %d",
                 param->reg.status, param->reg.app_id);

        /* 设置设备名称 */
        esp_ble_gap_set_device_name(DEVICE_NAME);

        /* 配置广播数据 */
        esp_ble_gap_config_adv_data(&adv_data);
        adv_config_done |= adv_config_flag;

        /* 配置扫描响应数据 */
        esp_ble_gap_config_adv_data(&scan_rsp_data);
        adv_config_done |= scan_rsp_config_flag;

        /* 创建属性表 */
        esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, IDX_NB, SVC_INST_ID);
        break;
    }

    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(GATTS_TAG, "读取事件, conn_id %d, trans_id %d, handle %d",
                 param->read.conn_id, param->read.trans_id, param->read.handle);

        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0x12;
        rsp.attr_value.value[1] = 0x34;
        rsp.attr_value.value[2] = 0x56;
        rsp.attr_value.value[3] = 0x78;

        esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                    param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }

    case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "写入事件, handle = %d, len = %d",
                 param->write.handle, param->write.len);

        if (!param->write.is_prep) {
            ESP_LOGI(GATTS_TAG, "写入值:");
            ESP_LOG_BUFFER_HEX(GATTS_TAG, param->write.value, param->write.len);

            /* 检查是否需要响应 */
            if (param->write.need_rsp) {
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                            param->write.trans_id, ESP_GATT_OK, NULL);
            }
        } else {
            /* 准备写入 (长数据) */
            example_prepare_write_event_env(gatts_if, &a_prepare_write_env, param);
        }
        break;
    }

    case ESP_GATTS_EXEC_WRITE_EVT: {
        ESP_LOGI(GATTS_TAG, "执行写入事件");
        example_exec_write_event_env(&a_prepare_write_env, param);
        break;
    }

    case ESP_GATTS_MTU_EVT: {
        ESP_LOGI(GATTS_TAG, "MTU 设置: %d", param->mtu.mtu);
        break;
    }

    case ESP_GATTS_CONF_EVT: {
        ESP_LOGI(GATTS_TAG, "通知/指示确认");
        break;
    }

    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(GATTS_TAG, "连接事件, conn_id = %d", param->connect.conn_id);

        /* 更新连接参数 */
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;

        gatts_if_for_notify = gatts_if;
        notify_conn_id = param->connect.conn_id;

        /* 发送连接参数更新 */
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }

    case ESP_GATTS_DISCONNECT_EVT: {
        ESP_LOGI(GATTS_TAG, "断开连接, reason = 0x%x", param->disconnect.reason);

        /* 重新开始广播 */
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }

    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(GATTS_TAG, "创建属性表失败, error code = 0x%x",
                     param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != IDX_NB) {
            ESP_LOGE(GATTS_TAG, "创建的句柄数量错误");
        } else {
            ESP_LOGI(GATTS_TAG, "属性表创建成功, handles:");
            for (int i = 0; i < IDX_NB; i++) {
                ESP_LOGI(GATTS_TAG, "  handle[%d] = %d", i, param->add_attr_tab.handles[i]);
            }

            /* 保存通知句柄 */
            notify_handle = param->add_attr_tab.handles[IDX_CHAR_CFG_A];

            /* 启动服务 */
            esp_ble_gatts_start_service(param->add_attr_tab.handles[IDX_SVC]);
        }
        break;
    }

    default:
        break;
    }
}

/**
 * @brief GAP 事件处理
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~adv_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        adv_config_done &= (~scan_rsp_config_flag);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TAG, "广播启动失败");
        } else {
            ESP_LOGI(GATTS_TAG, "广播启动成功");
        }
        break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(GATTS_TAG, "连接参数更新:");
        ESP_LOGI(GATTS_TAG, "  status = %d", param->update_conn_params.status);
        ESP_LOGI(GATTS_TAG, "  min_int = %d", param->update_conn_params.conn_int_min);
        ESP_LOGI(GATTS_TAG, "  max_int = %d", param->update_conn_params.conn_int_max);
        ESP_LOGI(GATTS_TAG, "  latency = %d", param->update_conn_params.conn_latency);
        ESP_LOGI(GATTS_TAG, "  timeout = %d", param->update_conn_params.supervision_timeout);
        break;

    default:
        break;
    }
}

/**
 * @brief 发送通知任务
 */
static void notify_task(void *pvParameters)
{
    uint8_t notify_data[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t counter = 0;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        if (gatts_if_for_notify != ESP_GATT_IF_NONE && notify_handle != 0) {
            /* 更新数据 */
            notify_data[3] = counter++;

            /* 发送通知 */
            esp_ble_gatts_send_indicate(gatts_if_for_notify, notify_conn_id,
                                        notify_handle, sizeof(notify_data),
                                        notify_data, false);

            ESP_LOGI(GATTS_TAG, "发送通知: %d", counter);
        }
    }
}

/**
 * @brief BLE 初始化
 */
static void ble_init(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    /* 初始化 BLE 控制器 */
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    /* 初始化 Bluedroid */
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    /* 注册 GAP 回调 */
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));

    /* 注册 GATT 回调 */
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));

    /* 注册应用程序 */
    ESP_ERROR_CHECK(esp_ble_gatts_app_register(PROFILE_A_APP_ID));
}

void app_main(void)
{
    /* 初始化 NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(GATTS_TAG, "ESP32 BLE GATT 服务器启动");

    /* 初始化 BLE */
    ble_init();

    /* 创建通知任务 */
    xTaskCreate(notify_task, "notify_task", 2048, NULL, 5, NULL);
}
