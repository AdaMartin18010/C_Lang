# 无线协议开发

## 概述

无线协议开发是嵌入式系统领域的核心技术之一，涉及射频通信、协议栈实现、功耗优化和实时数据处理。本章节深入探讨无线通信基础原理、蓝牙协议栈架构、WiFi 协议机制，以及基于 C 语言的嵌入式无线开发实践。

---

## 无线通信基础

### 射频通信原理

无线通信系统从应用层到射频前端的分层架构：
- 协议栈层：数据链路层、网络层、传输层
- 基带处理层：编码器、调制器、扩频器
- 射频前端层：PLL频率合成、PA功率放大、LNA低噪放大
- 天线系统

关键参数包括频率范围（2.4 GHz ISM、5 GHz WiFi、Sub-GHz）、调制方式（GFSK、QPSK、OFDM、LoRa）和扩频技术（DSSS、FHSS）。

```c
/*
 * 无线信道模型与参数计算
 */
#include <math.h>
#include <complex.h>

// 自由空间路径损耗模型
double fspl(double distance_m, double frequency_hz)
{
    const double c = 299792458.0;
    double d_km = distance_m / 1000.0;
    double f_mhz = frequency_hz / 1e6;
    return 20.0 * log10(d_km) + 20.0 * log10(f_mhz) + 32.45;
}

// 接收信号强度计算
double rssi_calc(double tx_power_dbm, double distance_m, 
                  double frequency_hz, double extra_loss_db)
{
    double path_loss = fspl(distance_m, frequency_hz);
    return tx_power_dbm - path_loss - extra_loss_db;
}
```

### 无线数据包结构

```c
#include <stdint.h>
#include <string.h>

#define PREAMBLE_PATTERN 0xAA

typedef struct __attribute__((packed)) {
    uint8_t preamble[4];
    uint8_t sync_word[2];
    uint8_t length;
} PhyHeader;

typedef struct __attribute__((packed)) {
    uint16_t frame_control;
    uint8_t  seq_number;
    uint16_t dst_addr;
    uint16_t src_addr;
    uint16_t pan_id;
} MacHeader;

#define MAX_PAYLOAD_SIZE 127

typedef struct __attribute__((packed)) {
    PhyHeader phy;
    MacHeader mac;
    uint8_t payload[MAX_PAYLOAD_SIZE];
    uint16_t fcs;
} WirelessPacket;

// CRC-16/CCITT
uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
        }
    }
    return crc;
}
```

---

## 蓝牙协议栈

### BLE GATT 服务实现

```c
#include <stdint.h>
#include <string.h>

#define UUID_SERVICE_HEART_RATE  0x180D
#define UUID_CHAR_HEART_RATE     0x2A37
#define ATT_PERM_READ            0x01
#define ATT_PERM_WRITE           0x02
#define ATT_PERM_NOTIFY          0x10

typedef struct {
    uint16_t handle;
    uint16_t uuid16;
    uint8_t  permissions;
    uint16_t value_len;
    uint8_t  *value;
    void (*on_write)(uint16_t handle, const uint8_t *data, uint16_t len);
} GattAttribute;

typedef struct __attribute__((packed)) {
    uint8_t  flags;
    uint16_t heart_rate;
} HeartRateMeasurement;

HeartRateMeasurement hr_measurement = {
    .flags = 0x01,
    .heart_rate = 75,
};

static uint8_t hr_cccd[2] = {0x00, 0x00};

void on_hr_cccd_write(uint16_t handle, const uint8_t *data, uint16_t len)
{
    if (len >= 2) {
        hr_cccd[0] = data[0];
        if (hr_cccd[0] & 0x01) {
            printf("Heart rate notifications enabled\n");
        }
    }
}
```

### BLE 连接状态机

```c
typedef enum {
    STATE_IDLE,
    STATE_ADVERTISING,
    STATE_SCANNING,
    STATE_INITIATING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,
} BleState;

typedef struct {
    BleState state;
    uint16_t conn_handle;
    uint8_t  peer_addr[6];
    uint16_t conn_interval;
    uint16_t slave_latency;
} BleConnection;
```

---

## WiFi 协议

### 802.11 帧结构

```c
#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint8_t protocol_version : 2;
    uint8_t type : 2;
    uint8_t subtype : 4;
    uint8_t to_ds : 1;
    uint8_t from_ds : 1;
    uint8_t more_frag : 1;
    uint8_t retry : 1;
    uint8_t power_mgmt : 1;
    uint8_t more_data : 1;
    uint8_t protected_frame : 1;
    uint8_t order : 1;
} FrameControl;

typedef struct __attribute__((packed)) {
    FrameControl fc;
    uint16_t duration;
    uint8_t addr1[6];
    uint8_t addr2[6];
    uint8_t addr3[6];
    uint16_t seq_ctrl;
} WifiMacHeader;

typedef struct __attribute__((packed)) {
    WifiMacHeader header;
    uint8_t payload[2312];
    uint32_t fcs;
} WifiFrame;
```

### WiFi 状态机

```c
typedef enum {
    WIFI_STATE_IDLE,
    WIFI_STATE_SCANNING,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTING,
} WifiState;

typedef struct {
    WifiState state;
    uint8_t bssid[6];
    uint8_t ssid[32];
    uint8_t channel;
    int16_t rssi;
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t rx_errors;
    uint32_t tx_errors;
} WifiConnection;

// 连接管理
bool wifi_connect(WifiConnection *conn, const char *ssid, const char *psk);
bool wifi_disconnect(WifiConnection *conn);
bool wifi_scan(WifiConnection *conn, void (*on_result)(const char *ssid, int rssi));
```

---

## 嵌入式无线开发

### 射频驱动抽象层

```c
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RF_MODE_SLEEP,
    RF_MODE_STANDBY,
    RF_MODE_TX,
    RF_MODE_RX,
} RfMode;

typedef struct {
    uint32_t frequency_hz;
    int8_t   tx_power_dbm;
    uint32_t bitrate;
    uint8_t  modulation;
    uint16_t preamble_len;
    uint8_t  sync_word[8];
} RfConfig;

typedef struct {
    bool (*init)(const RfConfig *config);
    bool (*set_mode)(RfMode mode);
    bool (*send)(const uint8_t *data, uint16_t len);
    bool (*receive)(uint8_t *buffer, uint16_t *len, int32_t timeout_ms);
    bool (*set_frequency)(uint32_t freq_hz);
    bool (*set_power)(int8_t power_dbm);
    int16_t (*get_rssi)(void);
} RfDriver;

// 示例：SX1262 LoRa 芯片驱动
bool sx1262_init(const RfConfig *config)
{
    // 硬件复位
    // 配置频率
    // 配置调制参数
    // 配置包格式
    (void)config;
    return true;
}

bool sx1262_send(const uint8_t *data, uint16_t len)
{
    // 写入 FIFO
    // 启动发送
    // 等待发送完成
    (void)data;
    (void)len;
    return true;
}

RfDriver sx1262_driver = {
    .init = sx1262_init,
    .send = sx1262_send,
    // ... 其他函数
};
```

### 功耗管理

```c
#include <stdint.h>

// 低功耗模式管理
typedef enum {
    PWR_MODE_ACTIVE,
    PWR_MODE_SLEEP,
    PWR_MODE_DEEP_SLEEP,
    PWR_MODE_HIBERNATE,
} PowerMode;

typedef struct {
    PowerMode current_mode;
    uint32_t sleep_duration_ms;
    void (*enter_sleep)(void);
    void (*exit_sleep)(void);
    void (*schedule_wakeup)(uint32_t delay_ms);
} PowerManager;

// BLE 低功耗管理
void ble_enter_low_power(BleConnection *conn)
{
    // 计算下一次活动事件
    uint32_t next_event = conn->conn_interval * 1250; // us
    
    // 配置唤醒定时器
    // 进入低功耗模式
    // 保持连接状态
}

// 射频占空比控制
void rf_duty_cycle_control(uint32_t tx_time_ms, uint32_t period_ms)
{
    // 确保占空比符合法规限制
    // 例如：1% 占空比 = 10ms TX / 1s 周期
    uint32_t duty_cycle = (tx_time_ms * 100) / period_ms;
    if (duty_cycle > 1) {
        // 插入延迟以满足限制
    }
}
```

### 协议栈集成

```c
// 通用无线协议栈接口
typedef struct {
    int (*init)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*send)(const void *data, size_t len);
    int (*register_recv_callback)(void (*cb)(const void *data, size_t len));
    int (*get_status)(void);
} WirelessStack;

// BLE 协议栈实例
WirelessStack ble_stack = {
    .init = ble_stack_init,
    .start = ble_stack_start,
    .stop = ble_stack_stop,
    .send = ble_stack_send,
    .register_recv_callback = ble_register_callback,
    .get_status = ble_get_status,
};

// 应用层使用
void wireless_app_init(void)
{
    ble_stack.init();
    ble_stack.register_recv_callback(on_ble_data_received);
    ble_stack.start();
}
```

---

## 实践项目

### 无线传感器节点

```c
// 简单的传感器数据采集与无线传输
#include <stdint.h>
#include <stdio.h>

#define SENSOR_READ_INTERVAL_MS 1000
#define TRANSMIT_INTERVAL_MS    10000

typedef struct __attribute__((packed)) {
    uint16_t node_id;
    int16_t  temperature;    // 0.01 degree C
    uint16_t humidity;       // 0.01 %RH
    uint16_t battery_mv;
    uint32_t timestamp;
} SensorData;

void sensor_node_main(void)
{
    SensorData data = {
        .node_id = 0x0001,
    };
    
    while (1) {
        // 读取传感器
        data.temperature = read_temperature_sensor();
        data.humidity = read_humidity_sensor();
        data.battery_mv = read_battery_voltage();
        data.timestamp = get_timestamp();
        
        // 发送数据
        wireless_send(&data, sizeof(data));
        
        // 进入低功耗等待
        enter_low_power_mode(TRANSMIT_INTERVAL_MS);
    }
}
```

---

## 延伸阅读

- [蓝牙核心规范](https://www.bluetooth.com/specifications/bluetooth-core-specification/)
- [IEEE 802.11 标准](https://standards.ieee.org/standard/802.11-2020.html)
- [嵌入式无线通信](https://en.wikipedia.org/wiki/Wireless_sensor_network)
