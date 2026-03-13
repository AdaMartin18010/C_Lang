# 汽车 ECU 开发 (Automotive ECU Development)

## 目录

- [汽车 ECU 开发 (Automotive ECU Development)](#汽车-ecu-开发-automotive-ecu-development)
  - [目录](#目录)
  - [概述](#概述)
    - [ECU 分类与功能](#ecu-分类与功能)
  - [AUTOSAR 架构](#autosar-架构)
    - [AUTOSAR 概述](#autosar-概述)
  - [ECU 软件开发](#ecu-软件开发)
    - [AUTOSAR C 代码示例](#autosar-c-代码示例)
  - [诊断协议 UDS](#诊断协议-uds)
    - [UDS 服务概述](#uds-服务概述)
    - [UDS 服务实现](#uds-服务实现)
  - [刷写流程](#刷写流程)
    - [软件刷写流程图](#软件刷写流程图)
    - [Bootloader 实现](#bootloader-实现)
  - [实践案例](#实践案例)
    - [案例：发动机ECU信号处理](#案例发动机ecu信号处理)
  - [参考资源](#参考资源)
    - [标准文档](#标准文档)
    - [开发工具](#开发工具)

---

## 概述

电子控制单元（Electronic Control Unit, ECU）是现代汽车的核心组件，负责控制发动机、变速箱、制动、转向等关键系统。
随着汽车电子化程度提高，一辆现代汽车可能包含超过100个ECU。

### ECU 分类与功能

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                         汽车ECU分类体系                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    动力总成 ECU (Powertrain)                     │   │
│  │                                                                 │   │
│  │  • ECM (Engine Control Module)      - 发动机控制                │   │
│  │    - 燃油喷射控制                                               │   │
│  │    - 点火正时控制                                               │   │
│  │    - 空燃比控制                                                 │   │
│  │    - 排放控制 (OBD)                                             │   │
│  │                                                                 │   │
│  │  • TCM (Transmission Control)       - 变速箱控制                │   │
│  │    - 换挡逻辑                                                   │   │
│  │    - 离合器控制                                                 │   │
│  │    - 扭矩管理                                                   │   │
│  │                                                                 │   │
│  │  • HCU (Hybrid Control Unit)        - 混动控制单元              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    底盘 ECU (Chassis)                            │   │
│  │                                                                 │   │
│  │  • ABS (Anti-lock Braking)          - 防抱死制动                │   │
│  │  • ESP/ESC (Electronic Stability)   - 车身电子稳定              │   │
│  │  • EPS (Electric Power Steering)    - 电动助力转向              │   │
│  │  • EPB (Electric Parking Brake)     - 电子驻车制动              │   │
│  │  • Airbag Control Module            - 安全气囊控制              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    车身 ECU (Body)                               │   │
│  │                                                                 │   │
│  │  • BCM (Body Control Module)        - 车身控制模块              │   │
│  │    - 灯光控制、雨刷、门窗、中央锁                               │   │
│  │                                                                 │   │
│  │  • HVAC (Climate Control)           - 空调控制                  │   │
│  │  • Seat Control                     - 座椅控制                  │   │
│  │  • TPMS (Tire Pressure)             - 胎压监测                  │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## AUTOSAR 架构

### AUTOSAR 概述

AUTOSAR（AUTomotive Open System ARchitecture）是汽车领域广泛采用的标准化软件架构，旨在提高软件复用性、可移植性和可扩展性。

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    AUTOSAR 分层架构                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    应用层 (ASW - Application Layer)              │   │
│  │                                                                 │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │   │
│  │  │  SWC: 组件A  │  │  SWC: 组件B  │  │  SWC: 组件C  │             │   │
│  │  │  端口接口   │  │  端口接口   │  │  端口接口   │             │   │
│  │  │  Sender/   │←→│  Receiver  │←→│  Client/   │             │   │
│  │  │  Receiver  │  │            │  │  Server    │             │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘             │   │
│  │                                                                 │   │
│  │  • SWC (软件组件) - 实现业务逻辑                                │   │
│  │  • Runnable - 可执行实体，由事件触发                            │   │
│  │  • Port - 组件间通信接口                                        │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    运行时环境 (RTE - Runtime Environment)        │   │
│  │                                                                 │   │
│  │     RTE生成器根据ARXML配置自动生成组件间通信代码                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    基础软件 (BSW - Basic Software)               │   │
│  │                                                                 │   │
│  │  ┌─────────────────────────────────────────────────────────┐   │   │
│  │  │                 服务层 (Services Layer)                  │   │   │
│  │  │  • 诊断服务 (Diagnostic Services)                        │   │   │
│  │  │  • 存储服务 (NVRAM Manager)                              │   │   │
│  │  │  • 通信服务 (COM/PDU Router)                             │   │   │
│  │  └─────────────────────────────────────────────────────────┘   │   │
│  │                                                                 │   │
│  │  ┌─────────────────────────────────────────────────────────┐   │   │
│  │  │                 ECU抽象层 (ECU Abstraction Layer)        │   │   │
│  │  │  • IO硬件抽象 (IO Hardware Abstraction)                  │   │   │
│  │  │  • 通信硬件抽象 (CAN/LIN/Ethernet)                       │   │   │
│  │  └─────────────────────────────────────────────────────────┘   │   │
│  │                                                                 │   │
│  │  ┌─────────────────────────────────────────────────────────┐   │   │
│  │  │                 微控制器驱动层 (Microcontroller Drivers) │   │   │
│  │  │  • 微控制器驱动 (MCU Driver)                             │   │   │
│  │  │  • 存储器驱动 (Flash/EEPROM Driver)                      │   │   │
│  │  │  • PWM驱动                                               │   │   │
│  │  │  • ADC驱动                                               │   │   │
│  │  └─────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## ECU 软件开发

### AUTOSAR C 代码示例

```c
/**
 * @file SpeedSensor_SWC.c
 * @brief 轮速传感器软件组件 - AUTOSAR应用层实现
 */

#include "Rte_SpeedSensor.h"
#include "Os.h"

#define WHEEL_CIRCUMFERENCE_MM      2050.0f
#define PULSE_PER_REVOLUTION        48

typedef struct {
    float speed_kmh;
    uint16_t pulse_count;
    uint32_t last_time_ms;
    boolean valid;
} WheelSpeedData_t;

static WheelSpeedData_t g_wheels[4];
static float g_vehicle_speed = 0.0f;

/**
 * @brief 初始化Runnable
 */
FUNC(void, SpeedSensor_CODE) SpeedSensor_Init(void)
{
    for (int i = 0; i < 4; i++) {
        g_wheels[i].speed_kmh = 0.0f;
        g_wheels[i].valid = FALSE;
    }
}

/**
 * @brief 周期性10ms处理Runnable
 */
FUNC(void, SpeedSensor_CODE) SpeedSensor_MainFunction(void)
{
    /* 读取传感器数据 */
    for (int i = 0; i < 4; i++) {
        uint16_t current_pulse;
        Rte_Call_RP_IoHwAb_GetPulseCounter(i, &current_pulse);

        uint32_t current_time = GetCounterValue();
        uint16_t delta_pulse = current_pulse - g_wheels[i].pulse_count;
        uint32_t delta_time = current_time - g_wheels[i].last_time_ms;

        if (delta_time > 0) {
            float distance_mm = (float)delta_pulse * WHEEL_CIRCUMFERENCE_MM
                               / PULSE_PER_REVOLUTION;
            float time_s = (float)delta_time / 1000.0f;
            g_wheels[i].speed_kmh = (distance_mm / time_s) * 3.6f / 1000.0f;
            g_wheels[i].valid = TRUE;
        }

        g_wheels[i].pulse_count = current_pulse;
        g_wheels[i].last_time_ms = current_time;
    }

    /* 计算整车速度 (平均值) */
    float sum = 0.0f;
    int valid_count = 0;
    for (int i = 0; i < 4; i++) {
        if (g_wheels[i].valid) {
            sum += g_wheels[i].speed_kmh;
            valid_count++;
        }
    }
    g_vehicle_speed = (valid_count > 0) ? sum / valid_count : 0.0f;

    /* 通过RTE发送数据 */
    Rte_Write_PP_SpeedData_VehicleSpeed(g_vehicle_speed);
}
```

---

## 诊断协议 UDS

### UDS 服务概述

UDS（Unified Diagnostic Services，统一诊断服务，ISO 14229）是汽车诊断的标准协议。

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    UDS 协议栈与分层                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    应用层 (UDS Services)                         │   │
│  │                                                                 │   │
│  │  $10 - 诊断会话控制 (Diagnostic Session Control)                │   │
│  │  $11 - ECU复位 (ECU Reset)                                       │   │
│  │  $22 - 通过DID读取数据 (Read Data By Identifier)                │   │
│  │  $2E - 通过DID写入数据 (Write Data By Identifier)               │   │
│  │  $31 - 例程控制 (Routine Control)                                │   │
│  │  $34 - 请求下载 (Request Download)                               │   │
│  │  $36 - 传输数据 (Transfer Data)                                  │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    传输层 (ISO-TP / CAN TP)                      │   │
│  │                                                                 │   │
│  │  单帧 (SF)  - 数据长度 <= 7字节                                   │   │
│  │  首帧 (FF)  - 多帧传输开始                                       │   │
│  │  连续帧 (CF) - 后续数据帧                                        │   │
│  │  流控帧 (FC) - 流控制                                            │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    网络层 (CAN / CAN-FD)                         │   │
│  │                                                                 │   │
│  │  • CAN 2.0B: 500kbps, 29-bit扩展ID                               │   │
│  │  • CAN-FD: 2Mbps/5Mbps, 64字节数据段                            │   │
│  │                                                                 │   │
│  │  诊断请求ID: 0x7E0 (物理地址) / 0x7DF (功能地址)                 │   │
│  │  诊断响应ID: 0x7E8 (ECU响应地址)                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### UDS 服务实现

```c
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
```

---

## 刷写流程

### 软件刷写流程图

```text
┌─────────────────────────────────────────────────────────────────────────┐
│                    ECU 软件刷写流程 (Bootloader)                         │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  刷写工具 (诊断仪)                           ECU                        │
│                                                                         │
│       │                                      │                          │
│       │  1. 进入编程会话 ($10 02)            │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 切换Bootloader模式       │
│       │  2. 正响应                            │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
│       │  3. 请求下载 ($34)                    │                          │
│       │  [地址、大小、格式]                  │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 擦除Flash区域            │
│       │  4. 正响应 (块大小)                   │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
│       │  5. 传输数据 ($36)                    │                          │
│       │  [Block #1, 数据...]                 │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 写入Flash                │
│       │  6. 正响应                            │                          │
│       │←─────────────────────────────────────│                          │
│       │          ... 重复步骤5-6 ...         │                          │
│       │                                      │                          │
│       │  n. 请求退出传输 ($37)                │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 校验完整性               │
│       │  n+1. 正响应                          │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
│       │  n+2. 校验软件 ($31 01 FF 02)         │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ CRC校验、签名验证        │
│       │  n+3. 正响应 (校验结果)               │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
│       │  n+4. 软件一致性检查 ($31 01 FF 01)   │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 对比编程指纹             │
│       │  n+5. 正响应                          │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
│       │  n+6. 复位ECU ($11 01)                │                          │
│       │─────────────────────────────────────→│                          │
│       │                                      │ 跳转应用程序             │
│       │  n+7. 正响应 (或无响应直接复位)       │                          │
│       │←─────────────────────────────────────│                          │
│       │                                      │                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### Bootloader 实现

```c
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
```

---

## 实践案例

### 案例：发动机ECU信号处理

```c
/**
 * @file engine_control.c
 * @brief 发动机控制ECU - 点火正时计算示例
 */

#include "engine_control.h"

/* 曲轴位置 */
typedef struct {
    uint16_t rpm;              /* 发动机转速 */
    uint16_t crank_angle;      /* 曲轴角度 (0-720度, 四冲程) */
    uint8_t cylinder_phase;    /* 气缸相位 */
} EnginePosition_t;

/* 点火参数 */
typedef struct {
    uint16_t ignition_advance; /* 点火提前角 (0.1度) */
    uint16_t dwell_time_us;    /* 充电时间 */
    uint8_t coil_number;       /* 点火线圈号 */
} IgnitionTiming_t;

/* 点火MAP (简化示例: 转速x负荷) */
static const uint16_t ignition_map[16][16] = {
    /* 负荷: 低 → 高 */
    /* 转速: 低 → 高 */
    {50, 50, 45, 40, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},  /* 500 RPM */
    {50, 50, 45, 40, 35, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
    /* ... 更多行 ... */
    {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 30, 30, 28, 28, 28, 28},  /* 6000 RPM */
};

/**
 * @brief 计算点火正时
 */
void CalculateIgnitionTiming(EnginePosition_t *pos,
                              EngineLoad_t *load,
                              IgnitionTiming_t *timing)
{
    uint8_t rpm_idx, load_idx;
    uint16_t base_advance;
    int16_t corrections = 0;

    /* MAP索引计算 */
    rpm_idx = (pos->rpm < 500) ? 0 : (pos->rpm - 500) / 367;
    if (rpm_idx > 15) rpm_idx = 15;

    load_idx = (uint8_t)(load->percent / 6.25f);
    if (load_idx > 15) load_idx = 15;

    /* 基础点火提前角 */
    base_advance = ignition_map[rpm_idx][load_idx];

    /* 冷却液温度修正 */
    if (load->coolant_temp < 70) {
        corrections += 5;  /* 冷启动加浓 */
    }

    /* 爆震修正 */
    if (load->knock_detected) {
        corrections -= 3;
    }

    /* 最终点火提前角 */
    timing->ignition_advance = base_advance + corrections;

    /* 计算充电时间 (基于线圈特性) */
    timing->dwell_time_us = 3000;  /* 3ms */

    /* 计算点火线圈 */
    timing->coil_number = (pos->cylinder_phase % 4);
}

/**
 * @brief 点火控制ISR (曲轴每度触发)
 */
void CRANK_IRQHandler(void)
{
    static EnginePosition_t pos = {0};
    static IgnitionTiming_t timing;
    EngineLoad_t load;

    /* 更新曲轴位置 */
    pos.crank_angle++;
    if (pos.crank_angle >= 720) {
        pos.crank_angle = 0;
        pos.cylinder_phase = 0;
    }

    /* 每90度计算一次点火 (四冲程8次点火) */
    if (pos.crank_angle % 90 == 0) {
        /* 读取传感器 */
        load.percent = ADC_Read(ADC_CHANNEL_MAP);  /* 进气歧管压力 */
        load.coolant_temp = ADC_Read(ADC_CHANNEL_CTS);
        load.knock_detected = GPIO_Read(KNOCK_PIN);

        /* 计算点火正时 */
        CalculateIgnitionTiming(&pos, &load, &timing);

        /* 设置点火定时器 */
        uint16_t ignition_angle = 360 - timing.ignition_advance;
        if (pos.crank_angle >= ignition_angle - 30) {
            /* 开始充电 (dwell) */
            SetCoilCharge(timing.coil_number, true);
        }
        if (pos.crank_angle >= ignition_angle) {
            /* 点火 */
            SetCoilCharge(timing.coil_number, false);
            pos.cylinder_phase++;
        }
    }
}
```

---

## 参考资源

### 标准文档

| 标准 | 描述 |
|------|------|
| ISO 11898 | CAN总线标准 |
| ISO 14229 | UDS诊断服务 |
| ISO 15765 | CAN诊断传输层 |
| ISO 26262 | 汽车功能安全 |
| AUTOSAR Classic | 经典平台标准 |
| AUTOSAR Adaptive | 自适应平台标准 |

### 开发工具

- **Vector CANoe/CANalyzer**: 总线分析工具
- **dSPACE**: 硬件在环仿真
- **ETAS INCA**: ECU标定
- **Lauterbach TRACE32**: 调试器
- **EB tresos**: AUTOSAR配置工具

---

*最后更新: 2026年3月*:
