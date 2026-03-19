/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1141
 * Language: c
 * Block ID: 21578081
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 安全OTA升级流程
// 1. 下载固件
// 2. 验证签名和版本
// 3. 写入OTA分区
// 4. 标记待验证
// 5. 重启
// 6. 验证后确认/回滚
// ============================================================

#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "mbedtls/md.h"

#define OTA_URL "https://ota.example.com/firmware.bin"
#define OTA_SIGNATURE_URL "https://ota.example.com/firmware.bin.sig"

// OTA状态机
typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_READY,
    OTA_STATE_APPLYING,
    OTA_STATE_CONFIRMED,
    OTA_STATE_ROLLBACK
} OTA_State;

static OTA_State ota_state = OTA_STATE_IDLE;
static esp_ota_handle_t ota_handle = 0;

// 开始OTA
esp_err_t ota_start(void) {
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    if (ota_partition == NULL) {
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "OTA partition: %s, size: %d",
             ota_partition->label, ota_partition->size);

    // 开始OTA写入
    esp_err_t err = esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        return err;
    }

    ota_state = OTA_STATE_DOWNLOADING;
    return ESP_OK;
}

// 写入固件数据
esp_err_t ota_write_data(const uint8_t *data, size_t len) {
    if (ota_state != OTA_STATE_DOWNLOADING) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = esp_ota_write(ota_handle, data, len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
        esp_ota_abort(ota_handle);
        ota_state = OTA_STATE_IDLE;
    }
    return err;
}

// 完成OTA写入，验证固件
esp_err_t ota_finish(void) {
    if (ota_state != OTA_STATE_DOWNLOADING) {
        return ESP_ERR_INVALID_STATE;
    }

    ota_state = OTA_STATE_VERIFYING;

    // 完成写入
    esp_err_t err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
        ota_state = OTA_STATE_IDLE;
        return err;
    }

    // 验证固件签名 (从服务器下载签名文件)
    if (!verify_firmware_signature()) {
        ESP_LOGE(TAG, "Firmware signature verification failed!");
        ota_state = OTA_STATE_IDLE;
        return ESP_ERR_INVALID_SIGNATURE;
    }

    // 设置启动分区
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    err = esp_ota_set_boot_partition(ota_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        ota_state = OTA_STATE_IDLE;
        return err;
    }

    ota_state = OTA_STATE_READY;
    return ESP_OK;
}

// 验证固件签名
bool verify_firmware_signature(void) {
    // 1. 计算固件哈希
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    uint8_t hash[32];

    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&ctx);

    // 分块读取固件并计算哈希
    uint8_t buffer[1024];
    for (size_t offset = 0; offset < ota_partition->size; offset += sizeof(buffer)) {
        size_t to_read = sizeof(buffer);
        if (offset + to_read > ota_partition->size) {
            to_read = ota_partition->size - offset;
        }
        esp_partition_read(ota_partition, offset, buffer, to_read);
        mbedtls_md_update(&ctx, buffer, to_read);
    }

    mbedtls_md_finish(&ctx, hash);
    mbedtls_md_free(&ctx);

    // 2. 从服务器获取签名
    uint8_t signature[256];  // RSA-2048签名长度
    if (!download_signature(signature, sizeof(signature))) {
        return false;
    }

    // 3. 使用公钥验证签名
    // ... RSA/ECDSA验证代码 ...

    return true;
}

// 重启并应用新固件
void ota_apply(void) {
    if (ota_state != OTA_STATE_READY) {
        return;
    }

    ota_state = OTA_STATE_APPLYING;

    // 保存OTA状态到NVS
    nvs_handle_t nvs;
    nvs_open("ota", NVS_READWRITE, &nvs);
    nvs_set_u8(nvs, "ota_pending", 1);
    nvs_commit(nvs);
    nvs_close(nvs);

    // 重启
    esp_restart();
}

// 启动后检查OTA状态
void ota_check_boot_status(void) {
    const esp_partition_t *running = esp_ota_get_running_partition();
    const esp_partition_t *boot = esp_ota_get_boot_partition();

    // 如果运行分区与启动分区不同，说明是从OTA分区启动
    if (running != boot) {
        ESP_LOGI(TAG, "Booted from OTA partition: %s", running->label);

        // 检查是否是第一次启动新固件
        nvs_handle_t nvs;
        if (nvs_open("ota", NVS_READONLY, &nvs) == ESP_OK) {
            uint8_t ota_pending = 0;
            nvs_get_u8(nvs, "ota_pending", &ota_pending);
            nvs_close(nvs);

            if (ota_pending) {
                // 第一次启动，启动验证定时器
                // 如果30分钟内没有确认，自动回滚
                start_ota_confirm_timer();
            }
        }
    }
}

// 确认OTA成功
void ota_confirm(void) {
    // 清除OTA待验证标志
    nvs_handle_t nvs;
    nvs_open("ota", NVS_READWRITE, &nvs);
    nvs_erase_key(nvs, "ota_pending");
    nvs_commit(nvs);
    nvs_close(nvs);

    // 标记固件为有效 (防回滚)
    esp_ota_mark_app_valid_cancel_rollback();

    ota_state = OTA_STATE_CONFIRMED;
    ESP_LOGI(TAG, "OTA confirmed successfully");
}

// OTA失败回滚
void ota_rollback(void) {
    esp_err_t err = esp_ota_mark_app_invalid_rollback_and_reboot();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Rollback failed: %s", esp_err_to_name(err));
    }
}
