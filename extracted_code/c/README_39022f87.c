/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\README.md
 * Line: 450
 * Language: c
 * Block ID: 39022f87
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file bootloader.c
 * @brief ECU Bootloader 实现
 */

#include "bootloader.h"
#include "flash_driver.h"
#include "uds_handler.h"

#define BOOTLOADER_VERSION          0x0100
#define APP_START_ADDRESS           0x00008000
#define APP_MAX_SIZE                0x00070000  /* 448KB */

typedef enum {
    BL_STATE_IDLE,
    BL_STATE_PROGRAMMING,
    BL_STATE_DOWNLOADING,
    BL_STATE_VERIFYING,
    BL_STATE_JUMP_TO_APP
} BootloaderState_t;

typedef struct {
    uint32_t start_addr;
    uint32_t total_size;
    uint32_t received_size;
    uint32_t block_counter;
    uint32_t crc32;
} DownloadInfo_t;

static BootloaderState_t g_state = BL_STATE_IDLE;
static DownloadInfo_t g_download = {0};

/**
 * @brief Bootloader 主函数
 */
void Bootloader_Main(void)
{
    /* 初始化 */
    Flash_Init();
    UDS_Init();
    CAN_Init();

    /* 检查是否需要进入编程模式 */
    if (!Check_ProgrammingRequest() && IsAppValid()) {
        /* 跳转到应用程序 */
        JumpToApplication();
    }

    /* 编程模式主循环 */
    while (1) {
        /* 处理CAN消息 */
        CAN_ProcessMessages();

        /* 处理UDS服务 */
        UDS_ProcessServices();

        /* 状态机处理 */
        switch (g_state) {
            case BL_STATE_JUMP_TO_APP:
                if (IsAppValid()) {
                    JumpToApplication();
                }
                break;
            default:
                break;
        }
    }
}

/**
 * @brief 请求下载处理 ($34)
 */
uint8_t Bootloader_RequestDownload(uint32_t addr, uint32_t size)
{
    /* 验证地址范围 */
    if (addr < APP_START_ADDRESS ||
        addr + size > APP_START_ADDRESS + APP_MAX_SIZE) {
        return UDS_ERROR_REQUEST_OUT_OF_RANGE;
    }

    /* 保存下载信息 */
    g_download.start_addr = addr;
    g_download.total_size = size;
    g_download.received_size = 0;
    g_download.block_counter = 0;
    g_download.crc32 = 0xFFFFFFFF;

    /* 擦除Flash */
    if (Flash_EraseSector(addr, size) != FLASH_OK) {
        return UDS_ERROR_CONDITIONS_NOT_CORRECT;
    }

    g_state = BL_STATE_DOWNLOADING;
    return UDS_OK;
}

/**
 * @brief 传输数据处理 ($36)
 */
uint8_t Bootloader_TransferData(uint8_t *data, uint16_t len)
{
    uint32_t write_addr;

    if (g_state != BL_STATE_DOWNLOADING) {
        return UDS_ERROR_REQUEST_SEQUENCE_ERROR;
    }

    /* 计算写入地址 */
    write_addr = g_download.start_addr + g_download.received_size;

    /* 写入Flash */
    if (Flash_Write(write_addr, data, len) != FLASH_OK) {
        return UDS_ERROR_CONDITIONS_NOT_CORRECT;
    }

    /* 更新CRC */
    g_download.crc32 = Calculate_CRC32(g_download.crc32, data, len);

    /* 更新进度 */
    g_download.received_size += len;
    g_download.block_counter++;

    return UDS_OK;
}

/**
 * @brief 校验应用程序有效性
 */
bool IsAppValid(void)
{
    uint32_t *app_vector = (uint32_t*)APP_START_ADDRESS;
    uint32_t stack_pointer = app_vector[0];
    uint32_t reset_vector = app_vector[1];

    /* 检查栈顶地址有效性 */
    if (stack_pointer < 0x20000000 || stack_pointer > 0x20080000) {
        return false;
    }

    /* 检查复位向量有效性 */
    if (reset_vector < APP_START_ADDRESS ||
        reset_vector > APP_START_ADDRESS + APP_MAX_SIZE) {
        return false;
    }

    /* 检查CRC/签名 (可选) */
    /* ... */

    return true;
}

/**
 * @brief 跳转到应用程序
 */
void JumpToApplication(void)
{
    uint32_t *app_vector = (uint32_t*)APP_START_ADDRESS;
    uint32_t stack_pointer = app_vector[0];
    uint32_t reset_vector = app_vector[1];

    /* 关闭中断 */
    __disable_irq();

    /* 设置主栈指针 */
    __set_MSP(stack_pointer);

    /* 跳转到应用程序复位向量 */
    typedef void (*AppResetHandler)(void);
    AppResetHandler app_reset = (AppResetHandler)reset_vector;
    app_reset();
}
