/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 198
 * Language: c
 * Block ID: ede956c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32-H2: 纯802.15.4芯片，用于Zigbee和Thread
// 无WiFi，专注低功耗网状网络
// ============================================================

#include "esp_zigbee.h"
#include "esp_zboss_api.h"

// Zigbee设备初始化
void zigbee_init(void) {
    esp_zb_platform_config_t config = {
        .radio_config = {
            .radio_mode = RADIO_MODE_NATIVE,
        },
        .host_config = {
            .host_connection_mode = HOST_CONNECTION_MODE_NONE,
        },
    };
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
}

// 创建Zigbee温度传感器节点
void zigbee_temperature_sensor_init(void) {
    esp_zb_cfg_t zb_nwk_cfg = {
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_ED,  // 终端设备
        .install_code_policy = false,
    };

    esp_zb_init(&zb_nwk_cfg);

    // 创建设备端点
    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();

    // 温度传感器簇
    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();

    // 基本簇
    esp_zb_attribute_list_t *basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
    esp_zb_basic_cluster_add_attr(basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version);
    esp_zb_cluster_list_add_basic_cluster(cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // 温度测量簇
    esp_zb_attribute_list_t *temp_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT);
    int16_t measured_value = 2500;  // 25.00°C (单位: 0.01°C)
    esp_zb_temperature_meas_cluster_add_attr(temp_cluster,
                                              ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID,
                                              &measured_value);
    esp_zb_cluster_list_add_temperature_meas_cluster(cluster_list, temp_cluster,
                                                      ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    // 添加到端点
    esp_zb_ep_list_add_ep(ep_list, cluster_list, 1, ESP_ZB_AF_HA_PROFILE_ID, ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID);

    // 注册设备
    esp_zb_device_register(ep_list);
    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
    esp_zb_start(true);  // 自动启动
}

// Thread边界路由器 (需要配合ESP32做WiFi回传)
// ESP32-H2运行OpenThread，ESP32提供WiFi连接
void thread_border_router_init(void) {
    // 初始化OpenThread
    esp_opthread_platform_config_t config = {
        .radio_config = {
            .radio_mode = RADIO_MODE_NATIVE,
        },
        .host_config = {
            .host_connection_mode = HOST_CONNECTION_MODE_RCP,
            .rcp_serial_config = {
                .port = UART_NUM_1,
                .uart_config = {
                    .baud_rate = 115200,
                    .data_bits = UART_DATA_8_BITS,
                    .parity = UART_PARITY_DISABLE,
                    .stop_bits = UART_STOP_BITS_1,
                },
            },
        },
    };

    ESP_ERROR_CHECK(esp_openthread_init(&config));

    // 启动Thread协议栈
    esp_openthread_lock_acquire(portMAX_DELAY);
    otInstance *instance = esp_openthread_get_instance();
    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);
    esp_openthread_lock_release();
}
