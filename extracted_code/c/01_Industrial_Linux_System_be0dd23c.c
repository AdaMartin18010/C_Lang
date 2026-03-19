/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 6566
 * Language: c
 * Block ID: be0dd23c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file ota_client.c
 * @brief OTA升级客户端实现
 * @details 支持HTTPS固件下载、签名验证、AB分区升级
 *
 * 编译: gcc -o ota_client ota_client.c -lcurl -lcrypto -ljson-c -lpthread
 * 运行: sudo ./ota_client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <curl/curl.h>
#include <json-c/json.h>

/* OTA配置 */
#define OTA_VERSION             "1.0.0"
#define OTA_SERVER_URL          "https://ota.example.com/api/v1"
#define OTA_DEVICE_ID           "DEV001"
#define OTA_CHECK_INTERVAL_SEC  3600

/* 固件配置 */
#define FIRMWARE_PATH           "/tmp/firmware.bin"
#define FIRMWARE_PART_A         "/dev/mmcblk0p2"
#define FIRMWARE_PART_B         "/dev/mmcblk0p3"
#define BOOT_FLAG_PATH          "/boot/ota_flag"

/* 缓冲区大小 */
#define DOWNLOAD_BUFFER_SIZE    (64 * 1024)
#define SHA256_HASH_SIZE        32

/* OTA状态 */
typedef enum {
    OTA_STATE_IDLE = 0,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_UPGRADING,
    OTA_STATE_REBOOTING,
    OTA_STATE_ROLLBACK
} ota_state_t;

/* 升级结果 */
typedef enum {
    OTA_RESULT_SUCCESS = 0,
    OTA_RESULT_NO_UPDATE,
    OTA_RESULT_DOWNLOAD_FAILED,
    OTA_RESULT_VERIFY_FAILED,
    OTA_RESULT_WRITE_FAILED,
    OTA_RESULT_INSUFFICIENT_SPACE,
    OTA_RESULT_BATTERY_LOW,
    OTA_RESULT_NETWORK_ERROR
} ota_result_t;

/* 固件信息 */
typedef struct {
    char version[32];
    char url[256];
    char hash[65];  /* SHA256 hex string */
    uint64_t size;
    char release_notes[512];
    bool force_update;
    bool delta_update;
    char delta_base[32];  /* 增量升级的基础版本 */
} firmware_info_t;

/* 设备信息 */
typedef struct {
    char device_id[32];
    char current_version[32];
    char hardware_version[32];
    char active_partition;  /* 'A' or 'B' */
    uint32_t battery_level; /* 百分比 */
    uint64_t free_space;
} device_info_t;

/* OTA上下文 */
typedef struct {
    ota_state_t state;
    firmware_info_t firmware;
    device_info_t device;
    char public_key_path[128];
    int download_progress;
    void (*progress_callback)(int percent);
    void (*status_callback)(ota_state_t state, const char *msg);
} ota_context_t;

static ota_context_t g_ctx = {0};

/* ============ 日志 ============ */
#define LOG_INFO(fmt, ...)  printf("[OTA-INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "[OTA-ERROR] " fmt "\n", ##__VA_ARGS__)

/* ============ 工具函数 ============ */

/**
 * @brief 执行shell命令并获取输出
 */
static int exec_command(const char *cmd, char *output, size_t output_size)
{
    FILE *fp = popen(cmd, "r");
    if (!fp) {
        return -1;
    }

    if (output && output_size > 0) {
        fgets(output, output_size, fp);
        /* 移除换行符 */
        size_t len = strlen(output);
        if (len > 0 && output[len - 1] == '\n') {
            output[len - 1] = '\0';
        }
    }

    int status = pclose(fp);
    return WEXITSTATUS(status);
}

/**
 * @brief 获取当前活动分区
 */
static char get_active_partition(void)
{
    char root_dev[64] = {0};

    /* 读取当前根分区 */
    FILE *fp = fopen("/proc/cmdline", "r");
    if (fp) {
        char cmdline[256];
        if (fgets(cmdline, sizeof(cmdline), fp)) {
            char *root = strstr(cmdline, "root=");
            if (root) {
                sscanf(root + 5, "%s", root_dev);
            }
        }
        fclose(fp);
    }

    /* 判断是A分区还是B分区 */
    if (strstr(root_dev, "mmcblk0p2") || strstr(root_dev, "p2")) {
        return 'A';
    }
    return 'B';
}

/**
 * @brief 获取当前版本
 */
static void get_current_version(char *version, size_t size)
{
    /* 从文件或命令读取 */
    FILE *fp = fopen("/etc/firmware_version", "r");
    if (fp) {
        fgets(version, size, fp);
        fclose(fp);
        /* 移除换行符 */
        size_t len = strlen(version);
        if (len > 0 && version[len - 1] == '\n') {
            version[len - 1] = '\0';
        }
    } else {
        strncpy(version, "1.0.0", size - 1);
    }
}

/* ============ 下载功能 ============ */

typedef struct {
    FILE *fp;
    size_t downloaded;
    size_t total;
} download_context_t;

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    download_context_t *ctx = (download_context_t *)userdata;
    size_t written = fwrite(ptr, size, nmemb, ctx->fp);
    ctx->downloaded += written * size;

    /* 更新进度 */
    if (ctx->total > 0 && g_ctx.progress_callback) {
        int progress = (int)((ctx->downloaded * 100) / ctx->total);
        if (progress != g_ctx.download_progress) {
            g_ctx.download_progress = progress;
            g_ctx.progress_callback(progress);
        }
    }

    return written;
}

static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                              curl_off_t ultotal, curl_off_t ulnow)
{
    (void)clientp;
    (void)ultotal;
    (void)ulnow;

    if (dltotal > 0) {
        download_context_t *ctx = (download_context_t *)clientp;
        ctx->total = dltotal;
    }

    return 0;
}

/**
 * @brief 下载固件
 */
static ota_result_t download_firmware(const char *url, const char *output_path)
{
    CURL *curl;
    CURLcode res;
    download_context_t dl_ctx = {0};

    LOG_INFO("Downloading firmware from: %s", url);

    dl_ctx.fp = fopen(output_path, "wb");
    if (!dl_ctx.fp) {
        LOG_ERROR("Failed to create file: %s", output_path);
        return OTA_RESULT_INSUFFICIENT_SPACE;
    }

    curl = curl_easy_init();
    if (!curl) {
        fclose(dl_ctx.fp);
        return OTA_RESULT_NETWORK_ERROR;
    }

    /* 设置下载选项 */
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dl_ctx);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L);  /* 5分钟超时 */
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &dl_ctx);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

    /* 启用SSL验证 */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    g_ctx.state = OTA_STATE_DOWNLOADING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Downloading firmware...");
    }

    res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);
    fclose(dl_ctx.fp);

    if (res != CURLE_OK) {
        LOG_ERROR("Download failed: %s", curl_easy_strerror(res));
        unlink(output_path);
        return OTA_RESULT_DOWNLOAD_FAILED;
    }

    if (http_code != 200) {
        LOG_ERROR("HTTP error: %ld", http_code);
        unlink(output_path);
        return OTA_RESULT_DOWNLOAD_FAILED;
    }

    LOG_INFO("Download complete: %zu bytes", dl_ctx.downloaded);
    return OTA_RESULT_SUCCESS;
}

/* ============ 签名验证 ============ */

/**
 * @brief 验证固件签名 (RSA-SHA256)
 */
static bool verify_firmware_signature(const char *firmware_path,
                                       const char *signature_path,
                                       const char *public_key_path)
{
    EVP_MD_CTX *ctx = NULL;
    EVP_PKEY *pkey = NULL;
    FILE *fp = NULL;
    bool result = false;

    LOG_INFO("Verifying firmware signature...");

    /* 读取公钥 */
    fp = fopen(public_key_path, "r");
    if (!fp) {
        LOG_ERROR("Failed to open public key: %s", public_key_path);
        return false;
    }

    pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);

    if (!pkey) {
        LOG_ERROR("Failed to read public key");
        return false;
    }

    /* 读取签名 */
    uint8_t signature[256];
    fp = fopen(signature_path, "rb");
    if (!fp) {
        LOG_ERROR("Failed to open signature file");
        EVP_PKEY_free(pkey);
        return false;
    }

    size_t sig_len = fread(signature, 1, sizeof(signature), fp);
    fclose(fp);

    /* 创建验证上下文 */
    ctx = EVP_MD_CTX_new();
    if (!ctx) {
        goto cleanup;
    }

    if (EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) != 1) {
        goto cleanup;
    }

    /* 读取固件并计算哈希 */
    fp = fopen(firmware_path, "rb");
    if (!fp) {
        goto cleanup;
    }

    uint8_t buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (EVP_DigestVerifyUpdate(ctx, buffer, bytes_read) != 1) {
            fclose(fp);
            goto cleanup;
        }
    }
    fclose(fp);

    /* 验证签名 */
    if (EVP_DigestVerifyFinal(ctx, signature, sig_len) == 1) {
        result = true;
        LOG_INFO("Signature verification PASSED");
    } else {
        LOG_ERROR("Signature verification FAILED");
    }

cleanup:
    if (ctx) EVP_MD_CTX_free(ctx);
    if (pkey) EVP_PKEY_free(pkey);

    return result;
}

/**
 * @brief 计算文件SHA256
 */
static bool calculate_file_sha256(const char *filepath, char *hash_str)
{
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        return false;
    }

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    uint8_t buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        SHA256_Update(&ctx, buffer, bytes_read);
    }
    fclose(fp);

    uint8_t hash[SHA256_HASH_SIZE];
    SHA256_Final(hash, &ctx);

    /* 转换为hex字符串 */
    for (int i = 0; i < SHA256_HASH_SIZE; i++) {
        sprintf(hash_str + (i * 2), "%02x", hash[i]);
    }
    hash_str[64] = '\0';

    return true;
}

/* ============ 升级执行 ============ */

/**
 * @brief 写入固件到分区
 */
static bool write_firmware_to_partition(const char *firmware_path,
                                          const char *partition)
{
    LOG_INFO("Writing firmware to %s...", partition);

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dd if=%s of=%s bs=1M status=progress conv=fsync",
             firmware_path, partition);

    int ret = system(cmd);
    if (ret != 0) {
        LOG_ERROR("Failed to write firmware");
        return false;
    }

    LOG_INFO("Firmware written successfully");
    return true;
}

/**
 * @brief 设置下次启动分区
 */
static bool set_boot_partition(char partition)
{
    FILE *fp = fopen(BOOT_FLAG_PATH, "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "BOOT_PART=%c\n", partition);
    fprintf(fp, "OTA_UPGRADE=1\n");
    fprintf(fp, "OTA_VERSION=%s\n", g_ctx.firmware.version);
    fprintf(fp, "OTA_TIMESTAMP=%lu\n", (unsigned long)time(NULL));
    fclose(fp);

    LOG_INFO("Boot partition set to: %c", partition);
    return true;
}

/**
 * @brief 执行系统重启
 */
static void system_reboot(void)
{
    LOG_INFO("System rebooting...");
    sync();
    sleep(2);
    reboot(LINUX_REBOOT_CMD_RESTART);
}

/**
 * @brief 报告升级状态到服务器
 */
static void report_upgrade_status(const char *status, const char *message)
{
    /* 构造JSON */
    struct json_object *root = json_object_new_object();
    json_object_object_add(root, "device_id",
                           json_object_new_string(g_ctx.device.device_id));
    json_object_object_add(root, "status", json_object_new_string(status));
    json_object_object_add(root, "message", json_object_new_string(message));
    json_object_object_add(root, "version",
                           json_object_new_string(g_ctx.firmware.version));
    json_object_object_add(root, "timestamp",
                           json_object_new_int64(time(NULL)));

    const char *json_str = json_object_to_json_string(root);
    LOG_INFO("Report status: %s", json_str);

    /* 实际项目中通过HTTP/MQTT发送 */

    json_object_put(root);
}

/* ============ 主OTA流程 ============ */

/**
 * @brief 检查更新
 */
static ota_result_t check_for_update(void)
{
    LOG_INFO("Checking for firmware update...");
    g_ctx.state = OTA_STATE_CHECKING;

    /* 获取设备信息 */
    strncpy(g_ctx.device.device_id, OTA_DEVICE_ID,
            sizeof(g_ctx.device.device_id) - 1);
    get_current_version(g_ctx.device.current_version,
                        sizeof(g_ctx.device.current_version));
    g_ctx.device.active_partition = get_active_partition();

    LOG_INFO("Current version: %s, Partition: %c",
             g_ctx.device.current_version, g_ctx.device.active_partition);

    /* 构造检查请求 */
    struct json_object *req = json_object_new_object();
    json_object_object_add(req, "device_id",
                           json_object_new_string(g_ctx.device.device_id));
    json_object_object_add(req, "current_version",
                           json_object_new_string(g_ctx.device.current_version));
    json_object_object_add(req, "hardware_version",
                           json_object_new_string("HW-1.0"));

    /* 实际项目中发送HTTP POST请求到OTA服务器 */
    /* 这里模拟服务器响应 */

    /* 模拟: 假设有新版本 */
    strncpy(g_ctx.firmware.version, "2.0.0", sizeof(g_ctx.firmware.version) - 1);
    strncpy(g_ctx.firmware.url,
            "https://ota.example.com/firmware/v2.0.0/firmware.bin",
            sizeof(g_ctx.firmware.url) - 1);
    strncpy(g_ctx.firmware.hash,
            "aabbccdd00112233445566778899aabbccdd00112233445566778899aabbccdd",
            sizeof(g_ctx.firmware.hash) - 1);
    g_ctx.firmware.size = 16 * 1024 * 1024;  /* 16MB */
    strncpy(g_ctx.firmware.release_notes, "Bug fixes and improvements",
            sizeof(g_ctx.firmware.release_notes) - 1);
    g_ctx.firmware.force_update = false;
    g_ctx.firmware.delta_update = false;

    json_object_put(req);

    /* 检查版本是否需要升级 */
    if (strcmp(g_ctx.firmware.version, g_ctx.device.current_version) == 0) {
        LOG_INFO("Already up to date");
        return OTA_RESULT_NO_UPDATE;
    }

    LOG_INFO("New firmware available: %s", g_ctx.firmware.version);
    return OTA_RESULT_SUCCESS;
}

/**
 * @brief 执行OTA升级
 */
static ota_result_t perform_ota_upgrade(void)
{
    ota_result_t result;
    char calculated_hash[65];

    /* 1. 下载固件 */
    result = download_firmware(g_ctx.firmware.url, FIRMWARE_PATH);
    if (result != OTA_RESULT_SUCCESS) {
        return result;
    }

    /* 2. 验证文件哈希 */
    g_ctx.state = OTA_STATE_VERIFYING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Verifying firmware integrity...");
    }

    if (!calculate_file_sha256(FIRMWARE_PATH, calculated_hash)) {
        LOG_ERROR("Failed to calculate file hash");
        return OTA_RESULT_VERIFY_FAILED;
    }

    if (strcasecmp(calculated_hash, g_ctx.firmware.hash) != 0) {
        LOG_ERROR("Hash mismatch! Expected: %s, Got: %s",
                  g_ctx.firmware.hash, calculated_hash);
        return OTA_RESULT_VERIFY_FAILED;
    }
    LOG_INFO("Hash verification passed");

    /* 3. 验证数字签名 */
    char sig_path[256];
    snprintf(sig_path, sizeof(sig_path), "%s.sig", FIRMWARE_PATH);

    if (access(g_ctx.public_key_path, F_OK) == 0) {
        if (!verify_firmware_signature(FIRMWARE_PATH, sig_path,
                                       g_ctx.public_key_path)) {
            return OTA_RESULT_VERIFY_FAILED;
        }
    }

    /* 4. 写入目标分区 */
    g_ctx.state = OTA_STATE_UPGRADING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Writing firmware...");
    }

    char target_partition[32];
    if (g_ctx.device.active_partition == 'A') {
        strncpy(target_partition, FIRMWARE_PART_B, sizeof(target_partition) - 1);
    } else {
        strncpy(target_partition, FIRMWARE_PART_A, sizeof(target_partition) - 1);
    }

    if (!write_firmware_to_partition(FIRMWARE_PATH, target_partition)) {
        return OTA_RESULT_WRITE_FAILED;
    }

    /* 5. 设置启动标志 */
    char next_partition = (g_ctx.device.active_partition == 'A') ? 'B' : 'A';
    if (!set_boot_partition(next_partition)) {
        LOG_ERROR("Failed to set boot partition");
        return OTA_RESULT_WRITE_FAILED;
    }

    /* 6. 报告状态并重启 */
    report_upgrade_status("SUCCESS", "Firmware installed, rebooting");

    g_ctx.state = OTA_STATE_REBOOTING;
    if (g_ctx.status_callback) {
        g_ctx.status_callback(g_ctx.state, "Rebooting system...");
    }

    /* 清理临时文件 */
    unlink(FIRMWARE_PATH);

    /* 重启 */
    system_reboot();

    return OTA_RESULT_SUCCESS;
}

/* ============ 回调函数 ============ */

static void on_download_progress(int percent)
{
    static int last_percent = -1;
    if (percent != last_percent) {
        printf("\rDownload progress: %d%%", percent);
        fflush(stdout);
        last_percent = percent;
        if (percent >= 100) {
            printf("\n");
        }
    }
}

static void on_status_change(ota_state_t state, const char *msg)
{
    const char *state_str[] = {
        "IDLE", "CHECKING", "DOWNLOADING", "VERIFYING",
        "UPGRADING", "REBOOTING", "ROLLBACK"
    };
    LOG_INFO("[State: %s] %s", state_str[state], msg);
}

/* ============ 主函数 ============ */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Industrial OTA Client v%-26s   ║\n", OTA_VERSION);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    curl_global_init(CURL_GLOBAL_DEFAULT);

    g_ctx.progress_callback = on_download_progress;
    g_ctx.status_callback = on_status_change;
    strncpy(g_ctx.public_key_path, "/etc/ota/public_key.pem",
            sizeof(g_ctx.public_key_path) - 1);

    /* 检查更新 */
    ota_result_t result = check_for_update();
    if (result == OTA_RESULT_NO_UPDATE) {
        LOG_INFO("No update available");
        curl_global_cleanup();
        return 0;
    } else if (result != OTA_RESULT_SUCCESS) {
        LOG_ERROR("Update check failed");
        curl_global_cleanup();
        return 1;
    }

    /* 执行升级 */
    LOG_INFO("Starting OTA upgrade to version %s", g_ctx.firmware.version);
    LOG_INFO("Release notes: %s", g_ctx.firmware.release_notes);

    result = perform_ota_upgrade();
    if (result != OTA_RESULT_SUCCESS) {
        LOG_ERROR("OTA upgrade failed: %d", result);
        report_upgrade_status("FAILED", "Upgrade failed");
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}
