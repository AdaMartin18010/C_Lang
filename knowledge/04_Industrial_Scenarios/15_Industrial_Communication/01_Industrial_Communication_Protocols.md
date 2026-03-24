# 工业通信协议完全指南

## 目录

- [工业通信协议完全指南](#工业通信协议完全指南)
  - [目录](#目录)
  - [1. 工业通信概述](#1-工业通信概述)
    - [1.1 工业通信的特点](#11-工业通信的特点)
    - [1.2 工业通信协议分类](#12-工业通信协议分类)
    - [1.3 工业通信OSI模型](#13-工业通信osi模型)
  - [2. RS-232/RS-485串口通信](#2-rs-232rs-485串口通信)
    - [2.1 串口通信基础](#21-串口通信基础)
      - [2.1.1 RS-232标准](#211-rs-232标准)
      - [2.1.2 RS-485标准](#212-rs-485标准)
    - [2.2 Linux串口编程完整实现](#22-linux串口编程完整实现)
    - [2.3 Windows串口编程实现](#23-windows串口编程实现)
  - [3. Modbus RTU协议完整实现](#3-modbus-rtu协议完整实现)
    - [3.1 Modbus协议概述](#31-modbus协议概述)
    - [3.2 Modbus RTU完整C实现](#32-modbus-rtu完整c实现)
    - [3.3 Modbus从机实现](#33-modbus从机实现)
  - [4. Modbus TCP协议实现](#4-modbus-tcp协议实现)
    - [4.1 Modbus TCP协议概述](#41-modbus-tcp协议概述)
    - [4.2 Modbus TCP完整C实现](#42-modbus-tcp完整c实现)
  - [5. CAN总线通信](#5-can总线通信)
    - [5.1 CAN总线概述](#51-can总线概述)
    - [5.2 Linux SocketCAN完整实现](#52-linux-socketcan完整实现)
  - [6. CANopen协议](#6-canopen协议)
    - [6.1 CANopen概述](#61-canopen概述)
    - [6.2 CANopen SDO通信](#62-canopen-sdo通信)
  - [7. EtherCAT协议介绍](#7-ethercat协议介绍)
    - [7.1 EtherCAT概述](#71-ethercat概述)
    - [7.2 EtherCAT主站简单示例](#72-ethercat主站简单示例)
  - [8. Profinet/Profibus](#8-profinetprofibus)
    - [8.1 Profibus概述](#81-profibus概述)
    - [8.2 Profinet概述](#82-profinet概述)
  - [9. 工业以太网](#9-工业以太网)
    - [9.1 工业以太网概述](#91-工业以太网概述)
    - [9.2 EtherNet/IP实现示例](#92-ethernetip实现示例)
  - [10. 无线工业通信](#10-无线工业通信)
    - [10.1 工业无线概述](#101-工业无线概述)
    - [10.2 4G/5G工业DTU实现](#102-4g5g工业dtu实现)
  - [11. 实际项目:多协议转换网关](#11-实际项目多协议转换网关)
    - [11.1 项目概述](#111-项目概述)
    - [11.2 网关完整代码](#112-网关完整代码)
    - [11.3 Makefile编译脚本](#113-makefile编译脚本)
  - [12. 总结与最佳实践](#12-总结与最佳实践)
    - [12.1 协议选择指南](#121-协议选择指南)
    - [12.2 开发最佳实践](#122-开发最佳实践)
    - [12.3 参考资料](#123-参考资料)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 工业通信概述

### 1.1 工业通信的特点

工业通信协议是工业自动化系统的神经系统，具有以下特点：

| 特性 | 要求 | 说明 |
|------|------|------|
| 实时性 | 毫秒/微秒级 | 控制指令必须在严格时限内送达 |
| 可靠性 | 99.999% | 工业环境电磁干扰强，需容错机制 |
| 确定性 | 固定延迟 | 数据传输时间可预测 |
| 安全性 | 高 | 防止未授权访问和操作 |
| 兼容性 | 开放标准 | 多厂商设备互联互通 |

### 1.2 工业通信协议分类

```
┌─────────────────────────────────────────────────────────────┐
│                    工业通信协议体系                          │
├─────────────────────────────────────────────────────────────┤
│  现场级(Field Level)    →    控制级(Control Level)           │
│  ├─ 串口通信                ├─ 工业以太网                     │
│  │  ├─ RS-232               │  ├─ EtherNet/IP               │
│  │  ├─ RS-485               │  ├─ Profinet                  │
│  │  └─ RS-422               │  ├─ EtherCAT                  │
│  ├─ 现场总线                │  └─ Modbus TCP                 │
│  │  ├─ CAN/CANopen          ├─ 控制系统总线                  │
│  │  ├─ Profibus DP          │  └─ PCI/PCIe                   │
│  │  ├─ Modbus RTU           └──────────────────────────────│
│  │  └─ DeviceNet                                             │
│  └─ 无线通信                                                │
│     ├─ Wireless HART                                        │
│     ├─ ZigBee                                               │
│     └─ 4G/5G/WiFi                                           │
└─────────────────────────────────────────────────────────────┘
```

### 1.3 工业通信OSI模型

工业协议通常采用简化OSI模型：

```
┌─────────────────────────────────────────────────────────────┐
│  OSI层次    │  工业协议对应层    │  典型功能                  │
├─────────────────────────────────────────────────────────────┤
│  应用层     │  应用层            │  设备配置、数据表示         │
│  表示层     │  (合并到应用层)    │  数据编码                   │
│  会话层     │  (合并到应用层)    │  会话管理                   │
├─────────────────────────────────────────────────────────────┤
│  传输层     │  传输层            │  TCP/UDP、可靠传输          │
├─────────────────────────────────────────────────────────────┤
│  网络层     │  网络层            │  IP地址、路由               │
├─────────────────────────────────────────────────────────────┤
│  数据链路层 │  数据链路层        │  MAC地址、帧格式、CRC       │
│  物理层     │  物理层            │  电气特性、线缆、连接器     │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. RS-232/RS-485串口通信

### 2.1 串口通信基础

#### 2.1.1 RS-232标准

RS-232是最经典的串行通信标准，定义于1960年代。

```
┌─────────────────────────────────────────────────────────────┐
│                    RS-232 DB9连接器                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│    5 4 3 2 1                                                │
│   ┌─────────┐                                               │
│   │ ● ● ● ● ●│   1: DCD (载波检测)                          │
│   │  ● ● ● ● │   2: RXD (接收数据)                          │
│   └─────────┘    3: TXD (发送数据)                          │
│    9 8 7 6       4: DTR (数据终端就绪)                      │
│                  5: GND (信号地)                            │
│                  6: DSR (数据设备就绪)                      │
│                  7: RTS (请求发送)                          │
│                  8: CTS (清除发送)                          │
│                  9: RI (振铃指示)                           │
└─────────────────────────────────────────────────────────────┘
```

**RS-232电气特性：**

- 逻辑1: -3V ~ -15V
- 逻辑0: +3V ~ +15V
- 最大传输距离: 15米
- 最大速率: 20Kbps~230.4Kbps

#### 2.1.2 RS-485标准

RS-485是差分信号标准，更适合工业环境。

```
┌─────────────────────────────────────────────────────────────┐
│                    RS-485 差分信号                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   发送端:                                                   │
│   逻辑1: A > B (A=+2V~+6V, B=-2V~-6V)                       │
│   逻辑0: A < B (A=-2V~-6V, B=+2V~+6V)                       │
│                                                             │
│   接线方式 (半双工):                                        │
│   ┌─────────┐                    ┌─────────┐               │
│   │ 设备A   │────A───────────────│ 设备B   │               │
│   │         │────B───────────────│         │               │
│   │    TX/RX│────GND─────────────│TX/RX    │               │
│   └─────────┘   120Ω终端电阻     └─────────┘               │
│                  (两端各一个)                                │
└─────────────────────────────────────────────────────────────┘
```

**RS-485特点：**

- 差分信号，抗干扰能力强
- 多点通信: 32/128/256节点
- 传输距离: 1200米(低速率)
- 速率: 最高10Mbps
- 支持半双工/全双工

### 2.2 Linux串口编程完整实现

```c
/**
 * @file serial_port.h
 * @brief RS-232/RS-485串口通信头文件
 * @version 1.0.0
 * @date 2024
 */

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 串口配置结构体 */
typedef struct {
    char device[64];        /* 设备路径,如 "/dev/ttyUSB0" */
    uint32_t baudrate;      /* 波特率 */
    uint8_t databits;       /* 数据位: 5,6,7,8 */
    uint8_t stopbits;       /* 停止位: 1,2 */
    char parity;            /* 校验: 'N'=无, 'O'=奇, 'E'=偶 */
    uint32_t timeout_ms;    /* 读超时时间(ms) */
    bool rs485_mode;        /* RS-485模式 */
    bool rts_on_send;       /* 发送时RTS有效 */
} serial_config_t;

/* 串口句柄 */
typedef struct {
    int fd;                     /* 文件描述符 */
    serial_config_t config;     /* 配置信息 */
    struct termios old_tio;     /* 原始配置(用于恢复) */
    uint64_t tx_bytes;          /* 发送字节计数 */
    uint64_t rx_bytes;          /* 接收字节计数 */
    uint64_t tx_errors;         /* 发送错误计数 */
    uint64_t rx_errors;         /* 接收错误计数 */
} serial_handle_t;

/* 波特率定义 */
#define SERIAL_BAUD_9600    9600
#define SERIAL_BAUD_19200   19200
#define SERIAL_BAUD_38400   38400
#define SERIAL_BAUD_57600   57600
#define SERIAL_BAUD_115200  115200
#define SERIAL_BAUD_230400  230400
#define SERIAL_BAUD_460800  460800
#define SERIAL_BAUD_921600  921600

/* 函数声明 */
int serial_init(serial_handle_t *handle, const serial_config_t *config);
void serial_close(serial_handle_t *handle);
int serial_send(serial_handle_t *handle, const uint8_t *data, size_t len);
int serial_receive(serial_handle_t *handle, uint8_t *buffer, size_t max_len, uint32_t timeout_ms);
int serial_flush(serial_handle_t *handle);
int serial_set_dtr(serial_handle_t *handle, bool state);
int serial_set_rts(serial_handle_t *handle, bool state);
int serial_get_modem_status(serial_handle_t *handle);
void serial_get_stats(serial_handle_t *handle, uint64_t *tx, uint64_t *rx, uint64_t *tx_err, uint64_t *rx_err);
const char* serial_strerror(int error);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_PORT_H */
```

```c
/**
 * @file serial_port.c
 * @brief RS-232/RS-485串口通信实现
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include "serial_port.h"

/* 错误码定义 */
#define SERIAL_OK           0
#define SERIAL_ERR_OPEN    -1
#define SERIAL_ERR_CONFIG  -2
#define SERIAL_ERR_WRITE   -3
#define SERIAL_ERR_READ    -4
#define SERIAL_ERR_SELECT  -5
#define SERIAL_ERR_TIMEOUT -6

/**
 * @brief 将波特率转换为termios常量
 */
static speed_t baudrate_to_constant(uint32_t baudrate)
{
    switch (baudrate) {
        case 9600:    return B9600;
        case 19200:   return B19200;
        case 38400:   return B38400;
        case 57600:   return B57600;
        case 115200:  return B115200;
        case 230400:  return B230400;
        case 460800:  return B460800;
        case 921600:  return B921600;
        default:      return B9600;
    }
}

/**
 * @brief 初始化串口
 * @param handle 串口句柄
 * @param config 配置参数
 * @return 0成功,负数错误码
 */
int serial_init(serial_handle_t *handle, const serial_config_t *config)
{
    struct termios new_tio;
    struct serial_rs485 rs485conf;

    if (!handle || !config) {
        return SERIAL_ERR_CONFIG;
    }

    /* 复制配置 */
    memcpy(&handle->config, config, sizeof(serial_config_t));

    /* 打开串口设备 */
    handle->fd = open(config->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (handle->fd < 0) {
        perror("open");
        return SERIAL_ERR_OPEN;
    }

    /* 清除非阻塞模式 */
    fcntl(handle->fd, F_SETFL, 0);

    /* 保存原始配置 */
    if (tcgetattr(handle->fd, &handle->old_tio) < 0) {
        perror("tcgetattr");
        close(handle->fd);
        return SERIAL_ERR_CONFIG;
    }

    /* 初始化新配置 */
    memset(&new_tio, 0, sizeof(new_tio));

    /* 设置控制模式: 本地连接,使能接收 */
    new_tio.c_cflag = CLOCAL | CREAD;

    /* 设置波特率 */
    speed_t baud = baudrate_to_constant(config->baudrate);
    cfsetispeed(&new_tio, baud);
    cfsetospeed(&new_tio, baud);

    /* 设置数据位 */
    switch (config->databits) {
        case 5: new_tio.c_cflag |= CS5; break;
        case 6: new_tio.c_cflag |= CS6; break;
        case 7: new_tio.c_cflag |= CS7; break;
        case 8: new_tio.c_cflag |= CS8; break;
        default: new_tio.c_cflag |= CS8; break;
    }

    /* 设置停止位 */
    if (config->stopbits == 2) {
        new_tio.c_cflag |= CSTOPB;
    }

    /* 设置校验 */
    switch (config->parity) {
        case 'O': /* 奇校验 */
            new_tio.c_cflag |= PARENB | PARODD;
            break;
        case 'E': /* 偶校验 */
            new_tio.c_cflag |= PARENB;
            new_tio.c_cflag &= ~PARODD;
            break;
        case 'N': /* 无校验 */
        default:
            new_tio.c_cflag &= ~PARENB;
            break;
    }

    /* 设置输入模式: 原始输入 */
    new_tio.c_iflag = IGNPAR;
    new_tio.c_iflag &= ~(ICRNL | IXON | IXOFF | IXANY);

    /* 设置输出模式: 原始输出 */
    new_tio.c_oflag = 0;

    /* 设置本地模式 */
    new_tio.c_lflag = 0;
    new_tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    /* 设置最小字符数和超时 */
    new_tio.c_cc[VMIN] = 0;
    new_tio.c_cc[VTIME] = 0;

    /* 清除输入输出缓冲区 */
    tcflush(handle->fd, TCIOFLUSH);

    /* 应用配置 */
    if (tcsetattr(handle->fd, TCSANOW, &new_tio) < 0) {
        perror("tcsetattr");
        close(handle->fd);
        return SERIAL_ERR_CONFIG;
    }

    /* RS-485模式配置 */
    if (config->rs485_mode) {
        memset(&rs485conf, 0, sizeof(rs485conf));
        rs485conf.flags = SER_RS485_ENABLED;

        if (config->rts_on_send) {
            rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        } else {
            rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
        }

        if (ioctl(handle->fd, TIOCSRS485, &rs485conf) < 0) {
            /* RS-485配置失败,但不影响基本功能 */
            fprintf(stderr, "Warning: RS-485 mode configuration failed\n");
        }
    }

    /* 初始化统计 */
    handle->tx_bytes = 0;
    handle->rx_bytes = 0;
    handle->tx_errors = 0;
    handle->rx_errors = 0;

    return SERIAL_OK;
}

/**
 * @brief 关闭串口
 */
void serial_close(serial_handle_t *handle)
{
    if (!handle || handle->fd < 0) {
        return;
    }

    /* 恢复原始配置 */
    tcsetattr(handle->fd, TCSANOW, &handle->old_tio);

    /* 关闭设备 */
    close(handle->fd);
    handle->fd = -1;
}

/**
 * @brief 发送数据
 * @return 实际发送字节数,负数错误
 */
int serial_send(serial_handle_t *handle, const uint8_t *data, size_t len)
{
    ssize_t written;
    size_t total = 0;

    if (!handle || handle->fd < 0 || !data) {
        return SERIAL_ERR_CONFIG;
    }

    while (total < len) {
        written = write(handle->fd, data + total, len - total);

        if (written < 0) {
            if (errno == EINTR) {
                continue;
            }
            handle->tx_errors++;
            return SERIAL_ERR_WRITE;
        }

        total += written;
    }

    /* 等待发送完成 */
    tcdrain(handle->fd);

    handle->tx_bytes += total;
    return (int)total;
}

/**
 * @brief 接收数据
 * @return 实际接收字节数,0超时,负数错误
 */
int serial_receive(serial_handle_t *handle, uint8_t *buffer, size_t max_len,
                   uint32_t timeout_ms)
{
    fd_set readfds;
    struct timeval tv;
    ssize_t n;
    int ret;

    if (!handle || handle->fd < 0 || !buffer || max_len == 0) {
        return SERIAL_ERR_CONFIG;
    }

    /* 设置超时 */
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    FD_ZERO(&readfds);
    FD_SET(handle->fd, &readfds);

    /* 等待数据 */
    ret = select(handle->fd + 1, &readfds, NULL, NULL,
                 timeout_ms > 0 ? &tv : NULL);

    if (ret < 0) {
        if (errno == EINTR) {
            return 0;
        }
        handle->rx_errors++;
        return SERIAL_ERR_SELECT;
    }

    if (ret == 0) {
        /* 超时 */
        return SERIAL_ERR_TIMEOUT;
    }

    /* 读取数据 */
    n = read(handle->fd, buffer, max_len);

    if (n < 0) {
        handle->rx_errors++;
        return SERIAL_ERR_READ;
    }

    handle->rx_bytes += n;
    return (int)n;
}

/**
 * @brief 清空缓冲区
 */
int serial_flush(serial_handle_t *handle)
{
    if (!handle || handle->fd < 0) {
        return SERIAL_ERR_CONFIG;
    }

    return tcflush(handle->fd, TCIOFLUSH);
}

/**
 * @brief 设置DTR信号
 */
int serial_set_dtr(serial_handle_t *handle, bool state)
{
    int status;

    if (!handle || handle->fd < 0) {
        return SERIAL_ERR_CONFIG;
    }

    if (ioctl(handle->fd, TIOCMGET, &status) < 0) {
        return -1;
    }

    if (state) {
        status |= TIOCM_DTR;
    } else {
        status &= ~TIOCM_DTR;
    }

    return ioctl(handle->fd, TIOCMSET, &status);
}

/**
 * @brief 设置RTS信号
 */
int serial_set_rts(serial_handle_t *handle, bool state)
{
    int status;

    if (!handle || handle->fd < 0) {
        return SERIAL_ERR_CONFIG;
    }

    if (ioctl(handle->fd, TIOCMGET, &status) < 0) {
        return -1;
    }

    if (state) {
        status |= TIOCM_RTS;
    } else {
        status &= ~TIOCM_RTS;
    }

    return ioctl(handle->fd, TIOCMSET, &status);
}

/**
 * @brief 获取Modem状态
 */
int serial_get_modem_status(serial_handle_t *handle)
{
    int status = 0;

    if (!handle || handle->fd < 0) {
        return SERIAL_ERR_CONFIG;
    }

    ioctl(handle->fd, TIOCMGET, &status);
    return status;
}

/**
 * @brief 获取统计信息
 */
void serial_get_stats(serial_handle_t *handle, uint64_t *tx, uint64_t *rx,
                      uint64_t *tx_err, uint64_t *rx_err)
{
    if (tx) *tx = handle->tx_bytes;
    if (rx) *rx = handle->rx_bytes;
    if (tx_err) *tx_err = handle->tx_errors;
    if (rx_err) *rx_err = handle->rx_errors;
}

/**
 * @brief 错误码转字符串
 */
const char* serial_strerror(int error)
{
    switch (error) {
        case SERIAL_OK:          return "Success";
        case SERIAL_ERR_OPEN:    return "Failed to open device";
        case SERIAL_ERR_CONFIG:  return "Configuration error";
        case SERIAL_ERR_WRITE:   return "Write error";
        case SERIAL_ERR_READ:    return "Read error";
        case SERIAL_ERR_SELECT:  return "Select error";
        case SERIAL_ERR_TIMEOUT: return "Timeout";
        default:                 return "Unknown error";
    }
}
```

### 2.3 Windows串口编程实现

```c
/**
 * @file serial_port_win32.c
 * @brief Windows平台串口通信实现
 */

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Windows串口句柄结构 */
typedef struct {
    HANDLE hComm;               /* 串口句柄 */
    char device[64];            /* 设备名 */
    uint32_t baudrate;          /* 波特率 */
    DCB dcb;                    /* 设备控制块 */
    COMMTIMEOUTS timeouts;      /* 超时设置 */
    uint64_t tx_bytes;          /* 发送字节 */
    uint64_t rx_bytes;          /* 接收字节 */
} win_serial_handle_t;

/**
 * @brief 初始化Windows串口
 */
int win_serial_init(win_serial_handle_t *handle, const char *port,
                    uint32_t baudrate, uint8_t databits,
                    uint8_t stopbits, char parity)
{
    char full_port_name[16];

    snprintf(full_port_name, sizeof(full_port_name), "\\\\.\\%s", port);

    /* 打开串口 */
    handle->hComm = CreateFile(
        full_port_name,
        GENERIC_READ | GENERIC_WRITE,
        0,                          /* 独占访问 */
        NULL,                       /* 默认安全属性 */
        OPEN_EXISTING,
        0,                          /* 非重叠I/O */
        NULL
    );

    if (handle->hComm == INVALID_HANDLE_VALUE) {
        return -1;
    }

    /* 获取当前配置 */
    handle->dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(handle->hComm, &handle->dcb)) {
        CloseHandle(handle->hComm);
        return -1;
    }

    /* 设置波特率 */
    handle->dcb.BaudRate = baudrate;

    /* 设置数据位 */
    handle->dcb.ByteSize = databits;

    /* 设置停止位 */
    switch (stopbits) {
        case 1: handle->dcb.StopBits = ONESTOPBIT; break;
        case 2: handle->dcb.StopBits = TWOSTOPBITS; break;
        default: handle->dcb.StopBits = ONESTOPBIT; break;
    }

    /* 设置校验 */
    switch (parity) {
        case 'N':
            handle->dcb.Parity = NOPARITY;
            handle->dcb.fParity = FALSE;
            break;
        case 'O':
            handle->dcb.Parity = ODDPARITY;
            handle->dcb.fParity = TRUE;
            break;
        case 'E':
            handle->dcb.Parity = EVENPARITY;
            handle->dcb.fParity = TRUE;
            break;
        default:
            handle->dcb.Parity = NOPARITY;
            handle->dcb.fParity = FALSE;
            break;
    }

    /* 流控制 */
    handle->dcb.fOutxCtsFlow = FALSE;
    handle->dcb.fOutxDsrFlow = FALSE;
    handle->dcb.fDtrControl = DTR_CONTROL_DISABLE;
    handle->dcb.fRtsControl = RTS_CONTROL_DISABLE;

    /* 应用配置 */
    if (!SetCommState(handle->hComm, &handle->dcb)) {
        CloseHandle(handle->hComm);
        return -1;
    }

    /* 设置超时 */
    handle->timeouts.ReadIntervalTimeout = 50;
    handle->timeouts.ReadTotalTimeoutConstant = 1000;
    handle->timeouts.ReadTotalTimeoutMultiplier = 10;
    handle->timeouts.WriteTotalTimeoutConstant = 1000;
    handle->timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(handle->hComm, &handle->timeouts);

    /* 清空缓冲区 */
    PurgeComm(handle->hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

    handle->baudrate = baudrate;
    handle->tx_bytes = 0;
    handle->rx_bytes = 0;

    return 0;
}

/**
 * @brief Windows串口发送数据
 */
int win_serial_send(win_serial_handle_t *handle, const uint8_t *data, size_t len)
{
    DWORD written = 0;

    if (!WriteFile(handle->hComm, data, (DWORD)len, &written, NULL)) {
        return -1;
    }

    handle->tx_bytes += written;
    return (int)written;
}

/**
 * @brief Windows串口接收数据
 */
int win_serial_receive(win_serial_handle_t *handle, uint8_t *buffer,
                       size_t max_len, uint32_t timeout_ms)
{
    DWORD read = 0;
    COMMTIMEOUTS timeouts;

    /* 临时修改超时 */
    GetCommTimeouts(handle->hComm, &timeouts);
    timeouts.ReadTotalTimeoutConstant = timeout_ms;
    SetCommTimeouts(handle->hComm, &timeouts);

    if (!ReadFile(handle->hComm, buffer, (DWORD)max_len, &read, NULL)) {
        return -1;
    }

    handle->rx_bytes += read;
    return (int)read;
}

/**
 * @brief 关闭Windows串口
 */
void win_serial_close(win_serial_handle_t *handle)
{
    if (handle->hComm != INVALID_HANDLE_VALUE) {
        CloseHandle(handle->hComm);
        handle->hComm = INVALID_HANDLE_VALUE;
    }
}
```

---

## 3. Modbus RTU协议完整实现

### 3.1 Modbus协议概述

Modbus是1979年由Modicon(现为施耐德电气)开发的串行通信协议，是工业领域最广泛使用的协议之一。

```
┌─────────────────────────────────────────────────────────────┐
│                    Modbus RTU帧格式                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  +--------+--------+--------+--------+--------+             │
│  | 地址   | 功能码 | 数据   | CRC低  | CRC高  |             │
│  | 1字节  | 1字节  | N字节  | 1字节  | 1字节  |             │
│  +--------+--------+--------+--------+--------+             │
│                                                             │
│  帧间间隔: ≥3.5字符时间(作为帧分隔)                         │
│  CRC校验: CRC-16                                            │
└─────────────────────────────────────────────────────────────┘
```

**Modbus功能码：**

| 功能码 | 名称 | 描述 |
|--------|------|------|
| 0x01 | 读线圈 | 读取离散输出状态 |
| 0x02 | 读离散输入 | 读取离散输入状态 |
| 0x03 | 读保持寄存器 | 读取多个保持寄存器 |
| 0x04 | 读输入寄存器 | 读取多个输入寄存器 |
| 0x05 | 写单个线圈 | 强制单线圈ON/OFF |
| 0x06 | 写单个寄存器 | 预置单寄存器 |
| 0x0F | 写多个线圈 | 强制多线圈 |
| 0x10 | 写多个寄存器 | 预置多寄存器 |
| 0x17 | 读/写多寄存器 | 同时读写寄存器 |

### 3.2 Modbus RTU完整C实现

```c
/**
 * @file modbus_rtu.h
 * @brief Modbus RTU协议栈头文件
 * @version 2.0.0
 * @date 2024
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus常量定义 */
#define MODBUS_BROADCAST_ADDRESS    0
#define MODBUS_MIN_SLAVE_ADDRESS    1
#define MODBUS_MAX_SLAVE_ADDRESS    247

#define MODBUS_RTU_MAX_ADU_LENGTH   256
#define MODBUS_RTU_MAX_PDU_LENGTH   253

/* 功能码定义 */
typedef enum {
    MODBUS_FC_READ_COILS                = 0x01,
    MODBUS_FC_READ_DISCRETE_INPUTS      = 0x02,
    MODBUS_FC_READ_HOLDING_REGISTERS    = 0x03,
    MODBUS_FC_READ_INPUT_REGISTERS      = 0x04,
    MODBUS_FC_WRITE_SINGLE_COIL         = 0x05,
    MODBUS_FC_WRITE_SINGLE_REGISTER     = 0x06,
    MODBUS_FC_READ_EXCEPTION_STATUS     = 0x07,
    MODBUS_FC_DIAGNOSTICS               = 0x08,
    MODBUS_FC_GET_COMM_EVENT_COUNTER    = 0x0B,
    MODBUS_FC_GET_COMM_EVENT_LOG        = 0x0C,
    MODBUS_FC_WRITE_MULTIPLE_COILS      = 0x0F,
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS  = 0x10,
    MODBUS_FC_REPORT_SLAVE_ID           = 0x11,
    MODBUS_FC_READ_FILE_RECORD          = 0x14,
    MODBUS_FC_WRITE_FILE_RECORD         = 0x15,
    MODBUS_FC_MASK_WRITE_REGISTER       = 0x16,
    MODBUS_FC_READ_WRITE_MULTIPLE_REGS  = 0x17,
    MODBUS_FC_READ_FIFO_QUEUE           = 0x18,
    MODBUS_FC_READ_DEVICE_IDENTIFICATION= 0x2B
} modbus_function_code_t;

/* 异常码定义 */
typedef enum {
    MODBUS_EX_NONE                      = 0x00,
    MODBUS_EX_ILLEGAL_FUNCTION          = 0x01,
    MODBUS_EX_ILLEGAL_DATA_ADDRESS      = 0x02,
    MODBUS_EX_ILLEGAL_DATA_VALUE        = 0x03,
    MODBUS_EX_SLAVE_DEVICE_FAILURE      = 0x04,
    MODBUS_EX_ACKNOWLEDGE               = 0x05,
    MODBUS_EX_SLAVE_BUSY                = 0x06,
    MODBUS_EX_MEMORY_PARITY_ERROR       = 0x08,
    MODBUS_EX_GATEWAY_PATH_UNAVAILABLE  = 0x0A,
    MODBUS_EX_GATEWAY_TARGET_FAILED     = 0x0B
} modbus_exception_code_t;

/* 错误码定义 */
typedef enum {
    MODBUS_OK                   = 0,
    MODBUS_ERR_INVALID_PARAM    = -1,
    MODBUS_ERR_INVALID_SLAVE    = -2,
    MODBUS_ERR_INVALID_FUNCTION = -3,
    MODBUS_ERR_INVALID_ADDRESS  = -4,
    MODBUS_ERR_INVALID_COUNT    = -5,
    MODBUS_ERR_CRC              = -6,
    MODBUS_ERR_TIMEOUT          = -7,
    MODBUS_ERR_SEND             = -8,
    MODBUS_ERR_RECV             = -9,
    MODBUS_ERR_EXCEPTION        = -10,
    MODBUS_ERR_BUFSIZE          = -11,
    MODBUS_ERR_NOT_CONNECTED    = -12
} modbus_error_t;

/* 串口后端结构 */
typedef struct modbus_backend modbus_backend_t;

/* Modbus上下文结构 */
typedef struct {
    uint8_t slave;                      /* 从机地址 */
    uint32_t response_timeout_ms;       /* 响应超时(ms) */
    uint32_t byte_timeout_ms;           /* 字节间超时(ms) */
    uint32_t frame_delay_us;            /* 帧间延迟(us) */

    /* 后端接口 */
    const modbus_backend_t *backend;
    void *backend_data;

    /* 调试标志 */
    bool debug;

    /* 统计 */
    uint64_t msgs_sent;
    uint64_t msgs_received;
    uint64_t msgs_failed;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} modbus_context_t;

/* 数据映射回调函数类型 */
typedef int (*modbus_read_bit_cb)(void *ctx, uint16_t addr, bool *value);
typedef int (*modbus_write_bit_cb)(void *ctx, uint16_t addr, bool value);
typedef int (*modbus_read_reg_cb)(void *ctx, uint16_t addr, uint16_t *value);
typedef int (*modbus_write_reg_cb)(void *ctx, uint16_t addr, uint16_t value);

/* 从机数据映射 */
typedef struct {
    /* 线圈 (0x01, 0x05, 0x0F) */
    uint16_t coil_start;
    uint16_t coil_count;
    modbus_read_bit_cb read_coil;
    modbus_write_bit_cb write_coil;

    /* 离散输入 (0x02) */
    uint16_t discrete_start;
    uint16_t discrete_count;
    modbus_read_bit_cb read_discrete;

    /* 保持寄存器 (0x03, 0x06, 0x10) */
    uint16_t holding_start;
    uint16_t holding_count;
    modbus_read_reg_cb read_holding;
    modbus_write_reg_cb write_holding;

    /* 输入寄存器 (0x04) */
    uint16_t input_start;
    uint16_t input_count;
    modbus_read_reg_cb read_input;

    /* 用户上下文 */
    void *user_ctx;
} modbus_slave_map_t;

/* 后端接口 */
struct modbus_backend {
    const char *name;
    size_t header_length;
    size_t checksum_length;
    size_t max_adu_length;

    int (*init)(void **backend_data, const char *device, int baudrate);
    void (*close)(void *backend_data);
    int (*send)(void *backend_data, const uint8_t *data, size_t len);
    int (*recv)(void *backend_data, uint8_t *buffer, size_t max_len,
                uint32_t timeout_ms, uint32_t byte_timeout_ms);
    int (*flush)(void *backend_data);
    void (*pre_send)(void *backend_data);
    void (*post_send)(void *backend_data);
    uint16_t (*checksum)(const uint8_t *data, size_t len);
    int (*check_integrity)(modbus_context_t *ctx, uint8_t *msg, size_t msg_len);
};

/* 函数声明 - 上下文管理 */
modbus_context_t* modbus_new_rtu(const char *device, int baudrate,
                                  char parity, int data_bits, int stop_bits);
void modbus_free(modbus_context_t *ctx);
int modbus_set_slave(modbus_context_t *ctx, uint8_t slave);
int modbus_connect(modbus_context_t *ctx);
void modbus_close(modbus_context_t *ctx);
int modbus_flush(modbus_context_t *ctx);
void modbus_set_debug(modbus_context_t *ctx, bool debug);

/* 函数声明 - 超时设置 */
void modbus_set_response_timeout(modbus_context_t *ctx, uint32_t timeout_ms);
void modbus_set_byte_timeout(modbus_context_t *ctx, uint32_t timeout_ms);
void modbus_get_response_timeout(modbus_context_t *ctx, uint32_t *timeout_ms);
void modbus_get_byte_timeout(modbus_context_t *ctx, uint32_t *timeout_ms);

/* 函数声明 - 主站功能 */
int modbus_read_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                     uint8_t *dest);
int modbus_read_input_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           uint8_t *dest);
int modbus_read_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                          uint16_t *dest);
int modbus_read_input_registers(modbus_context_t *ctx, uint16_t addr,
                                uint16_t nb, uint16_t *dest);
int modbus_write_bit(modbus_context_t *ctx, uint16_t addr, bool status);
int modbus_write_register(modbus_context_t *ctx, uint16_t addr,
                          uint16_t value);
int modbus_write_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                      const uint8_t *data);
int modbus_write_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           const uint16_t *data);
int modbus_mask_write_register(modbus_context_t *ctx, uint16_t addr,
                               uint16_t and_mask, uint16_t or_mask);
int modbus_write_and_read_registers(modbus_context_t *ctx,
                                    uint16_t write_addr, uint16_t write_nb,
                                    const uint16_t *write_data,
                                    uint16_t read_addr, uint16_t read_nb,
                                    uint16_t *read_dest);

/* 函数声明 - 从站功能 */
int modbus_slave_poll(modbus_context_t *ctx, const modbus_slave_map_t *map,
                      uint8_t *req_buffer, size_t req_buffer_len);
int modbus_reply_exception(modbus_context_t *ctx, const uint8_t *req,
                           modbus_exception_code_t exception);

/* 函数声明 - 工具函数 */
uint16_t modbus_crc16(const uint8_t *data, size_t len);
const char* modbus_strerror(int errnum);
const char* modbus_exception_str(modbus_exception_code_t code);
void modbus_get_stats(modbus_context_t *ctx, uint64_t *sent, uint64_t *received,
                      uint64_t *failed, uint64_t *bytes_sent,
                      uint64_t *bytes_received);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_RTU_H */


```c
/**
 * @file modbus_rtu.c
 * @brief Modbus RTU协议栈完整实现
 * @version 2.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include "modbus_rtu.h"
#include "serial_port.h"

/* RTU常量 */
#define MODBUS_RTU_CHECKSUM_LEN     2
#define MODBUS_RTU_HEADER_LEN       0
#define MODBUS_RTU_PRESET_REQ       0xFFFF
#define MODBUS_RTU_PRESET_RSP       0x0000

/* CRC表 (预计算以提高性能) */
static const uint16_t modbus_crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/**
 * @brief 计算CRC16校验值
 */
uint16_t modbus_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = MODBUS_RTU_PRESET_REQ;
    size_t i;

    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ modbus_crc16_table[(crc ^ data[i]) & 0xFF];
    }

    return crc;
}

/* RTU后端数据结构 */
typedef struct {
    serial_handle_t serial;
    serial_config_t config;
    int baudrate;
} rtu_backend_data_t;

/**
 * @brief RTU后端初始化
 */
static int rtu_backend_init(void **backend_data, const char *device, int baudrate)
{
    rtu_backend_data_t *data;
    int ret;

    data = (rtu_backend_data_t *)malloc(sizeof(rtu_backend_data_t));
    if (!data) {
        return -1;
    }

    memset(data, 0, sizeof(*data));
    data->baudrate = baudrate;

    /* 配置串口参数 */
    strncpy(data->config.device, device, sizeof(data->config.device) - 1);
    data->config.baudrate = baudrate;
    data->config.databits = 8;
    data->config.stopbits = 1;
    data->config.parity = 'N';
    data->config.timeout_ms = 500;
    data->config.rs485_mode = false;

    ret = serial_init(&data->serial, &data->config);
    if (ret < 0) {
        free(data);
        return -1;
    }

    *backend_data = data;
    return 0;
}

/**
 * @brief RTU后端关闭
 */
static void rtu_backend_close(void *backend_data)
{
    rtu_backend_data_t *data = (rtu_backend_data_t *)backend_data;

    if (data) {
        serial_close(&data->serial);
        free(data);
    }
}

/**
 * @brief RTU后端发送数据
 */
static int rtu_backend_send(void *backend_data, const uint8_t *data, size_t len)
{
    rtu_backend_data_t *rtu_data = (rtu_backend_data_t *)backend_data;

    return serial_send(&rtu_data->serial, data, len);
}

/**
 * @brief RTU后端接收数据
 */
static int rtu_backend_recv(void *backend_data, uint8_t *buffer, size_t max_len,
                            uint32_t timeout_ms, uint32_t byte_timeout_ms)
{
    rtu_backend_data_t *rtu_data = (rtu_backend_data_t *)backend_data;

    return serial_receive(&rtu_data->serial, buffer, max_len, timeout_ms);
}

/**
 * @brief RTU后端清空缓冲区
 */
static int rtu_backend_flush(void *backend_data)
{
    rtu_backend_data_t *rtu_data = (rtu_backend_data_t *)backend_data;

    return serial_flush(&rtu_data->serial);
}

/**
 * @brief RTU后端帧延迟计算
 * RTU标准要求帧间间隔至少3.5个字符时间
 */
static void compute_frame_delay(rtu_backend_data_t *data, uint32_t *delay_us)
{
    /* 1字符 = 11位(1起始+8数据+无校验+1停止) 或 12位(有校验) */
    int bits_per_char = 11;

    /* 3.5字符时间 (微秒) */
    *delay_us = (3500000 * bits_per_char) / data->baudrate;

    /* 最小值保证 */
    if (*delay_us < 1750) {
        *delay_us = 1750;
    }
}

/**
 * @brief RTU后端发送前延时
 */
static void rtu_backend_pre_send(void *backend_data)
{
    rtu_backend_data_t *data = (rtu_backend_data_t *)backend_data;
    uint32_t delay_us;

    compute_frame_delay(data, &delay_us);
    usleep(delay_us);
}

/**
 * @brief RTU后端发送后延时
 */
static void rtu_backend_post_send(void *backend_data)
{
    rtu_backend_data_t *data = (rtu_backend_data_t *)backend_data;
    uint32_t delay_us;

    compute_frame_delay(data, &delay_us);
    usleep(delay_us);
}

/**
 * @brief RTU后端校验
 */
static uint16_t rtu_backend_checksum(const uint8_t *data, size_t len)
{
    return modbus_crc16(data, len);
}

/**
 * @brief RTU后端完整性检查
 */
static int rtu_backend_check_integrity(modbus_context_t *ctx, uint8_t *msg,
                                       size_t msg_len)
{
    uint16_t crc_calculated;
    uint16_t crc_received;

    if (msg_len < MODBUS_RTU_CHECKSUM_LEN + 1) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 计算CRC */
    crc_calculated = modbus_crc16(msg, msg_len - MODBUS_RTU_CHECKSUM_LEN);

    /* 获取接收到的CRC (小端序) */
    crc_received = (msg[msg_len - 1] << 8) | msg[msg_len - 2];

    if (crc_calculated != crc_received) {
        if (ctx->debug) {
            fprintf(stderr, "CRC错误: 计算值=0x%04X, 接收值=0x%04X\n",
                    crc_calculated, crc_received);
        }
        return MODBUS_ERR_CRC;
    }

    return MODBUS_OK;
}

/* RTU后端定义 */
static const modbus_backend_t modbus_rtu_backend = {
    "RTU",
    MODBUS_RTU_HEADER_LEN,
    MODBUS_RTU_CHECKSUM_LEN,
    MODBUS_RTU_MAX_ADU_LENGTH,
    rtu_backend_init,
    rtu_backend_close,
    rtu_backend_send,
    rtu_backend_recv,
    rtu_backend_flush,
    rtu_backend_pre_send,
    rtu_backend_post_send,
    rtu_backend_checksum,
    rtu_backend_check_integrity
};

/**
 * @brief 创建新的RTU上下文
 */
modbus_context_t* modbus_new_rtu(const char *device, int baudrate,
                                  char parity, int data_bits, int stop_bits)
{
    modbus_context_t *ctx;
    rtu_backend_data_t *rtu_data;
    int ret;

    if (!device || baudrate <= 0) {
        return NULL;
    }

    ctx = (modbus_context_t *)malloc(sizeof(modbus_context_t));
    if (!ctx) {
        return NULL;
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->backend = &modbus_rtu_backend;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 500;
    ctx->slave = 1;

    /* 初始化后端 */
    ret = rtu_backend_init((void **)&rtu_data, device, baudrate);
    if (ret < 0) {
        free(ctx);
        return NULL;
    }

    /* 设置串口参数 */
    rtu_data->config.parity = parity;
    rtu_data->config.databits = data_bits;
    rtu_data->config.stopbits = stop_bits;

    /* 重新初始化以应用新参数 */
    serial_close(&rtu_data->serial);
    serial_init(&rtu_data->serial, &rtu_data->config);

    ctx->backend_data = rtu_data;

    /* 计算帧延迟 */
    uint32_t delay_us;
    compute_frame_delay(rtu_data, &delay_us);
    ctx->frame_delay_us = delay_us;

    return ctx;
}

/**
 * @brief 释放Modbus上下文
 */
void modbus_free(modbus_context_t *ctx)
{
    if (ctx) {
        if (ctx->backend && ctx->backend->close) {
            ctx->backend->close(ctx->backend_data);
        }
        free(ctx);
    }
}

/**
 * @brief 设置从机地址
 */
int modbus_set_slave(modbus_context_t *ctx, uint8_t slave)
{
    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    if (slave < MODBUS_MIN_SLAVE_ADDRESS || slave > MODBUS_MAX_SLAVE_ADDRESS) {
        return MODBUS_ERR_INVALID_SLAVE;
    }

    ctx->slave = slave;
    return MODBUS_OK;
}

/**
 * @brief 连接(对于RTU,在new_rtu时已连接)
 */
int modbus_connect(modbus_context_t *ctx)
{
    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    return MODBUS_OK;
}

/**
 * @brief 关闭连接
 */
void modbus_close(modbus_context_t *ctx)
{
    if (ctx && ctx->backend && ctx->backend->close) {
        ctx->backend->close(ctx->backend_data);
        ctx->backend_data = NULL;
    }
}

/**
 * @brief 清空缓冲区
 */
int modbus_flush(modbus_context_t *ctx)
{
    if (!ctx || !ctx->backend) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    return ctx->backend->flush(ctx->backend_data);
}

/**
 * @brief 设置调试模式
 */
void modbus_set_debug(modbus_context_t *ctx, bool debug)
{
    if (ctx) {
        ctx->debug = debug;
    }
}

/**
 * @brief 设置响应超时
 */
void modbus_set_response_timeout(modbus_context_t *ctx, uint32_t timeout_ms)
{
    if (ctx) {
        ctx->response_timeout_ms = timeout_ms;
    }
}

/**
 * @brief 设置字节超时
 */
void modbus_set_byte_timeout(modbus_context_t *ctx, uint32_t timeout_ms)
{
    if (ctx) {
        ctx->byte_timeout_ms = timeout_ms;
    }
}

/**
 * @brief 构建并发送请求
 */
static int send_msg(modbus_context_t *ctx, uint8_t *msg, size_t msg_len)
{
    uint16_t crc;
    int ret;

    /* 添加CRC */
    crc = modbus_crc16(msg, msg_len);
    msg[msg_len++] = crc & 0xFF;
    msg[msg_len++] = (crc >> 8) & 0xFF;

    /* 发送前延时 */
    if (ctx->backend->pre_send) {
        ctx->backend->pre_send(ctx->backend_data);
    }

    if (ctx->debug) {
        printf("发送: ");
        for (size_t i = 0; i < msg_len; i++) {
            printf("%02X ", msg[i]);
        }
        printf("\n");
    }

    ret = ctx->backend->send(ctx->backend_data, msg, msg_len);
    if (ret < 0) {
        ctx->msgs_failed++;
        return MODBUS_ERR_SEND;
    }

    ctx->msgs_sent++;
    ctx->bytes_sent += msg_len;

    /* 发送后延时 */
    if (ctx->backend->post_send) {
        ctx->backend->post_send(ctx->backend_data);
    }

    return MODBUS_OK;
}

/**
 * @brief 接收并验证响应
 */
static int receive_msg(modbus_context_t *ctx, uint8_t *msg, size_t *msg_len)
{
    int ret;
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];

    ret = ctx->backend->recv(ctx->backend_data, rsp, sizeof(rsp),
                             ctx->response_timeout_ms, ctx->byte_timeout_ms);

    if (ret < 0) {
        return MODBUS_ERR_TIMEOUT;
    }

    *msg_len = ret;

    if (ctx->debug) {
        printf("接收: ");
        for (size_t i = 0; i < *msg_len; i++) {
            printf("%02X ", rsp[i]);
        }
        printf("\n");
    }

    /* 检查完整性 */
    ret = ctx->backend->check_integrity(ctx, rsp, *msg_len);
    if (ret < 0) {
        return ret;
    }

    /* 复制到输出缓冲区(去掉CRC) */
    memcpy(msg, rsp, *msg_len - 2);
    *msg_len -= 2;

    ctx->msgs_received++;
    ctx->bytes_received += *msg_len + 2;

    return MODBUS_OK;
}

/**
 * @brief 读线圈 (功能码0x01)
 */
int modbus_read_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                     uint8_t *dest)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 2000) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 构建请求 */
    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_READ_COILS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    /* 验证响应 */
    if (rsp_len < 3 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_READ_COILS) {
        /* 可能是异常响应 */
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 复制数据 */
    uint8_t byte_count = rsp[2];
    if (byte_count != (nb + 7) / 8) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    memcpy(dest, &rsp[3], byte_count);

    return byte_count;
}

/**
 * @brief 读离散输入 (功能码0x02)
 */
int modbus_read_input_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           uint8_t *dest)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 2000) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_READ_DISCRETE_INPUTS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 3 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_READ_DISCRETE_INPUTS) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t byte_count = rsp[2];
    memcpy(dest, &rsp[3], byte_count);

    return byte_count;
}

/**
 * @brief 读保持寄存器 (功能码0x03)
 */
int modbus_read_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                          uint16_t *dest)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 125) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_READ_HOLDING_REGISTERS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 3 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_READ_HOLDING_REGISTERS) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t byte_count = rsp[2];
    if (byte_count != nb * 2) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 大端序转主机字节序 */
    for (int i = 0; i < nb; i++) {
        dest[i] = (rsp[3 + i * 2] << 8) | rsp[4 + i * 2];
    }

    return nb;
}

/**
 * @brief 读输入寄存器 (功能码0x04)
 */
int modbus_read_input_registers(modbus_context_t *ctx, uint16_t addr,
                                uint16_t nb, uint16_t *dest)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 125) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_READ_INPUT_REGISTERS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 3 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_READ_INPUT_REGISTERS) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t byte_count = rsp[2];
    for (int i = 0; i < nb; i++) {
        dest[i] = (rsp[3 + i * 2] << 8) | rsp[4 + i * 2];
    }

    return nb;
}

/**
 * @brief 写单个线圈 (功能码0x05)
 */
int modbus_write_bit(modbus_context_t *ctx, uint16_t addr, bool status)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint16_t value = status ? 0xFF00 : 0x0000;

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_WRITE_SINGLE_COIL;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (value >> 8) & 0xFF;
    req[req_len++] = value & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 6 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_WRITE_SINGLE_COIL) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    return 1;
}

/**
 * @brief 写单个寄存器 (功能码0x06)
 */
int modbus_write_register(modbus_context_t *ctx, uint16_t addr, uint16_t value)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_WRITE_SINGLE_REGISTER;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (value >> 8) & 0xFF;
    req[req_len++] = value & 0xFF;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 6 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_WRITE_SINGLE_REGISTER) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    return 1;
}

/**
 * @brief 写多个线圈 (功能码0x0F)
 */
int modbus_write_bits(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                      const uint8_t *data)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;
    int byte_count;

    if (!ctx || !data || nb < 1 || nb > 0x07B0) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    byte_count = (nb + 7) / 8;

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_WRITE_MULTIPLE_COILS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;
    req[req_len++] = byte_count;

    memcpy(&req[req_len], data, byte_count);
    req_len += byte_count;

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 6 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_WRITE_MULTIPLE_COILS) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    return nb;
}

/**
 * @brief 写多个寄存器 (功能码0x10)
 */
int modbus_write_registers(modbus_context_t *ctx, uint16_t addr, uint16_t nb,
                           const uint16_t *data)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    size_t rsp_len = 0;
    int ret;

    if (!ctx || !data || nb < 1 || nb > 0x007B) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    req[req_len++] = ctx->slave;
    req[req_len++] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;
    req[req_len++] = nb * 2;

    /* 主机字节序转大端序 */
    for (int i = 0; i < nb; i++) {
        req[req_len++] = (data[i] >> 8) & 0xFF;
        req[req_len++] = data[i] & 0xFF;
    }

    ret = send_msg(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    ret = receive_msg(ctx, rsp, &rsp_len);
    if (ret < 0) {
        return ret;
    }

    if (rsp_len < 6 || rsp[0] != ctx->slave ||
        rsp[1] != MODBUS_FC_WRITE_MULTIPLE_REGISTERS) {
        if (rsp_len >= 2 && (rsp[1] & 0x80)) {
            return MODBUS_ERR_EXCEPTION - rsp[2];
        }
        return MODBUS_ERR_INVALID_PARAM;
    }

    return nb;
}

/**
 * @brief 错误码转字符串
 */
const char* modbus_strerror(int errnum)
{
    switch (errnum) {
        case MODBUS_OK:                   return "Success";
        case MODBUS_ERR_INVALID_PARAM:    return "Invalid parameter";
        case MODBUS_ERR_INVALID_SLAVE:    return "Invalid slave address";
        case MODBUS_ERR_INVALID_FUNCTION: return "Invalid function code";
        case MODBUS_ERR_INVALID_ADDRESS:  return "Invalid data address";
        case MODBUS_ERR_INVALID_COUNT:    return "Invalid data count";
        case MODBUS_ERR_CRC:              return "CRC error";
        case MODBUS_ERR_TIMEOUT:          return "Timeout";
        case MODBUS_ERR_SEND:             return "Send error";
        case MODBUS_ERR_RECV:             return "Receive error";
        case MODBUS_ERR_EXCEPTION:        return "Modbus exception";
        case MODBUS_ERR_BUFSIZE:          return "Buffer size error";
        case MODBUS_ERR_NOT_CONNECTED:    return "Not connected";
        default:                          return "Unknown error";
    }
}

/**
 * @brief 异常码转字符串
 */
const char* modbus_exception_str(modbus_exception_code_t code)
{
    switch (code) {
        case MODBUS_EX_NONE:                      return "No exception";
        case MODBUS_EX_ILLEGAL_FUNCTION:          return "Illegal function";
        case MODBUS_EX_ILLEGAL_DATA_ADDRESS:      return "Illegal data address";
        case MODBUS_EX_ILLEGAL_DATA_VALUE:        return "Illegal data value";
        case MODBUS_EX_SLAVE_DEVICE_FAILURE:      return "Slave device failure";
        case MODBUS_EX_ACKNOWLEDGE:               return "Acknowledge";
        case MODBUS_EX_SLAVE_BUSY:                return "Slave device busy";
        case MODBUS_EX_MEMORY_PARITY_ERROR:       return "Memory parity error";
        case MODBUS_EX_GATEWAY_PATH_UNAVAILABLE:  return "Gateway path unavailable";
        case MODBUS_EX_GATEWAY_TARGET_FAILED:     return "Gateway target device failed";
        default:                                  return "Unknown exception";
    }
}

/**
 * @brief 获取统计信息
 */
void modbus_get_stats(modbus_context_t *ctx, uint64_t *sent, uint64_t *received,
                      uint64_t *failed, uint64_t *bytes_sent,
                      uint64_t *bytes_received)
{
    if (!ctx) return;

    if (sent) *sent = ctx->msgs_sent;
    if (received) *received = ctx->msgs_received;
    if (failed) *failed = ctx->msgs_failed;
    if (bytes_sent) *bytes_sent = ctx->bytes_sent;
    if (bytes_received) *bytes_received = ctx->bytes_received;
}
```

### 3.3 Modbus从机实现

```c
/**
 * @file modbus_slave.c
 * @brief Modbus从机实现
 */

#include "modbus_rtu.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief 构建异常响应
 */
static int build_exception_response(uint8_t *rsp, uint8_t slave,
                                     uint8_t function,
                                     modbus_exception_code_t exception)
{
    rsp[0] = slave;
    rsp[1] = function | 0x80;
    rsp[2] = exception;
    return 3;
}

/**
 * @brief 处理读线圈请求
 */
static int handle_read_coils(const modbus_slave_map_t *map, const uint8_t *req,
                              uint8_t *rsp, uint16_t addr, uint16_t nb)
{
    int byte_count = (nb + 7) / 8;
    int rsp_len = 0;

    rsp[rsp_len++] = req[0];  /* 从机地址 */
    rsp[rsp_len++] = MODBUS_FC_READ_COILS;
    rsp[rsp_len++] = byte_count;

    memset(&rsp[rsp_len], 0, byte_count);

    for (int i = 0; i < nb; i++) {
        uint16_t coil_addr = addr + i;
        bool value = false;

        if (map->read_coil) {
            int ret = map->read_coil(map->user_ctx, coil_addr, &value);
            if (ret < 0) {
                return build_exception_response(rsp, req[0], MODBUS_FC_READ_COILS,
                                                MODBUS_EX_SLAVE_DEVICE_FAILURE);
            }
        }

        if (value) {
            rsp[rsp_len + i / 8] |= (1 << (i % 8));
        }
    }

    rsp_len += byte_count;
    return rsp_len;
}

/**
 * @brief 处理读保持寄存器请求
 */
static int handle_read_holding(const modbus_slave_map_t *map, const uint8_t *req,
                                uint8_t *rsp, uint16_t addr, uint16_t nb)
{
    int rsp_len = 0;

    rsp[rsp_len++] = req[0];
    rsp[rsp_len++] = MODBUS_FC_READ_HOLDING_REGISTERS;
    rsp[rsp_len++] = nb * 2;

    for (int i = 0; i < nb; i++) {
        uint16_t reg_addr = addr + i;
        uint16_t value = 0;

        if (map->read_holding) {
            int ret = map->read_holding(map->user_ctx, reg_addr, &value);
            if (ret < 0) {
                return build_exception_response(rsp, req[0],
                                                MODBUS_FC_READ_HOLDING_REGISTERS,
                                                MODBUS_EX_SLAVE_DEVICE_FAILURE);
            }
        }

        rsp[rsp_len++] = (value >> 8) & 0xFF;
        rsp[rsp_len++] = value & 0xFF;
    }

    return rsp_len;
}

/**
 * @brief 处理写单个线圈请求
 */
static int handle_write_coil(const modbus_slave_map_t *map, const uint8_t *req,
                              uint8_t *rsp, uint16_t addr, uint16_t value)
{
    if (!map->write_coil) {
        return build_exception_response(rsp, req[0], MODBUS_FC_WRITE_SINGLE_COIL,
                                        MODBUS_EX_ILLEGAL_FUNCTION);
    }

    bool coil_value = (value == 0xFF00);

    int ret = map->write_coil(map->user_ctx, addr, coil_value);
    if (ret < 0) {
        return build_exception_response(rsp, req[0], MODBUS_FC_WRITE_SINGLE_COIL,
                                        MODBUS_EX_SLAVE_DEVICE_FAILURE);
    }

    /* 回显请求 */
    memcpy(rsp, req, 6);
    return 6;
}

/**
 * @brief 处理写单个寄存器请求
 */
static int handle_write_register(const modbus_slave_map_t *map, const uint8_t *req,
                                  uint8_t *rsp, uint16_t addr, uint16_t value)
{
    if (!map->write_holding) {
        return build_exception_response(rsp, req[0],
                                        MODBUS_FC_WRITE_SINGLE_REGISTER,
                                        MODBUS_EX_ILLEGAL_FUNCTION);
    }

    int ret = map->write_holding(map->user_ctx, addr, value);
    if (ret < 0) {
        return build_exception_response(rsp, req[0],
                                        MODBUS_FC_WRITE_SINGLE_REGISTER,
                                        MODBUS_EX_SLAVE_DEVICE_FAILURE);
    }

    memcpy(rsp, req, 6);
    return 6;
}

/**
 * @brief Modbus从机轮询处理
 */
int modbus_slave_poll(modbus_context_t *ctx, const modbus_slave_map_t *map,
                      uint8_t *req_buffer, size_t req_buffer_len)
{
    uint8_t req[MODBUS_RTU_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_RTU_MAX_ADU_LENGTH];
    size_t req_len = 0;
    int ret;

    if (!ctx || !map || !req_buffer) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 接收请求 */
    ret = ctx->backend->recv(ctx->backend_data, req, sizeof(req),
                             ctx->response_timeout_ms, ctx->byte_timeout_ms);

    if (ret < 0) {
        return MODBUS_ERR_TIMEOUT;
    }

    req_len = ret;

    /* 检查CRC */
    ret = ctx->backend->check_integrity(ctx, req, req_len);
    if (ret < 0) {
        return ret;
    }

    req_len -= 2;  /* 去掉CRC */

    /* 检查地址 */
    if (req[0] != ctx->slave && req[0] != MODBUS_BROADCAST_ADDRESS) {
        return MODBUS_OK;  /* 不是发给本机 */
    }

    /* 解析请求 */
    uint8_t function = req[1];
    uint16_t addr = (req[2] << 8) | req[3];
    uint16_t value = (req[4] << 8) | req[5];
    int rsp_len = 0;

    switch (function) {
        case MODBUS_FC_READ_COILS:
            rsp_len = handle_read_coils(map, req, rsp, addr, value);
            break;

        case MODBUS_FC_READ_HOLDING_REGISTERS:
            rsp_len = handle_read_holding(map, req, rsp, addr, value);
            break;

        case MODBUS_FC_WRITE_SINGLE_COIL:
            rsp_len = handle_write_coil(map, req, rsp, addr, value);
            break;

        case MODBUS_FC_WRITE_SINGLE_REGISTER:
            rsp_len = handle_write_register(map, req, rsp, addr, value);
            break;

        default:
            rsp_len = build_exception_response(rsp, req[0], function,
                                                MODBUS_EX_ILLEGAL_FUNCTION);
            break;
    }

    /* 发送响应(广播不响应) */
    if (req[0] != MODBUS_BROADCAST_ADDRESS && rsp_len > 0) {
        uint16_t crc = modbus_crc16(rsp, rsp_len);
        rsp[rsp_len++] = crc & 0xFF;
        rsp[rsp_len++] = (crc >> 8) & 0xFF;

        ctx->backend->send(ctx->backend_data, rsp, rsp_len);
    }

    return MODBUS_OK;
}
```


---

## 4. Modbus TCP协议实现

### 4.1 Modbus TCP协议概述

Modbus TCP是Modbus协议的以太网版本，使用TCP/IP作为传输层。

```
┌─────────────────────────────────────────────────────────────┐
│                    Modbus TCP帧结构 (MBAP)                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  +--------+--------+--------+--------+--------+---------+   │
│  | 事务ID | 协议ID | 长度   | 单元ID | 功能码 | 数据    |   │
│  | 2字节  | 2字节  | 2字节  | 1字节  | 1字节  | N字节   |   │
│  +--------+--------+--------+--------+--------+---------+   │
│                                                             │
│  事务ID: 用于匹配请求和响应                                  │
│  协议ID: 固定为0x0000 (Modbus协议)                          │
│  长度:   后续字节数(单元ID+PDU)                             │
│  单元ID: 从机地址(类似RTU的地址,常设为0xFF)                 │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Modbus TCP完整C实现

```c
/**
 * @file modbus_tcp.h
 * @brief Modbus TCP协议栈头文件
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MODBUS_TCP_DEFAULT_PORT     502
#define MODBUS_TCP_MAX_ADU_LENGTH   260

/* TCP后端上下文 */
typedef struct {
    int socket_fd;
    char ip[64];
    int port;
    uint16_t transaction_id;
} modbus_tcp_backend_t;

/* Modbus TCP上下文 */
typedef struct {
    uint8_t slave;
    uint32_t response_timeout_ms;
    modbus_tcp_backend_t *backend;
    bool debug;
    uint64_t msgs_sent;
    uint64_t msgs_received;
} modbus_tcp_context_t;

/* 函数声明 */
modbus_tcp_context_t* modbus_tcp_new(const char *ip, int port);
void modbus_tcp_free(modbus_tcp_context_t *ctx);
int modbus_tcp_connect(modbus_tcp_context_t *ctx);
void modbus_tcp_close(modbus_tcp_context_t *ctx);
int modbus_tcp_set_slave(modbus_tcp_context_t *ctx, uint8_t slave);

/* 主站功能 */
int modbus_tcp_read_bits(modbus_tcp_context_t *ctx, uint16_t addr,
                         uint16_t nb, uint8_t *dest);
int modbus_tcp_read_registers(modbus_tcp_context_t *ctx, uint16_t addr,
                              uint16_t nb, uint16_t *dest);
int modbus_tcp_write_bit(modbus_tcp_context_t *ctx, uint16_t addr, bool status);
int modbus_tcp_write_register(modbus_tcp_context_t *ctx, uint16_t addr,
                               uint16_t value);
int modbus_tcp_write_registers(modbus_tcp_context_t *ctx, uint16_t addr,
                               uint16_t nb, const uint16_t *data);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_TCP_H */
```

```c
/**
 * @file modbus_tcp.c
 * @brief Modbus TCP协议栈实现
 */

#include "modbus_tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>

#define MODBUS_TCP_HEADER_LENGTH    7
#define MODBUS_TCP_PROTOCOL_ID      0x0000

/* 功能码定义 */
#define MODBUS_FC_READ_COILS                0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS      0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS    0x03
#define MODBUS_FC_READ_INPUT_REGISTERS      0x04
#define MODBUS_FC_WRITE_SINGLE_COIL         0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER     0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS      0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS  0x10

/**
 * @brief 创建Modbus TCP上下文
 */
modbus_tcp_context_t* modbus_tcp_new(const char *ip, int port)
{
    modbus_tcp_context_t *ctx;
    modbus_tcp_backend_t *backend;

    if (!ip) {
        return NULL;
    }

    ctx = (modbus_tcp_context_t *)malloc(sizeof(modbus_tcp_context_t));
    if (!ctx) {
        return NULL;
    }

    backend = (modbus_tcp_backend_t *)malloc(sizeof(modbus_tcp_backend_t));
    if (!backend) {
        free(ctx);
        return NULL;
    }

    memset(ctx, 0, sizeof(*ctx));
    memset(backend, 0, sizeof(*backend));

    strncpy(backend->ip, ip, sizeof(backend->ip) - 1);
    backend->port = (port > 0) ? port : MODBUS_TCP_DEFAULT_PORT;
    backend->socket_fd = -1;
    backend->transaction_id = 1;

    ctx->backend = backend;
    ctx->response_timeout_ms = 3000;
    ctx->slave = 0xFF;  /* TCP中常设为0xFF */

    return ctx;
}

/**
 * @brief 释放Modbus TCP上下文
 */
void modbus_tcp_free(modbus_tcp_context_t *ctx)
{
    if (ctx) {
        if (ctx->backend) {
            modbus_tcp_close(ctx);
            free(ctx->backend);
        }
        free(ctx);
    }
}

/**
 * @brief 连接到Modbus TCP服务器
 */
int modbus_tcp_connect(modbus_tcp_context_t *ctx)
{
    struct sockaddr_in addr;
    int flags;
    int ret;

    if (!ctx || !ctx->backend) {
        return -1;
    }

    /* 创建socket */
    ctx->backend->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->backend->socket_fd < 0) {
        perror("socket");
        return -1;
    }

    /* 设置地址 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ctx->backend->port);

    if (inet_pton(AF_INET, ctx->backend->ip, &addr.sin_addr) <= 0) {
        close(ctx->backend->socket_fd);
        ctx->backend->socket_fd = -1;
        return -1;
    }

    /* 连接 */
    ret = connect(ctx->backend->socket_fd, (struct sockaddr *)&addr,
                  sizeof(addr));
    if (ret < 0) {
        perror("connect");
        close(ctx->backend->socket_fd);
        ctx->backend->socket_fd = -1;
        return -1;
    }

    /* 禁用Nagle算法(降低延迟) */
    int flag = 1;
    setsockopt(ctx->backend->socket_fd, IPPROTO_TCP, TCP_NODELAY,
               &flag, sizeof(flag));

    return 0;
}

/**
 * @brief 关闭连接
 */
void modbus_tcp_close(modbus_tcp_context_t *ctx)
{
    if (ctx && ctx->backend && ctx->backend->socket_fd >= 0) {
        close(ctx->backend->socket_fd);
        ctx->backend->socket_fd = -1;
    }
}

/**
 * @brief 设置从机地址
 */
int modbus_tcp_set_slave(modbus_tcp_context_t *ctx, uint8_t slave)
{
    if (!ctx) {
        return -1;
    }
    ctx->slave = slave;
    return 0;
}

/**
 * @brief 构建MBAP头部
 */
static void build_mbap_header(uint8_t *header, uint16_t transaction_id,
                               uint16_t length, uint8_t unit_id)
{
    header[0] = (transaction_id >> 8) & 0xFF;
    header[1] = transaction_id & 0xFF;
    header[2] = 0;  /* 协议ID高字节 */
    header[3] = 0;  /* 协议ID低字节 */
    header[4] = (length >> 8) & 0xFF;
    header[5] = length & 0xFF;
    header[6] = unit_id;
}

/**
 * @brief 发送Modbus TCP请求
 */
static int tcp_send(modbus_tcp_context_t *ctx, uint8_t *req, size_t req_len)
{
    ssize_t sent;
    size_t total = 0;

    while (total < req_len) {
        sent = send(ctx->backend->socket_fd, req + total, req_len - total, 0);
        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        total += sent;
    }

    ctx->msgs_sent++;
    return 0;
}

/**
 * @brief 接收Modbus TCP响应
 */
static int tcp_receive(modbus_tcp_context_t *ctx, uint8_t *rsp, size_t *rsp_len)
{
    fd_set readfds;
    struct timeval tv;
    int ret;
    ssize_t received;
    uint8_t header[MODBUS_TCP_HEADER_LENGTH];
    uint16_t length;

    /* 等待数据 */
    FD_ZERO(&readfds);
    FD_SET(ctx->backend->socket_fd, &readfds);

    tv.tv_sec = ctx->response_timeout_ms / 1000;
    tv.tv_usec = (ctx->response_timeout_ms % 1000) * 1000;

    ret = select(ctx->backend->socket_fd + 1, &readfds, NULL, NULL, &tv);
    if (ret <= 0) {
        return -1;  /* 超时或错误 */
    }

    /* 接收MBAP头部 */
    received = recv(ctx->backend->socket_fd, header, MODBUS_TCP_HEADER_LENGTH,
                    MSG_WAITALL);
    if (received < MODBUS_TCP_HEADER_LENGTH) {
        return -1;
    }

    /* 解析长度 */
    length = (header[4] << 8) | header[5];

    /* 接收PDU */
    received = recv(ctx->backend->socket_fd, rsp, length, MSG_WAITALL);
    if (received < length) {
        return -1;
    }

    *rsp_len = received;

    ctx->msgs_received++;
    return 0;
}

/**
 * @brief 发送请求并接收响应
 */
static int send_and_receive(modbus_tcp_context_t *ctx, uint8_t *req_pdu,
                            size_t pdu_len, uint8_t *rsp_pdu, size_t *rsp_len)
{
    uint8_t req[MODBUS_TCP_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    int ret;

    /* 构建完整请求 */
    build_mbap_header(req, ctx->backend->transaction_id, pdu_len + 1,
                      ctx->slave);
    memcpy(req + MODBUS_TCP_HEADER_LENGTH, req_pdu, pdu_len);

    if (ctx->debug) {
        printf("TCP发送 [事务ID=%d]: ", ctx->backend->transaction_id);
        for (size_t i = 0; i < MODBUS_TCP_HEADER_LENGTH + pdu_len; i++) {
            printf("%02X ", req[i]);
        }
        printf("\n");
    }

    /* 发送 */
    ret = tcp_send(ctx, req, MODBUS_TCP_HEADER_LENGTH + pdu_len);
    if (ret < 0) {
        return -1;
    }

    /* 接收 */
    ret = tcp_receive(ctx, rsp, rsp_len);
    if (ret < 0) {
        return -1;
    }

    if (ctx->debug) {
        printf("TCP接收: ");
        for (size_t i = 0; i < *rsp_len; i++) {
            printf("%02X ", rsp[i]);
        }
        printf("\n");
    }

    /* 验证响应 */
    uint16_t rsp_transaction = (rsp[-MODBUS_TCP_HEADER_LENGTH] << 8) |
                                rsp[-MODBUS_TCP_HEADER_LENGTH + 1];
    if (rsp_transaction != ctx->backend->transaction_id) {
        return -1;  /* 事务ID不匹配 */
    }

    /* 复制PDU到输出 */
    memcpy(rsp_pdu, rsp, *rsp_len);
    *rsp_len = *rsp_len;

    /* 递增事务ID */
    ctx->backend->transaction_id++;
    if (ctx->backend->transaction_id == 0) {
        ctx->backend->transaction_id = 1;
    }

    return 0;
}

/**
 * @brief 读线圈 (功能码0x01)
 */
int modbus_tcp_read_bits(modbus_tcp_context_t *ctx, uint16_t addr,
                         uint16_t nb, uint8_t *dest)
{
    uint8_t req[12];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t rsp_len;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 2000) {
        return -1;
    }

    /* 构建PDU */
    req[0] = MODBUS_FC_READ_COILS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    ret = send_and_receive(ctx, req, 5, rsp, &rsp_len);
    if (ret < 0) {
        return -1;
    }

    /* 验证响应 */
    if (rsp_len < 3 || rsp[0] != MODBUS_FC_READ_COILS) {
        return -1;
    }

    int byte_count = rsp[1];
    memcpy(dest, &rsp[2], byte_count);

    return byte_count;
}

/**
 * @brief 读保持寄存器 (功能码0x03)
 */
int modbus_tcp_read_registers(modbus_tcp_context_t *ctx, uint16_t addr,
                              uint16_t nb, uint16_t *dest)
{
    uint8_t req[12];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t rsp_len;
    int ret;

    if (!ctx || !dest || nb < 1 || nb > 125) {
        return -1;
    }

    req[0] = MODBUS_FC_READ_HOLDING_REGISTERS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    ret = send_and_receive(ctx, req, 5, rsp, &rsp_len);
    if (ret < 0) {
        return -1;
    }

    if (rsp_len < 3 || rsp[0] != MODBUS_FC_READ_HOLDING_REGISTERS) {
        return -1;
    }

    for (int i = 0; i < nb; i++) {
        dest[i] = (rsp[2 + i * 2] << 8) | rsp[3 + i * 2];
    }

    return nb;
}

/**
 * @brief 写单个线圈 (功能码0x05)
 */
int modbus_tcp_write_bit(modbus_tcp_context_t *ctx, uint16_t addr, bool status)
{
    uint8_t req[12];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t rsp_len;
    int ret;

    if (!ctx) {
        return -1;
    }

    uint16_t value = status ? 0xFF00 : 0x0000;

    req[0] = MODBUS_FC_WRITE_SINGLE_COIL;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (value >> 8) & 0xFF;
    req[4] = value & 0xFF;

    ret = send_and_receive(ctx, req, 5, rsp, &rsp_len);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief 写单个寄存器 (功能码0x06)
 */
int modbus_tcp_write_register(modbus_tcp_context_t *ctx, uint16_t addr,
                               uint16_t value)
{
    uint8_t req[12];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t rsp_len;
    int ret;

    if (!ctx) {
        return -1;
    }

    req[0] = MODBUS_FC_WRITE_SINGLE_REGISTER;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (value >> 8) & 0xFF;
    req[4] = value & 0xFF;

    ret = send_and_receive(ctx, req, 5, rsp, &rsp_len);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief 写多个寄存器 (功能码0x10)
 */
int modbus_tcp_write_registers(modbus_tcp_context_t *ctx, uint16_t addr,
                               uint16_t nb, const uint16_t *data)
{
    uint8_t req[MODBUS_TCP_MAX_ADU_LENGTH];
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    size_t rsp_len;
    int ret;
    int req_len = 0;

    if (!ctx || !data || nb < 1 || nb > 123) {
        return -1;
    }

    req[req_len++] = MODBUS_FC_WRITE_MULTIPLE_REGISTERS;
    req[req_len++] = (addr >> 8) & 0xFF;
    req[req_len++] = addr & 0xFF;
    req[req_len++] = (nb >> 8) & 0xFF;
    req[req_len++] = nb & 0xFF;
    req[req_len++] = nb * 2;

    for (int i = 0; i < nb; i++) {
        req[req_len++] = (data[i] >> 8) & 0xFF;
        req[req_len++] = data[i] & 0xFF;
    }

    ret = send_and_receive(ctx, req, req_len, rsp, &rsp_len);
    if (ret < 0) {
        return -1;
    }

    return 0;
}
```

---

## 5. CAN总线通信

### 5.1 CAN总线概述

CAN(Controller Area Network)是Bosch公司于1986年开发的串行通信协议，广泛应用于汽车电子和工业自动化领域。

```
┌─────────────────────────────────────────────────────────────┐
│                    CAN帧格式                                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  标准帧格式 (11位标识符):                                   │
│  +------+------+------+------+------+------+------+         │
│  | SOF  |  ID  | RTR  | 控制 | 数据 | CRC  | ACK  | 帧尾   │
│  | 1bit | 11bit| 1bit | 6bit |0-8B  | 16bit| 2bit | 7bit   │
│  +------+------+------+------+------+------+------+         │
│                                                             │
│  扩展帧格式 (29位标识符):                                   │
│  +------+------+------+------+------+------+------+         │
│  | SOF  |BaseID|SRR |ExtID | RTR  | 控制 | 数据 | CRC...    │
│  | 1bit | 11bit|1bit|18bit| 1bit | 6bit |0-8B  |...         │
│  +------+------+------+------+------+------+------+         │
│                                                             │
│  SOF: 起始帧                                                │
│  ID: 标识符(决定优先级,数值越小优先级越高)                   │
│  RTR: 远程传输请求                                          │
│  CRC: 循环冗余校验                                          │
│  ACK: 应答                                                  │
└─────────────────────────────────────────────────────────────┘
```

**CAN总线特点：**

| 特性 | 参数 | 说明 |
|------|------|------|
| 传输速率 | 125Kbps~1Mbps | 高速CAN可达1Mbps |
| 传输距离 | 40m~10km | 速率越低距离越远 |
| 节点数 | 理论110个 | 实际受驱动能力限制 |
| 数据长度 | 0~8字节 | 每帧最多8字节 |
| 仲裁方式 | 非破坏性位仲裁 | 标识符越小优先级越高 |
| 错误检测 | CRC+ACK+位填充 | 多重错误检测机制 |

### 5.2 Linux SocketCAN完整实现

```c
/**
 * @file can_bus.h
 * @brief CAN总线通信头文件
 * @version 1.0.0
 */

#ifndef CAN_BUS_H
#define CAN_BUS_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/can.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAN帧结构(封装内核结构) */
typedef struct {
    uint32_t can_id;        /* CAN标识符 */
    uint8_t  can_dlc;       /* 数据长度码(0-8) */
    uint8_t  data[8];       /* 数据 */
    bool     is_extended;   /* 是否为扩展帧 */
    bool     is_rtr;        /* 是否为远程帧 */
} can_frame_t;

/* 过滤规则 */
typedef struct {
    uint32_t can_id;        /* 要过滤的ID */
    uint32_t can_mask;      /* 掩码 */
} can_filter_t;

/* CAN句柄 */
typedef struct {
    int socket_fd;              /* Socket文件描述符 */
    char interface[16];         /* 接口名(如 "can0") */
    struct sockaddr_can addr;   /* CAN地址 */
    uint64_t tx_count;          /* 发送帧数 */
    uint64_t rx_count;          /* 接收帧数 */
    uint64_t tx_errors;         /* 发送错误 */
    uint64_t rx_errors;         /* 接收错误 */
    uint64_t rx_dropped;        /* 丢帧数 */
} can_handle_t;

/* CAN速率定义 */
typedef enum {
    CAN_SPEED_10K   = 10000,
    CAN_SPEED_20K   = 20000,
    CAN_SPEED_50K   = 50000,
    CAN_SPEED_100K  = 100000,
    CAN_SPEED_125K  = 125000,
    CAN_SPEED_250K  = 250000,
    CAN_SPEED_500K  = 500000,
    CAN_SPEED_800K  = 800000,
    CAN_SPEED_1000K = 1000000
} can_speed_t;

/* 函数声明 */
int can_init(can_handle_t *handle, const char *interface, can_speed_t speed);
void can_close(can_handle_t *handle);
int can_send(can_handle_t *handle, const can_frame_t *frame);
int can_receive(can_handle_t *handle, can_frame_t *frame, uint32_t timeout_ms);
int can_add_filter(can_handle_t *handle, const can_filter_t *filter);
int can_clear_filters(can_handle_t *handle);
int can_set_loopback(can_handle_t *handle, bool enable);
int can_get_stats(can_handle_t *handle, struct can_device_stats *stats);
const char* can_speed_to_string(can_speed_t speed);

#ifdef __cplusplus
}
#endif

#endif /* CAN_BUS_H */
```

```c
/**
 * @file can_bus.c
 * @brief Linux SocketCAN完整实现
 */

#include "can_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <net/if.h>
#include <linux/can/raw.h>

/**
 * @brief 初始化CAN接口
 */
int can_init(can_handle_t *handle, const char *interface, can_speed_t speed)
{
    struct ifreq ifr;
    int ret;

    if (!handle || !interface) {
        return -1;
    }

    memset(handle, 0, sizeof(can_handle_t));
    strncpy(handle->interface, interface, sizeof(handle->interface) - 1);

    /* 创建CAN socket */
    handle->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (handle->socket_fd < 0) {
        perror("socket");
        return -1;
    }

    /* 获取接口索引 */
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    ret = ioctl(handle->socket_fd, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl SIOCGIFINDEX");
        close(handle->socket_fd);
        return -1;
    }

    /* 绑定到CAN接口 */
    handle->addr.can_family = AF_CAN;
    handle->addr.can_ifindex = ifr.ifr_ifindex;

    ret = bind(handle->socket_fd, (struct sockaddr *)&handle->addr,
               sizeof(handle->addr));
    if (ret < 0) {
        perror("bind");
        close(handle->socket_fd);
        return -1;
    }

    /* 设置非阻塞模式 */
    int flags = fcntl(handle->socket_fd, F_GETFL, 0);
    fcntl(handle->socket_fd, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

/**
 * @brief 关闭CAN接口
 */
void can_close(can_handle_t *handle)
{
    if (handle && handle->socket_fd >= 0) {
        close(handle->socket_fd);
        handle->socket_fd = -1;
    }
}

/**
 * @brief 发送CAN帧
 */
int can_send(can_handle_t *handle, const can_frame_t *frame)
{
    struct can_frame canframe;
    ssize_t sent;

    if (!handle || handle->socket_fd < 0 || !frame) {
        return -1;
    }

    /* 填充CAN帧 */
    memset(&canframe, 0, sizeof(canframe));

    canframe.can_id = frame->can_id;
    if (frame->is_extended) {
        canframe.can_id |= CAN_EFF_FLAG;
    }
    if (frame->is_rtr) {
        canframe.can_id |= CAN_RTR_FLAG;
    }

    canframe.can_dlc = frame->can_dlc;
    if (frame->can_dlc > 8) {
        canframe.can_dlc = 8;
    }

    memcpy(canframe.data, frame->data, canframe.can_dlc);

    /* 发送 */
    sent = write(handle->socket_fd, &canframe, sizeof(canframe));
    if (sent != sizeof(canframe)) {
        handle->tx_errors++;
        return -1;
    }

    handle->tx_count++;
    return 0;
}

/**
 * @brief 接收CAN帧
 */
int can_receive(can_handle_t *handle, can_frame_t *frame, uint32_t timeout_ms)
{
    struct can_frame canframe;
    fd_set readfds;
    struct timeval tv;
    int ret;
    ssize_t received;

    if (!handle || handle->socket_fd < 0 || !frame) {
        return -1;
    }

    /* 等待数据 */
    FD_ZERO(&readfds);
    FD_SET(handle->socket_fd, &readfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(handle->socket_fd + 1, &readfds, NULL, NULL,
                 timeout_ms > 0 ? &tv : NULL);

    if (ret < 0) {
        handle->rx_errors++;
        return -1;
    }

    if (ret == 0) {
        return 0;  /* 超时 */
    }

    /* 接收 */
    received = read(handle->socket_fd, &canframe, sizeof(canframe));
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        handle->rx_errors++;
        return -1;
    }

    if (received != sizeof(canframe)) {
        handle->rx_dropped++;
        return -1;
    }

    /* 解析帧 */
    frame->can_id = canframe.can_id & CAN_EFF_MASK;
    frame->is_extended = (canframe.can_id & CAN_EFF_FLAG) ? true : false;
    frame->is_rtr = (canframe.can_id & CAN_RTR_FLAG) ? true : false;
    frame->can_dlc = canframe.can_dlc;
    memcpy(frame->data, canframe.data, 8);

    handle->rx_count++;
    return 1;
}

/**
 * @brief 添加接收过滤器
 */
int can_add_filter(can_handle_t *handle, const can_filter_t *filter)
{
    struct can_filter rfilter;

    if (!handle || handle->socket_fd < 0 || !filter) {
        return -1;
    }

    rfilter.can_id = filter->can_id;
    rfilter.can_mask = filter->can_mask;

    return setsockopt(handle->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                      &rfilter, sizeof(rfilter));
}

/**
 * @brief 清除所有过滤器
 */
int can_clear_filters(can_handle_t *handle)
{
    if (!handle || handle->socket_fd < 0) {
        return -1;
    }

    /* 禁用过滤器(接收所有帧) */
    int filter_mode = 0;
    return setsockopt(handle->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                      &filter_mode, sizeof(filter_mode));
}

/**
 * @brief 设置回环模式
 */
int can_set_loopback(can_handle_t *handle, bool enable)
{
    int loopback = enable ? 1 : 0;

    if (!handle || handle->socket_fd < 0) {
        return -1;
    }

    return setsockopt(handle->socket_fd, SOL_CAN_RAW, CAN_RAW_LOOPBACK,
                      &loopback, sizeof(loopback));
}

/**
 * @brief 速率转字符串
 */
const char* can_speed_to_string(can_speed_t speed)
{
    switch (speed) {
        case CAN_SPEED_10K:   return "10Kbps";
        case CAN_SPEED_20K:   return "20Kbps";
        case CAN_SPEED_50K:   return "50Kbps";
        case CAN_SPEED_100K:  return "100Kbps";
        case CAN_SPEED_125K:  return "125Kbps";
        case CAN_SPEED_250K:  return "250Kbps";
        case CAN_SPEED_500K:  return "500Kbps";
        case CAN_SPEED_800K:  return "800Kbps";
        case CAN_SPEED_1000K: return "1Mbps";
        default:              return "Unknown";
    }
}
```

---

## 6. CANopen协议

### 6.1 CANopen概述

CANopen是基于CAN总线的高层协议，由CiA(CAN in Automation)制定，定义了应用层和通信配置。

```
┌─────────────────────────────────────────────────────────────┐
│                    CANopen通信对象                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  COB-ID范围:                                                │
│  +------------------+----------------+--------------------+   │
│  | COB-ID           | 通信对象       | 用途               |   │
│  +------------------+----------------+--------------------+   │
│  | 0x000            | NMT            | 网络管理           |   │
│  | 0x001-0x07F      | 保留           |                    |   │
│  | 0x080            | SYNC           | 同步对象           |   │
│  | 0x081-0x0FF      | EMCY           | 紧急报文           |   │
│  | 0x100-0x17F      | TPDO1/RPDO1    | 过程数据对象1      |   │
│  | 0x180-0x1FF      | TPDO2/RPDO2    | 过程数据对象2      |   │
│  | 0x200-0x27F      | TPDO3/RPDO3    | 过程数据对象3      |   │
│  | 0x280-0x2FF      | TPDO4/RPDO4    | 过程数据对象4      |   │
│  | 0x300-0x5FF      | SDO            | 服务数据对象       |   │
│  | 0x600-0x67F      | SDO            | 服务数据对象       |   │
│  | 0x700-0x77F      | NMT Error      | 节点守护/心跳      |   │
│  +------------------+----------------+--------------------+   │
│                                                             │
│  Node-ID: 1-127 (0和128-255保留)                            │
└─────────────────────────────────────────────────────────────┘
```

**CANopen对象字典(OD):**

```
┌─────────────────────────────────────────────────────────────┐
│                    对象字典结构                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  索引范围          | 用途                                    │
│  ─────────────────────────────────────────────────────────  │
│  0x0001-0x025F    | 数据类型定义                            │
│  0x0260-0x0FFF    | 保留                                    │
│  0x1000-0x1FFF    | 通信协议区(设备类型、错误寄存器等)      │
│  0x2000-0x5FFF    | 制造商特定区                            │
│  0x6000-0x9FFF    | 标准设备行规区                          │
│  0xA000-0xFFFF    | 保留                                    │
│                                                             │
│  常用对象字典条目:                                          │
│  0x1000 - 设备类型                                          │
│  0x1001 - 错误寄存器                                        │
│  0x1008 - 制造商设备名称                                    │
│  0x1018 - 身份对象(Vendor ID, Product Code等)               │
│  0x1200-0x127F - SDO服务器参数                              │
│  0x1400-0x15FF - RPDO通信参数                               │
│  0x1600-0x17FF - RPDO映射参数                               │
│  0x1800-0x19FF - TPDO通信参数                               │
│  0x1A00-0x1BFF - TPDO映射参数                               │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 CANopen SDO通信

```c
/**
 * @file canopen_sdo.h
 * @brief CANopen SDO协议实现
 */

#ifndef CANOPEN_SDO_H
#define CANOPEN_SDO_H

#include "can_bus.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SDO命令字节定义 */
#define SDO_CCS_DOWNLOAD_INITIATE   1   /* 客户端发起下载 */
#define SDO_CCS_DOWNLOAD_SEGMENT    0   /* 客户端下载段 */
#define SDO_CCS_UPLOAD_INITIATE     2   /* 客户端发起上传 */
#define SDO_CCS_UPLOAD_SEGMENT      3   /* 客户端请求段 */
#define SDO_CCS_ABORT               4   /* 中止传输 */

#define SDO_SCS_UPLOAD_INITIATE     2   /* 服务器响应上传启动 */
#define SDO_SCS_UPLOAD_SEGMENT      0   /* 服务器上传段 */
#define SDO_SCS_DOWNLOAD_INITIATE   3   /* 服务器响应下载启动 */
#define SDO_SCS_DOWNLOAD_SEGMENT    1   /* 服务器响应下载段 */

/* SDO客户端结构 */
typedef struct {
    can_handle_t *can;          /* CAN句柄 */
    uint8_t node_id;            /* 目标节点ID */
    uint32_t cob_id_tx;         /* SDO请求COB-ID */
    uint32_t cob_id_rx;         /* SDO响应COB-ID */
    uint32_t timeout_ms;        /* 超时时间 */
} sdo_client_t;

/* SDO错误码 */
typedef enum {
    SDO_OK = 0,
    SDO_ERR_TIMEOUT = -1,
    SDO_ERR_TOGGLE = -2,
    SDO_ERR_CRC = -3,
    SDO_ERR_SIZE = -4,
    SDO_ERR_ABORT = -5
} sdo_error_t;

/* 函数声明 */
int sdo_client_init(sdo_client_t *client, can_handle_t *can,
                    uint8_t node_id, uint32_t timeout_ms);
int sdo_download_expedited(sdo_client_t *client, uint16_t index,
                           uint8_t subindex, const uint8_t *data,
                           uint8_t len);
int sdo_upload_expedited(sdo_client_t *client, uint16_t index,
                         uint8_t subindex, uint8_t *data,
                         uint8_t *len);
int sdo_abort_transfer(sdo_client_t *client, uint16_t index,
                       uint8_t subindex, uint32_t abort_code);

#ifdef __cplusplus
}
#endif

#endif /* CANOPEN_SDO_H */
```

```c
/**
 * @file canopen_sdo.c
 * @brief CANopen SDO协议实现
 */

#include "canopen_sdo.h"
#include <stdio.h>
#include <string.h>

/* SDO COB-ID基地址 */
#define SDO_RX_BASE     0x600   /* 客户端→服务器 */
#define SDO_TX_BASE     0x580   /* 服务器→客户端 */

/* 构建CCS命令字节(加速传输) */
static uint8_t build_ccs_download_expedited(uint8_t data_len)
{
    uint8_t cmd = (SDO_CCS_DOWNLOAD_INITIATE << 5);
    cmd |= 0x03;  /* 加速传输 */
    cmd |= ((4 - data_len) << 2);  /* 未使用字节数 */
    cmd |= 0x02;  /* 大小指示 */
    return cmd;
}

/**
 * @brief 初始化SDO客户端
 */
int sdo_client_init(sdo_client_t *client, can_handle_t *can,
                    uint8_t node_id, uint32_t timeout_ms)
{
    if (!client || !can || node_id < 1 || node_id > 127) {
        return -1;
    }

    client->can = can;
    client->node_id = node_id;
    client->cob_id_tx = SDO_RX_BASE + node_id;
    client->cob_id_rx = SDO_TX_BASE + node_id;
    client->timeout_ms = timeout_ms;

    return 0;
}

/**
 * @brief 等待SDO响应
 */
static int wait_for_response(sdo_client_t *client, can_frame_t *response)
{
    uint32_t elapsed = 0;
    const uint32_t poll_interval = 10;

    while (elapsed < client->timeout_ms) {
        int ret = can_receive(client->can, response, poll_interval);
        if (ret > 0) {
            /* 检查是否是我们期望的响应 */
            if ((response->can_id & 0x7FF) == client->cob_id_rx) {
                return 0;
            }
        }
        elapsed += poll_interval;
    }

    return SDO_ERR_TIMEOUT;
}

/**
 * @brief SDO加速下载(expedited download)
 * 用于传输≤4字节的数据
 */
int sdo_download_expedited(sdo_client_t *client, uint16_t index,
                           uint8_t subindex, const uint8_t *data, uint8_t len)
{
    can_frame_t req, rsp;
    int ret;

    if (!client || !data || len > 4) {
        return -1;
    }

    /* 构建请求帧 */
    memset(&req, 0, sizeof(req));
    req.can_id = client->cob_id_tx;
    req.can_dlc = 8;
    req.is_extended = false;
    req.is_rtr = false;

    req.data[0] = build_ccs_download_expedited(len);
    req.data[1] = index & 0xFF;
    req.data[2] = (index >> 8) & 0xFF;
    req.data[3] = subindex;

    /* 复制数据(小端序) */
    memcpy(&req.data[4], data, len);
    /* 清空未使用的字节 */
    for (int i = 4 + len; i < 8; i++) {
        req.data[i] = 0;
    }

    /* 发送请求 */
    ret = can_send(client->can, &req);
    if (ret < 0) {
        return -1;
    }

    /* 等待响应 */
    ret = wait_for_response(client, &rsp);
    if (ret < 0) {
        return ret;
    }

    /* 检查响应 */
    uint8_t scs = (rsp.data[0] >> 5) & 0x07;
    if (scs == SDO_SCS_DOWNLOAD_INITIATE) {
        return 0;  /* 成功 */
    }

    if (scs == SDO_CCS_ABORT) {
        uint32_t abort_code = rsp.data[4] | (rsp.data[5] << 8) |
                              (rsp.data[6] << 16) | (rsp.data[7] << 24);
        printf("SDO中止,错误码: 0x%08X\n", abort_code);
        return SDO_ERR_ABORT;
    }

    return -1;
}

/**
 * @brief SDO加速上传(expedited upload)
 */
int sdo_upload_expedited(sdo_client_t *client, uint16_t index,
                         uint8_t subindex, uint8_t *data, uint8_t *len)
{
    can_frame_t req, rsp;
    int ret;

    if (!client || !data || !len) {
        return -1;
    }

    /* 构建请求 */
    memset(&req, 0, sizeof(req));
    req.can_id = client->cob_id_tx;
    req.can_dlc = 8;
    req.is_extended = false;
    req.is_rtr = false;

    req.data[0] = (SDO_CCS_UPLOAD_INITIATE << 5);
    req.data[1] = index & 0xFF;
    req.data[2] = (index >> 8) & 0xFF;
    req.data[3] = subindex;

    ret = can_send(client->can, &req);
    if (ret < 0) {
        return -1;
    }

    /* 等待响应 */
    ret = wait_for_response(client, &rsp);
    if (ret < 0) {
        return ret;
    }

    /* 检查响应 */
    uint8_t scs = (rsp.data[0] >> 5) & 0x07;
    if (scs != SDO_SCS_UPLOAD_INITIATE) {
        if (scs == SDO_CCS_ABORT) {
            return SDO_ERR_ABORT;
        }
        return -1;
    }

    /* 解析数据长度 */
    uint8_t cmd = rsp.data[0];
    if ((cmd & 0x02) == 0) {
        /* 非加速传输,不支持 */
        return -1;
    }

    /* 计算实际数据长度 */
    uint8_t unused = (cmd >> 2) & 0x03;
    *len = 4 - unused;

    /* 复制数据 */
    memcpy(data, &rsp.data[4], *len);

    return 0;
}

/**
 * @brief 中止SDO传输
 */
int sdo_abort_transfer(sdo_client_t *client, uint16_t index,
                       uint8_t subindex, uint32_t abort_code)
{
    can_frame_t req;

    if (!client) {
        return -1;
    }

    memset(&req, 0, sizeof(req));
    req.can_id = client->cob_id_tx;
    req.can_dlc = 8;
    req.is_extended = false;
    req.is_rtr = false;

    req.data[0] = (SDO_CCS_ABORT << 5);
    req.data[1] = index & 0xFF;
    req.data[2] = (index >> 8) & 0xFF;
    req.data[3] = subindex;
    req.data[4] = abort_code & 0xFF;
    req.data[5] = (abort_code >> 8) & 0xFF;
    req.data[6] = (abort_code >> 16) & 0xFF;
    req.data[7] = (abort_code >> 24) & 0xFF;

    return can_send(client->can, &req);
}
```

---

## 7. EtherCAT协议介绍

### 7.1 EtherCAT概述

EtherCAT(Ethernet for Control Automation Technology)是Beckhoff于2003年开发的实时工业以太网协议，以其极高的性能和灵活性著称。

```
┌─────────────────────────────────────────────────────────────┐
│                    EtherCAT工作原理                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  主站                                                       │
│   │                                                         │
│   │ 发送帧(经过所有从站后返回)                              │
│   ▼                                                         │
│  ┌──────┐   ┌──────┐   ┌──────┐   ┌──────┐                 │
│  │从站1 │──→│从站2 │──→│从站3 │──→│...   │──→ 回主站       │
│  │(处理)│   │(处理)│   │(处理)│   │      │                 │
│  └──────┘   └──────┘   └──────┘   └──────┘                 │
│                                                             │
│  特点:                                                      │
│  1. "on the fly"处理 - 帧经过从站时实时处理                 │
│  2. 拓扑灵活 - 支持线型、树型、星型、环型                   │
│  3. 极短周期 - 1000个I/O点可在30μs内刷新                   │
│  4. 单帧多从站 - 一帧可服务多个从站                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**EtherCAT帧结构：**

```
┌─────────────────────────────────────────────────────────────┐
│                    EtherCAT数据帧                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  以太网帧头                                                 │
│  +--------+--------+--------+--------+--------+             │
│  |目的MAC |源MAC   |EtherType|        |        |             │
│  | 6字节  | 6字节  |0x88A4  |        |        |             │
│  +--------+--------+--------+--------+--------+             │
│                                                             │
│  EtherCAT头                                                 │
│  +--------+--------+--------+                               │
│  | 长度  | 保留  | 类型   |                                  │
│  | 11bit | 1bit  | 4bit   |                                  │
│  +--------+--------+--------+                               │
│                                                             │
│  一个或多个子报文                                           │
│  +--------+--------+--------+--------+--------+             │
│  |命令   |索引   |地址   |长度   |R/I/M  |数据   |WKC       │
│  | 8bit  | 8bit  |32bit  |11bit  | 5bit  |N字节  |16bit     │
│  +--------+--------+--------+--------+--------+             │
│                                                             │
│  命令类型:                                                  │
│  0x01 - APRD  (自动增量物理读)                              │
│  0x02 - APWR  (自动增量物理写)                              │
│  0x04 - FPRD  (配置地址物理读)                              │
│  0x05 - FPWR  (配置地址物理写)                              │
│  0x07 - BRD   (广播读)                                      │
│  0x08 - BWR   (广播写)                                      │
│  0x0D - LRD   (逻辑读)                                      │
│  0x0E - LWR   (逻辑写)                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**EtherCAT状态机：**

```
┌─────────────────────────────────────────────────────────────┐
│                    EtherCAT状态机                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│     ┌─────────┐                                             │
│     │  Init   │ ← 上电状态,无通信                           │
│     └────┬────┘                                             │
│          │ 初始化                                            │
│          ▼                                                   │
│     ┌─────────┐                                             │
│     │Pre-Op  │ ← 邮箱通信(SDODownload/Upload)              │
│     └────┬────┘                                             │
│          │ 配置参数                                          │
│          ▼                                                   │
│     ┌─────────┐                                             │
│     │ SafeOp  │ ← 过程数据输入有效,输出保持安全状态         │
│     └────┬────┘                                             │
│          │ 应用就绪                                          │
│          ▼                                                   │
│     ┌─────────┐                                             │
│     │   Op    │ ← 完全运行,输入输出都有效                   │
│     └─────────┘                                             │
│                                                             │
│  任何状态 ←────────────────── 错误 → 错误处理              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 EtherCAT主站简单示例

```c
/**
 * @file ethercat_simple.h
 * @brief EtherCAT主站简单接口(基于SOEM库概念)
 *
 * 注意:完整EtherCAT实现需要专门的硬件和SOEM/IgH等库
 * 这里仅展示基本概念和结构
 */

#ifndef ETHERCAT_SIMPLE_H
#define ETHERCAT_SIMPLE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EtherCAT从站状态 */
typedef enum {
    EC_STATE_NONE           = 0x00,
    EC_STATE_INIT           = 0x01,
    EC_STATE_PRE_OP         = 0x02,
    EC_STATE_BOOT           = 0x03,
    EC_STATE_SAFE_OP        = 0x04,
    EC_STATE_OPERATIONAL    = 0x08,
    EC_STATE_ACK            = 0x10,
    EC_STATE_ERROR          = 0x10
} ec_state_t;

/* 从站信息 */
typedef struct {
    uint16_t station_address;       /* 站点地址 */
    uint32_t vendor_id;             /* 厂商ID */
    uint32_t product_code;          /* 产品代码 */
    uint32_t revision_number;       /* 版本号 */
    uint32_t serial_number;         /* 序列号 */
    ec_state_t state;               /* 当前状态 */
    uint16_t al_status;             /* 应用层状态 */
    uint16_t al_status_code;        /* 状态码 */
} ec_slave_info_t;

/* EtherCAT主站上下文 */
typedef struct {
    char ifname[32];                /* 网卡名 */
    int socket_fd;                  /* 原始socket */
    int slave_count;                /* 从站数量 */
    ec_slave_info_t *slaves;        /* 从站信息数组 */
    uint8_t *inputs;                /* 输入数据映射 */
    uint8_t *outputs;               /* 输出数据映射 */
    uint64_t cycle_time_us;         /* 周期时间(微秒) */
} ec_master_t;

/* 函数声明 */
int ec_init(ec_master_t *master, const char *ifname);
void ec_close(ec_master_t *master);
int ec_config_init(ec_master_t *master, bool use_cache);
int ec_config_map(ec_master_t *master);
int ec_writestate(ec_master_t *master, uint16_t slave, ec_state_t state);
int ec_statecheck(ec_master_t *master, uint16_t slave, ec_state_t state,
                  uint32_t timeout_us);
int ec_send_processdata(ec_master_t *master);
int ec_receive_processdata(ec_master_t *master, uint32_t timeout_us);

#ifdef __cplusplus
}
#endif

#endif /* ETHERCAT_SIMPLE_H */
```

---

## 8. Profinet/Profibus

### 8.1 Profibus概述

Profibus(Process Field Bus)是德国标准(DIN 19245)和欧洲标准(EN 50170)的现场总线标准。

```
┌─────────────────────────────────────────────────────────────┐
│                    Profibus协议族                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │ Profibus家族                                          │  │
│  ├───────────────────────────────────────────────────────┤  │
│  │                                                       │  │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐   │  │
│  │  │Profibus DP  │  │Profibus PA  │  │Profibus FMS │   │  │
│  │  │(分布式I/O)  │  │(过程自动化) │  │(现场总线    │   │  │
│  │  │             │  │             │  │ 报文规范)   │   │  │
│  │  └─────────────┘  └─────────────┘  └─────────────┘   │  │
│  │         │                │                │           │  │
│  │         │                │                │           │  │
│  │    工厂自动化       过程自动化          (已淘汰)       │  │
│  │    最高12Mbps       本质安全31.25kbps                  │  │
│  │                                                       │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 Profinet概述

Profinet是Profibus的以太网版本，基于工业以太网，兼容Profibus的通信模型。

```
┌─────────────────────────────────────────────────────────────┐
│                    Profinet协议栈                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  应用层 (Application Layer)                          │    │
│  │  ├─ Profinet IO (实时I/O通信)                        │    │
│  │  ├─ Profinet CBA (基于组件的自动化)                  │    │
│  │  └─ Profinet IO-Controller / IO-Device               │    │
│  ├─────────────────────────────────────────────────────┤    │
│  │  运行时环境 (RT/IRT)                                 │    │
│  │  ├─ NRT (Non-Real-Time): 标准TCP/IP (~100ms)        │    │
│  │  ├─ RT (Real-Time): 软件实时 (~1-10ms)              │    │
│  │  └─ IRT (Isochronous RT): 硬件实时 (~0.25-1ms)      │    │
│  ├─────────────────────────────────────────────────────┤    │
│  │  传输层                                              │    │
│  │  ├─ UDP/IP (RT通信)                                  │    │
│  │  ├─ TCP/IP (非实时通信)                              │    │
│  │  └─ 以太网 (IEEE 802.3)                              │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  Profinet设备类型:                                          │
│  ├─ IO-Controller: 控制器(类似Profibus主站)                 │
│  ├─ IO-Device: I/O设备(类似Profibus从站)                    │
│  └─ IO-Supervisor: 监控/配置设备                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Profinet IO设备模型：**

```
┌─────────────────────────────────────────────────────────────┐
│                    Profinet IO设备模型                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                   设备 (Device)                      │    │
│  │  ┌─────────────────────────────────────────────┐    │    │
│  │  │               插槽0 (DAP - 设备访问点)        │    │    │
│  │  │  ┌─────────────┐  ┌─────────────┐            │    │    │
│  │  │  │  子插槽1    │  │  子插槽2    │  ...       │    │    │
│  │  │  │ (接口)      │  │ (端口)      │            │    │    │
│  │  │  └─────────────┘  └─────────────┘            │    │    │
│  │  └─────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────┐    │    │
│  │  │               插槽1                         │    │    │
│  │  │  ┌─────────────┐  ┌─────────────┐            │    │    │
│  │  │  │  子插槽1    │  │  子插槽2    │  ...       │    │    │
│  │  │  │ (输入模块)  │  │ (输出模块)  │            │    │    │
│  │  │  └─────────────┘  └─────────────┘            │    │    │
│  │  └─────────────────────────────────────────────┘    │    │
│  │  ┌─────────────────────────────────────────────┐    │    │
│  │  │               插槽2                         │    │    │
│  │  │  ┌─────────────┐  ┌─────────────┐            │    │    │
│  │  │  │  子插槽1    │  │  子插槽2    │  ...       │    │    │
│  │  │  │ (AI模块)    │  │ (AO模块)    │            │    │    │
│  │  │  └─────────────┘  └─────────────┘            │    │    │
│  │  └─────────────────────────────────────────────┘    │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 9. 工业以太网

### 9.1 工业以太网概述

工业以太网是将标准以太网技术应用于工业环境的一系列协议和技术的总称。

```
┌─────────────────────────────────────────────────────────────┐
│                    工业以太网协议对比                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  协议          | 厂商        | 实时性    | 特点             │
│  ─────────────────────────────────────────────────────────  │
│  EtherNet/IP   | Rockwell    | 中        | CIP协议,设备行规 │
│  Profinet      | Siemens     | 高        | 兼容Profibus     │
│  EtherCAT      | Beckhoff    | 极高      | "On The Fly"     │
│  Modbus TCP    | Schneider   | 中        | 简单易用         │
│  POWERLINK     | B&R         | 极高      | 开源主站         │
│  SERCOS III    | SERCOS组织  | 极高      | 运动控制优化     │
│  CC-Link IE    | 三菱        | 高        | 亚洲流行         │
│                                                             │
│  实时性比较:                                                │
│  标准以太网:   不确定延迟,无实时性                          │
│  软实时:       1-10ms (NRT/RT)                              │
│  硬实时:       <1ms (IRT/Cycle)                             │
│  等时同步:     <100μs (Motion Control)                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 EtherNet/IP实现示例

```c
/**
 * @file ethernet_ip.h
 * @brief EtherNet/IP协议简单实现
 *
 * EtherNet/IP使用CIP(Common Industrial Protocol)作为应用层协议
 */

#ifndef ETHERNET_IP_H
#define ETHERNET_IP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EtherNet/IP命令 */
#define EIP_CMD_NOP             0x0000
#define EIP_CMD_LIST_SERVICES   0x0004
#define EIP_CMD_LIST_IDENTITY   0x0063
#define EIP_CMD_LIST_INTERFACES 0x0064
#define EIP_CMD_REGISTER_SESSION 0x0065
#define EIP_CMD_UNREGISTER_SESSION 0x0066
#define EIP_CMD_SEND_RR_DATA    0x006F
#define EIP_CMD_SEND_UNIT_DATA  0x0070

/* CIP服务 */
#define CIP_SERVICE_READ        0x4C
#define CIP_SERVICE_WRITE       0x4D

/* CIP数据类型 */
#define CIP_TYPE_BOOL           0xC1
#define CIP_TYPE_SINT           0xC2
#define CIP_TYPE_INT            0xC3
#define CIP_TYPE_DINT           0xC4
#define CIP_TYPE_REAL           0xCA

/* EtherNet/IP会话句柄 */
typedef struct {
    int socket_fd;
    uint32_t session_handle;
    uint32_t sequence_count;
    char target_ip[64];
    uint16_t target_port;
    uint32_t timeout_ms;
} eip_session_t;

/* 函数声明 */
int eip_connect(eip_session_t *session, const char *ip, uint16_t port);
void eip_disconnect(eip_session_t *session);
int eip_register_session(eip_session_t *session);
int eip_unregister_session(eip_session_t *session);
int eip_read_tag(eip_session_t *session, const char *tag_name,
                 uint8_t *data, uint16_t *len);
int eip_write_tag(eip_session_t *session, const char *tag_name,
                  uint16_t type, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* ETHERNET_IP_H */
```

```c
/**
 * @file ethernet_ip.c
 * @brief EtherNet/IP协议简单实现
 */

#include "ethernet_ip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define EIP_DEFAULT_PORT    44818
#define EIP_ENCAP_HEADER_LEN 24

/* EtherNet/IP封装头部 */
typedef struct __attribute__((packed)) {
    uint16_t command;
    uint16_t length;
    uint32_t session_handle;
    uint32_t status;
    uint64_t sender_context;
    uint32_t options;
} eip_encap_header_t;

/**
 * @brief 创建Encapsulation消息
 */
static int build_encap_message(uint8_t *buffer, uint16_t command,
                                uint32_t session_handle,
                                const uint8_t *data, uint16_t data_len)
{
    eip_encap_header_t *header = (eip_encap_header_t *)buffer;

    header->command = command;
    header->length = data_len;
    header->session_handle = session_handle;
    header->status = 0;
    header->sender_context = 0;
    header->options = 0;

    if (data && data_len > 0) {
        memcpy(buffer + EIP_ENCAP_HEADER_LEN, data, data_len);
    }

    return EIP_ENCAP_HEADER_LEN + data_len;
}

/**
 * @brief 连接到EtherNet/IP设备
 */
int eip_connect(eip_session_t *session, const char *ip, uint16_t port)
{
    struct sockaddr_in addr;

    if (!session || !ip) {
        return -1;
    }

    memset(session, 0, sizeof(eip_session_t));
    strncpy(session->target_ip, ip, sizeof(session->target_ip) - 1);
    session->target_port = port > 0 ? port : EIP_DEFAULT_PORT;
    session->timeout_ms = 5000;

    /* 创建socket */
    session->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (session->socket_fd < 0) {
        return -1;
    }

    /* 设置地址 */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(session->target_port);
    inet_pton(AF_INET, session->target_ip, &addr.sin_addr);

    /* 连接 */
    if (connect(session->socket_fd, (struct sockaddr *)&addr,
                sizeof(addr)) < 0) {
        close(session->socket_fd);
        session->socket_fd = -1;
        return -1;
    }

    return 0;
}

/**
 * @brief 断开连接
 */
void eip_disconnect(eip_session_t *session)
{
    if (session) {
        if (session->socket_fd >= 0) {
            close(session->socket_fd);
            session->socket_fd = -1;
        }
        session->session_handle = 0;
    }
}

/**
 * @brief 注册会话
 */
int eip_register_session(eip_session_t *session)
{
    uint8_t req[256], rsp[256];
    uint16_t register_data[2] = {0x0100, 0x0000};  /* 协议版本,选项 */
    int req_len, rsp_len;

    if (!session || session->socket_fd < 0) {
        return -1;
    }

    /* 构建注册请求 */
    req_len = build_encap_message(req, EIP_CMD_REGISTER_SESSION, 0,
                                   (uint8_t *)register_data, 4);

    /* 发送 */
    if (send(session->socket_fd, req, req_len, 0) != req_len) {
        return -1;
    }

    /* 接收响应 */
    rsp_len = recv(session->socket_fd, rsp, sizeof(rsp), 0);
    if (rsp_len < EIP_ENCAP_HEADER_LEN + 4) {
        return -1;
    }

    /* 解析会话句柄 */
    eip_encap_header_t *header = (eip_encap_header_t *)rsp;
    if (header->command != EIP_CMD_REGISTER_SESSION || header->status != 0) {
        return -1;
    }

    session->session_handle = header->session_handle;

    return 0;
}

/**
 * @brief 注销会话
 */
int eip_unregister_session(eip_session_t *session)
{
    uint8_t req[256], rsp[256];
    int req_len;

    if (!session || session->socket_fd < 0) {
        return -1;
    }

    req_len = build_encap_message(req, EIP_CMD_UNREGISTER_SESSION,
                                   session->session_handle, NULL, 0);

    send(session->socket_fd, req, req_len, 0);
    recv(session->socket_fd, rsp, sizeof(rsp), 0);

    session->session_handle = 0;
    return 0;
}
```


---

## 10. 无线工业通信

### 10.1 工业无线概述

工业无线通信正在快速发展，主要技术包括：

```
┌─────────────────────────────────────────────────────────────┐
│                    工业无线技术对比                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  技术          | 频段      | 速率      | 距离      | 特点    │
│  ─────────────────────────────────────────────────────────  │
│  WiFi 4/5/6    | 2.4/5GHz  | 1Gbps+    | 100m      | 高带宽  │
│  4G LTE        | 授权频段  | 100Mbps   | km级      | 广覆盖  │
│  5G            | 授权频段  | 10Gbps    | km级      | 低时延  │
│  LoRa          | Sub-GHz   | 50kbps    | 15km      | 超远距  │
│  ZigBee        | 2.4GHz    | 250kbps   | 100m      | 低功耗  │
│  WirelessHART  | 2.4GHz    | 250kbps   | 200m      | 工业级  │
│  NB-IoT        | 授权频段  | 100kbps   | km级      | 广覆盖  │
│                                                             │
│  工业应用选择:                                              │
│  ├─ 视频监控/大数据: WiFi 6, 5G                             │
│  ├─ 移动设备/远程监控: 4G/5G                                │
│  ├─ 传感器网络: LoRa, ZigBee, WirelessHART                  │
│  └─ 大规模传感器: NB-IoT                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 10.2 4G/5G工业DTU实现

```c
/**
 * @file industrial_4g.h
 * @brief 工业4G/5G DTU通信模块
 *
 * 通过AT命令控制4G/5G模块(如移远、SIMCom等)
 */

#ifndef INDUSTRIAL_4G_H
#define INDUSTRIAL_4G_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 4G模块句柄 */
typedef struct {
    int serial_fd;              /* 串口文件描述符 */
    char apn[64];               /* APN接入点 */
    char server_ip[64];         /* 服务器IP */
    uint16_t server_port;       /* 服务器端口 */
    bool connected;             /* 连接状态 */
    int8_t signal_level;        /* 信号强度(0-31, 99=未知) */
    uint8_t network_type;       /* 网络类型 */
} dtu_handle_t;

/* 网络类型 */
#define NET_TYPE_UNKNOWN    0
#define NET_TYPE_2G         2
#define NET_TYPE_3G         3
#define NET_TYPE_4G         4
#define NET_TYPE_5G         5

/* 函数声明 */
int dtu_init(dtu_handle_t *handle, const char *serial_port, int baudrate);
void dtu_close(dtu_handle_t *handle);
int dtu_check_module(dtu_handle_t *handle);
int dtu_get_signal(dtu_handle_t *handle, int8_t *level);
int dtu_get_network_type(dtu_handle_t *handle, uint8_t *type);
int dtu_set_apn(dtu_handle_t *handle, const char *apn);
int dtu_connect_tcp(dtu_handle_t *handle, const char *ip, uint16_t port);
int dtu_disconnect(dtu_handle_t *handle);
int dtu_send_data(dtu_handle_t *handle, const uint8_t *data, uint16_t len);
int dtu_receive_data(dtu_handle_t *handle, uint8_t *buffer, uint16_t max_len,
                     uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* INDUSTRIAL_4G_H */
```

```c
/**
 * @file industrial_4g.c
 * @brief 工业4G DTU实现(AT命令方式)
 */

#include "industrial_4g.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

#define AT_TIMEOUT_MS       3000
#define AT_RETRY_COUNT      3

/**
 * @brief 打开串口
 */
static int open_serial(const char *port, int baudrate)
{
    int fd;
    struct termios tty;

    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        return -1;
    }

    tcgetattr(fd, &tty);

    /* 设置波特率 */
    speed_t speed;
    switch (baudrate) {
        case 9600:   speed = B9600; break;
        case 19200:  speed = B19200; break;
        case 38400:  speed = B38400; break;
        case 115200: speed = B115200; break;
        default:     speed = B115200; break;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    /* 8N1 */
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &tty);
    tcflush(fd, TCIOFLUSH);

    return fd;
}

/**
 * @brief 发送AT命令并等待响应
 */
static int at_command(int fd, const char *cmd, char *response,
                      size_t resp_len, uint32_t timeout_ms)
{
    char buffer[512];
    fd_set readfds;
    struct timeval tv;
    int ret;
    ssize_t n;
    size_t total = 0;

    /* 发送命令 */
    snprintf(buffer, sizeof(buffer), "%s\r\n", cmd);
    write(fd, buffer, strlen(buffer));
    tcdrain(fd);

    /* 等待响应 */
    memset(response, 0, resp_len);

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(fd + 1, &readfds, NULL, NULL, &tv);
    if (ret <= 0) {
        return -1;
    }

    /* 读取响应 */
    while (total < resp_len - 1) {
        n = read(fd, response + total, resp_len - total - 1);
        if (n > 0) {
            total += n;
            response[total] = '\0';
            /* 检查是否收到OK或ERROR */
            if (strstr(response, "OK") || strstr(response, "ERROR")) {
                break;
            }
        } else if (n < 0 && errno != EAGAIN) {
            break;
        }

        /* 再等待一点数据 */
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
            break;
        }
    }

    return (strstr(response, "OK") != NULL) ? 0 : -1;
}

/**
 * @brief 初始化DTU
 */
int dtu_init(dtu_handle_t *handle, const char *serial_port, int baudrate)
{
    if (!handle || !serial_port) {
        return -1;
    }

    memset(handle, 0, sizeof(dtu_handle_t));

    handle->serial_fd = open_serial(serial_port, baudrate);
    if (handle->serial_fd < 0) {
        return -1;
    }

    return 0;
}

/**
 * @brief 关闭DTU
 */
void dtu_close(dtu_handle_t *handle)
{
    if (handle && handle->serial_fd >= 0) {
        dtu_disconnect(handle);
        close(handle->serial_fd);
        handle->serial_fd = -1;
    }
}

/**
 * @brief 检查模块是否就绪
 */
int dtu_check_module(dtu_handle_t *handle)
{
    char response[256];
    int retries = AT_RETRY_COUNT;

    while (retries-- > 0) {
        if (at_command(handle->serial_fd, "AT", response,
                       sizeof(response), AT_TIMEOUT_MS) == 0) {
            return 0;
        }
        usleep(100000);
    }

    return -1;
}

/**
 * @brief 获取信号强度
 */
int dtu_get_signal(dtu_handle_t *handle, int8_t *level)
{
    char response[256];
    char *ptr;
    int rssi;

    if (at_command(handle->serial_fd, "AT+CSQ", response,
                   sizeof(response), AT_TIMEOUT_MS) < 0) {
        return -1;
    }

    /* 解析 +CSQ: <rssi>,<ber> */
    ptr = strstr(response, "+CSQ:");
    if (!ptr) {
        return -1;
    }

    sscanf(ptr, "+CSQ: %d", &rssi);
    handle->signal_level = (int8_t)rssi;
    if (level) *level = handle->signal_level;

    return 0;
}

/**
 * @brief 设置APN
 */
int dtu_set_apn(dtu_handle_t *handle, const char *apn)
{
    char cmd[128];
    char response[256];

    snprintf(cmd, sizeof(cmd), "AT+CGDCONT=1,\"IP\",\"%s\"", apn);

    if (at_command(handle->serial_fd, cmd, response,
                   sizeof(response), AT_TIMEOUT_MS) < 0) {
        return -1;
    }

    strncpy(handle->apn, apn, sizeof(handle->apn) - 1);
    return 0;
}

/**
 * @brief TCP连接到服务器
 */
int dtu_connect_tcp(dtu_handle_t *handle, const char *ip, uint16_t port)
{
    char cmd[256];
    char response[256];

    /* 先断开之前的连接 */
    dtu_disconnect(handle);

    /* 设置PDP上下文激活 */
    at_command(handle->serial_fd, "AT+CGACT=1,1", response,
               sizeof(response), AT_TIMEOUT_MS);

    /* 建立TCP连接(具体AT命令因模块而异) */
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);

    if (at_command(handle->serial_fd, cmd, response,
                   sizeof(response), 10000) < 0) {
        return -1;
    }

    strncpy(handle->server_ip, ip, sizeof(handle->server_ip) - 1);
    handle->server_port = port;
    handle->connected = true;

    return 0;
}

/**
 * @brief 断开连接
 */
int dtu_disconnect(dtu_handle_t *handle)
{
    char response[256];

    at_command(handle->serial_fd, "AT+CIPCLOSE", response,
               sizeof(response), AT_TIMEOUT_MS);

    handle->connected = false;
    return 0;
}

/**
 * @brief 发送数据
 */
int dtu_send_data(dtu_handle_t *handle, const uint8_t *data, uint16_t len)
{
    char cmd[64];
    char response[256];
    char prompt[4];

    if (!handle->connected) {
        return -1;
    }

    /* 发送数据长度命令 */
    snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d", len);
    write(handle->serial_fd, cmd, strlen(cmd));
    write(handle->serial_fd, "\r\n", 2);
    tcdrain(handle->serial_fd);

    /* 等待 > 提示符 */
    usleep(100000);
    read(handle->serial_fd, response, sizeof(response));

    if (!strstr(response, ">")) {
        return -1;
    }

    /* 发送数据 */
    write(handle->serial_fd, data, len);

    /* 发送结束符(通常是Ctrl+Z = 0x1A) */
    write(handle->serial_fd, "\x1A", 1);
    tcdrain(handle->serial_fd);

    /* 等待SEND OK */
    if (at_command(handle->serial_fd, "", response,
                   sizeof(response), 5000) < 0) {
        return -1;
    }

    return len;
}

/**
 * @brief 接收数据
 */
int dtu_receive_data(dtu_handle_t *handle, uint8_t *buffer, uint16_t max_len,
                     uint32_t timeout_ms)
{
    fd_set readfds;
    struct timeval tv;
    int ret;
    ssize_t n;

    FD_ZERO(&readfds);
    FD_SET(handle->serial_fd, &readfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    ret = select(handle->serial_fd + 1, &readfds, NULL, NULL, &tv);
    if (ret <= 0) {
        return 0;
    }

    n = read(handle->serial_fd, buffer, max_len);
    if (n > 0) {
        /* 解析收到的数据(可能需要从+IPD报文中提取) */
        char *data_ptr = strstr((char *)buffer, "+IPD,");
        if (data_ptr) {
            int data_len;
            char *colon = strchr(data_ptr, ':');
            if (colon) {
                sscanf(data_ptr, "+IPD,%d", &data_len);
                memmove(buffer, colon + 1, data_len);
                return data_len;
            }
        }
    }

    return (int)n;
}
```

---

## 11. 实际项目:多协议转换网关

### 11.1 项目概述

多协议转换网关是工业物联网中的核心设备，负责不同协议之间的数据转换和转发。

```
┌─────────────────────────────────────────────────────────────┐
│                    多协议转换网关架构                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                  协议网关主程序                        │    │
│  ├─────────────────────────────────────────────────────┤    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │    │
│  │  │ Modbus RTU  │  │  Modbus TCP │  │   CANopen   │  │    │
│  │  │   适配器    │  │   适配器    │  │   适配器    │  │    │
│  │  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  │    │
│  │         │                │                │         │    │
│  │  ┌──────┴────────────────┴────────────────┴──────┐  │    │
│  │  │           统一数据模型 / 消息总线              │  │    │
│  │  └──────┬────────────────┬────────────────┬──────┘  │    │
│  │         │                │                │         │    │
│  │  ┌──────┴──────┐  ┌──────┴──────┐  ┌──────┴──────┐  │    │
│  │  │MQTT发布/订阅│  │HTTP REST API│  │本地数据缓存  │  │    │
│  │  │  云连接    │  │  外部接口   │  │SQLite存储   │  │    │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  │    │
│  └─────────────────────────────────────────────────────┘    │
│                                                             │
│  硬件接口:                                                  │
│  ├─ RS-485 × 2 (Modbus RTU设备)                            │
│  ├─ RS-232 × 1 (调试/配置)                                 │
│  ├─ CAN × 1 (CANopen设备)                                  │
│  ├─ Ethernet × 1 (Modbus TCP/云服务)                       │
│  └─ 4G/WiFi (无线云连接)                                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 11.2 网关完整代码

```c
/**
 * @file protocol_gateway.c
 * @brief 工业多协议转换网关主程序
 * @version 1.0.0
 *
 * 本程序实现了一个完整的多协议工业网关，支持:
 * - Modbus RTU (RS-485)
 * - Modbus TCP (以太网)
 * - CAN/CANopen
 * - MQTT上传
 * - HTTP配置接口
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

/* 包含各协议头文件 */
#include "serial_port.h"
#include "modbus_rtu.h"
#include "modbus_tcp.h"
#include "can_bus.h"
#include "canopen_sdo.h"
#include "mqtt_client.h"
#include "data_cache.h"

#define GATEWAY_VERSION     "1.0.0"
#define MAX_DEVICE_TYPES    3
#define MAX_DEVICES         16
#define MAX_DATA_POINTS     256

/* 设备类型定义 */
typedef enum {
    DEV_TYPE_MODBUS_RTU = 0,
    DEV_TYPE_MODBUS_TCP = 1,
    DEV_TYPE_CANOPEN    = 2
} device_type_t;

/* 设备状态 */
typedef enum {
    DEV_STATUS_OFFLINE = 0,
    DEV_STATUS_ONLINE  = 1,
    DEV_STATUS_ERROR   = 2
} device_status_t;

/* 数据点定义 */
typedef struct {
    char name[64];          /* 数据点名称 */
    char unit[16];          /* 单位 */
    uint16_t reg_addr;      /* 寄存器地址 */
    uint8_t reg_type;       /* 寄存器类型 */
    uint8_t data_type;      /* 数据类型 */
    float scale;            /* 比例因子 */
    float offset;           /* 偏移量 */
    float value;            /* 当前值 */
    uint32_t timestamp;     /* 时间戳 */
    bool alarm_high;        /* 高报警 */
    bool alarm_low;         /* 低报警 */
    float alarm_high_val;   /* 高报警阈值 */
    float alarm_low_val;    /* 低报警阈值 */
} data_point_t;

/* 设备配置 */
typedef struct {
    uint8_t device_id;          /* 设备ID */
    device_type_t type;         /* 设备类型 */
    device_status_t status;     /* 设备状态 */
    char name[64];              /* 设备名称 */

    /* 连接参数 */
    union {
        struct {
            char port[64];          /* 串口设备 */
            int baudrate;           /* 波特率 */
            uint8_t slave_addr;     /* 从机地址 */
            modbus_context_t *ctx;  /* Modbus上下文 */
        } modbus_rtu;

        struct {
            char ip[64];            /* IP地址 */
            int port;               /* 端口 */
            uint8_t slave_addr;     /* 单元ID */
            modbus_tcp_context_t *ctx;
        } modbus_tcp;

        struct {
            char interface[16];     /* CAN接口 */
            uint8_t node_id;        /* Node-ID */
            sdo_client_t sdo;       /* SDO客户端 */
        } canopen;
    } conn;

    /* 数据点 */
    data_point_t *points;
    uint16_t point_count;

    /* 统计 */
    uint64_t read_count;
    uint64_t read_errors;
    uint32_t last_read_time;

    /* 同步 */
    pthread_mutex_t lock;
} device_config_t;

/* 网关配置 */
typedef struct {
    /* MQTT配置 */
    struct {
        char broker[128];
        int port;
        char client_id[64];
        char username[64];
        char password[64];
        char topic_prefix[128];
        int publish_interval;
        bool ssl_enable;
    } mqtt;

    /* HTTP配置 */
    struct {
        int port;
        bool enable;
    } http;

    /* 数据缓存 */
    struct {
        bool enable;
        char db_path[256];
        int retention_days;
    } cache;

    /* 扫描间隔(ms) */
    uint32_t scan_interval;

} gateway_config_t;

/* 网关上下文 */
typedef struct {
    device_config_t devices[MAX_DEVICES];
    uint8_t device_count;
    gateway_config_t config;

    /* 运行状态 */
    volatile bool running;
    volatile bool paused;

    /* 线程 */
    pthread_t scan_thread;
    pthread_t mqtt_thread;
    pthread_t http_thread;

    /* 统计 */
    uint64_t total_reads;
    uint64_t total_errors;
    time_t start_time;

} gateway_context_t;

static gateway_context_t g_gateway;

/* 函数声明 */
static int load_config(const char *config_file);
static int init_devices(void);
static void cleanup_devices(void);
static int read_device_data(device_config_t *dev);
static void *scan_thread_func(void *arg);
static void *mqtt_thread_func(void *arg);
static int publish_to_mqtt(device_config_t *dev);
static void signal_handler(int sig);

/**
 * @brief 打印网关信息
 */
static void print_banner(void)
{
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║     工业多协议转换网关 v%s                        ║\n", GATEWAY_VERSION);
    printf("║                                                        ║\n");
    printf("║  支持协议: Modbus RTU | Modbus TCP | CANopen          ║\n");
    printf("║  上行协议: MQTT / HTTP REST API                       ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * @brief 获取当前时间戳(毫秒)
 */
static uint64_t get_timestamp_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief 加载配置文件
 *
 * 实际项目中应使用JSON/YAML/XML解析器
 * 这里使用简单格式演示
 */
static int load_config(const char *config_file)
{
    /* 默认配置 */
    memset(&g_gateway.config, 0, sizeof(gateway_config_t));

    /* MQTT默认配置 */
    strcpy(g_gateway.config.mqtt.broker, "mqtt.example.com");
    g_gateway.config.mqtt.port = 1883;
    snprintf(g_gateway.config.mqtt.client_id, sizeof(g_gateway.config.mqtt.client_id),
             "gateway_%ld", (long)getpid());
    strcpy(g_gateway.config.mqtt.topic_prefix, "factory/gateway1");
    g_gateway.config.mqtt.publish_interval = 5000;  /* 5秒 */

    /* HTTP默认配置 */
    g_gateway.config.http.port = 8080;
    g_gateway.config.http.enable = true;

    /* 扫描间隔 */
    g_gateway.config.scan_interval = 1000;  /* 1秒 */

    /* 从文件加载配置(简化版) */
    FILE *fp = fopen(config_file, "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            /* 简单解析key=value格式 */
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *key = line;
                char *value = eq + 1;
                /* 去除换行符 */
                value[strcspn(value, "\r\n")] = '\0';

                if (strcmp(key, "mqtt_broker") == 0) {
                    strncpy(g_gateway.config.mqtt.broker, value,
                            sizeof(g_gateway.config.mqtt.broker) - 1);
                } else if (strcmp(key, "mqtt_port") == 0) {
                    g_gateway.config.mqtt.port = atoi(value);
                }
                /* ... 更多配置项 ... */
            }
        }
        fclose(fp);
    }

    /* 加载设备配置(示例) */
    g_gateway.device_count = 0;

    /* 设备1: Modbus RTU温度传感器 */
    {
        device_config_t *dev = &g_gateway.devices[g_gateway.device_count++];
        memset(dev, 0, sizeof(device_config_t));
        dev->device_id = 1;
        dev->type = DEV_TYPE_MODBUS_RTU;
        strcpy(dev->name, "温度传感器1");
        strcpy(dev->conn.modbus_rtu.port, "/dev/ttyUSB0");
        dev->conn.modbus_rtu.baudrate = 9600;
        dev->conn.modbus_rtu.slave_addr = 1;
        pthread_mutex_init(&dev->lock, NULL);

        /* 配置数据点 */
        dev->points = calloc(2, sizeof(data_point_t));
        dev->point_count = 2;

        /* 温度值 */
        strcpy(dev->points[0].name, "temperature");
        strcpy(dev->points[0].unit, "℃");
        dev->points[0].reg_addr = 0;
        dev->points[0].reg_type = 0x03;  /* 保持寄存器 */
        dev->points[0].data_type = 0x03; /* 16位有符号 */
        dev->points[0].scale = 0.1f;
        dev->points[0].offset = 0;
        dev->points[0].alarm_high_val = 80.0f;
        dev->points[0].alarm_low_val = -20.0f;

        /* 湿度值 */
        strcpy(dev->points[1].name, "humidity");
        strcpy(dev->points[1].unit, "%RH");
        dev->points[1].reg_addr = 1;
        dev->points[1].reg_type = 0x03;
        dev->points[1].data_type = 0x03;
        dev->points[1].scale = 0.1f;
        dev->points[1].alarm_high_val = 90.0f;
        dev->points[1].alarm_low_val = 10.0f;
    }

    /* 设备2: Modbus TCP电能表 */
    {
        device_config_t *dev = &g_gateway.devices[g_gateway.device_count++];
        memset(dev, 0, sizeof(device_config_t));
        dev->device_id = 2;
        dev->type = DEV_TYPE_MODBUS_TCP;
        strcpy(dev->name, "电能表1");
        strcpy(dev->conn.modbus_tcp.ip, "192.168.1.100");
        dev->conn.modbus_tcp.port = 502;
        dev->conn.modbus_tcp.slave_addr = 1;
        pthread_mutex_init(&dev->lock, NULL);

        dev->points = calloc(3, sizeof(data_point_t));
        dev->point_count = 3;

        strcpy(dev->points[0].name, "voltage");
        strcpy(dev->points[0].unit, "V");
        dev->points[0].reg_addr = 0;
        dev->points[0].scale = 0.01f;

        strcpy(dev->points[1].name, "current");
        strcpy(dev->points[1].unit, "A");
        dev->points[1].reg_addr = 2;
        dev->points[1].scale = 0.001f;

        strcpy(dev->points[2].name, "power");
        strcpy(dev->points[2].unit, "W");
        dev->points[2].reg_addr = 4;
        dev->points[2].scale = 0.1f;
    }

    printf("已加载 %d 个设备配置\n", g_gateway.device_count);
    return 0;
}

/**
 * @brief 初始化所有设备连接
 */
static int init_devices(void)
{
    int success_count = 0;

    for (int i = 0; i < g_gateway.device_count; i++) {
        device_config_t *dev = &g_gateway.devices[i];
        int ret = -1;

        printf("初始化设备 %d (%s)... ", dev->device_id, dev->name);

        switch (dev->type) {
            case DEV_TYPE_MODBUS_RTU: {
                modbus_context_t *ctx = modbus_new_rtu(
                    dev->conn.modbus_rtu.port,
                    dev->conn.modbus_rtu.baudrate,
                    'N', 8, 1);
                if (ctx) {
                    modbus_set_slave(ctx, dev->conn.modbus_rtu.slave_addr);
                    modbus_set_response_timeout(ctx, 1000);
                    modbus_connect(ctx);
                    dev->conn.modbus_rtu.ctx = ctx;
                    dev->status = DEV_STATUS_ONLINE;
                    ret = 0;
                }
                break;
            }

            case DEV_TYPE_MODBUS_TCP: {
                modbus_tcp_context_t *ctx = modbus_tcp_new(
                    dev->conn.modbus_tcp.ip,
                    dev->conn.modbus_tcp.port);
                if (ctx) {
                    ret = modbus_tcp_connect(ctx);
                    if (ret == 0) {
                        dev->conn.modbus_tcp.ctx = ctx;
                        dev->status = DEV_STATUS_ONLINE;
                    }
                }
                break;
            }

            case DEV_TYPE_CANOPEN: {
                /* CANopen初始化 */
                break;
            }

            default:
                break;
        }

        if (ret == 0) {
            printf("成功\n");
            success_count++;
        } else {
            printf("失败\n");
            dev->status = DEV_STATUS_ERROR;
        }
    }

    printf("设备初始化: %d/%d 成功\n", success_count, g_gateway.device_count);
    return success_count;
}

/**
 * @brief 清理设备连接
 */
static void cleanup_devices(void)
{
    for (int i = 0; i < g_gateway.device_count; i++) {
        device_config_t *dev = &g_gateway.devices[i];

        switch (dev->type) {
            case DEV_TYPE_MODBUS_RTU:
                if (dev->conn.modbus_rtu.ctx) {
                    modbus_close(dev->conn.modbus_rtu.ctx);
                    modbus_free(dev->conn.modbus_rtu.ctx);
                }
                break;

            case DEV_TYPE_MODBUS_TCP:
                if (dev->conn.modbus_tcp.ctx) {
                    modbus_tcp_close(dev->conn.modbus_tcp.ctx);
                    modbus_tcp_free(dev->conn.modbus_tcp.ctx);
                }
                break;

            case DEV_TYPE_CANOPEN:
                break;
        }

        if (dev->points) {
            free(dev->points);
        }

        pthread_mutex_destroy(&dev->lock);
    }
}

/**
 * @brief 读取单个设备数据
 */
static int read_device_data(device_config_t *dev)
{
    int ret = -1;
    uint16_t registers[16];
    uint64_t start_time = get_timestamp_ms();

    pthread_mutex_lock(&dev->lock);

    if (dev->status == DEV_STATUS_OFFLINE) {
        pthread_mutex_unlock(&dev->lock);
        return -1;
    }

    switch (dev->type) {
        case DEV_TYPE_MODBUS_RTU: {
            if (dev->conn.modbus_rtu.ctx) {
                /* 读取保持寄存器 */
                ret = modbus_read_registers(
                    dev->conn.modbus_rtu.ctx,
                    dev->points[0].reg_addr,
                    dev->point_count,
                    registers);
            }
            break;
        }

        case DEV_TYPE_MODBUS_TCP: {
            if (dev->conn.modbus_tcp.ctx) {
                ret = modbus_tcp_read_registers(
                    dev->conn.modbus_tcp.ctx,
                    dev->points[0].reg_addr,
                    dev->point_count,
                    registers);
            }
            break;
        }

        case DEV_TYPE_CANOPEN: {
            /* CANopen SDO读取 */
            break;
        }
    }

    if (ret > 0) {
        /* 解析数据 */
        uint32_t timestamp = (uint32_t)(start_time / 1000);

        for (int i = 0; i < dev->point_count && i < ret; i++) {
            float raw_value = (float)(int16_t)registers[i];
            dev->points[i].value = raw_value * dev->points[i].scale
                                   + dev->points[i].offset;
            dev->points[i].timestamp = timestamp;

            /* 报警检查 */
            dev->points[i].alarm_high =
                dev->points[i].value > dev->points[i].alarm_high_val;
            dev->points[i].alarm_low =
                dev->points[i].value < dev->points[i].alarm_low_val;
        }

        dev->status = DEV_STATUS_ONLINE;
        dev->read_count++;
        dev->last_read_time = timestamp;
        g_gateway.total_reads++;
    } else {
        dev->read_errors++;
        g_gateway.total_errors++;

        if (dev->read_errors > 10) {
            dev->status = DEV_STATUS_ERROR;
        }
    }

    pthread_mutex_unlock(&dev->lock);

    return ret;
}

/**
 * @brief 扫描线程 - 周期性读取所有设备数据
 */
static void *scan_thread_func(void *arg)
{
    (void)arg;

    printf("扫描线程启动 (间隔: %d ms)\n", g_gateway.config.scan_interval);

    while (g_gateway.running) {
        if (!g_gateway.paused) {
            for (int i = 0; i < g_gateway.device_count; i++) {
                device_config_t *dev = &g_gateway.devices[i];

                if (dev->status != DEV_STATUS_OFFLINE) {
                    int ret = read_device_data(dev);

                    if (g_gateway.config.cache.enable && ret > 0) {
                        /* 保存到本地缓存 */
                    }
                }
            }
        }

        /* 睡眠等待下一个周期 */
        usleep(g_gateway.config.scan_interval * 1000);
    }

    printf("扫描线程退出\n");
    return NULL;
}

/**
 * @brief MQTT发布线程
 */
static void *mqtt_thread_func(void *arg)
{
    (void)arg;

    printf("MQTT线程启动\n");

    /* MQTT客户端初始化 */
    mqtt_client_t mqtt;
    int ret = mqtt_connect(&mqtt, g_gateway.config.mqtt.broker,
                           g_gateway.config.mqtt.port,
                           g_gateway.config.mqtt.client_id,
                           g_gateway.config.mqtt.username,
                           g_gateway.config.mqtt.password);
    if (ret < 0) {
        printf("MQTT连接失败\n");
        return NULL;
    }

    while (g_gateway.running) {
        if (!g_gateway.paused) {
            /* 发布所有设备数据 */
            for (int i = 0; i < g_gateway.device_count; i++) {
                device_config_t *dev = &g_gateway.devices[i];

                if (dev->status == DEV_STATUS_ONLINE && dev->read_count > 0) {
                    /* 构建JSON消息 */
                    char payload[2048];
                    int len = 0;

                    len += snprintf(payload + len, sizeof(payload) - len,
                                    "{\"device_id\":%d,", dev->device_id);
                    len += snprintf(payload + len, sizeof(payload) - len,
                                    "\"device_name\":\"%s\",", dev->name);
                    len += snprintf(payload + len, sizeof(payload) - len,
                                    "\"timestamp\":%u,",
                                    (unsigned)time(NULL));
                    len += snprintf(payload + len, sizeof(payload) - len,
                                    "\"data\":{");

                    for (int j = 0; j < dev->point_count; j++) {
                        data_point_t *pt = &dev->points[j];
                        len += snprintf(payload + len, sizeof(payload) - len,
                                        "\"%s\":{\"value\":%.2f,",
                                        pt->name, pt->value);
                        len += snprintf(payload + len, sizeof(payload) - len,
                                        "\"unit\":\"%s\",", pt->unit);
                        len += snprintf(payload + len, sizeof(payload) - len,
                                        "\"alarm_high\":%s,",
                                        pt->alarm_high ? "true" : "false");
                        len += snprintf(payload + len, sizeof(payload) - len,
                                        "\"alarm_low\":%s}",
                                        pt->alarm_low ? "true" : "false");
                        if (j < dev->point_count - 1) {
                            payload[len++] = ',';
                        }
                    }

                    len += snprintf(payload + len, sizeof(payload) - len, "}}");

                    /* 发布 */
                    char topic[256];
                    snprintf(topic, sizeof(topic), "%s/device/%d/data",
                             g_gateway.config.mqtt.topic_prefix, dev->device_id);

                    mqtt_publish(&mqtt, topic, payload, len);
                }
            }
        }

        /* 等待下次发布 */
        usleep(g_gateway.config.mqtt.publish_interval * 1000);
    }

    mqtt_disconnect(&mqtt);
    printf("MQTT线程退出\n");
    return NULL;
}

/**
 * @brief 信号处理
 */
static void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n接收到信号 %d,正在关闭...\n", sig);
        g_gateway.running = false;
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    const char *config_file = argc > 1 ? argv[1] : "gateway.conf";

    print_banner();

    /* 初始化 */
    memset(&g_gateway, 0, sizeof(gateway_context_t));
    g_gateway.running = true;
    g_gateway.start_time = time(NULL);

    /* 信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 加载配置 */
    if (load_config(config_file) < 0) {
        fprintf(stderr, "加载配置失败\n");
        return 1;
    }

    /* 初始化设备 */
    if (init_devices() <= 0) {
        fprintf(stderr, "没有设备成功初始化\n");
        cleanup_devices();
        return 1;
    }

    /* 创建扫描线程 */
    if (pthread_create(&g_gateway.scan_thread, NULL,
                       scan_thread_func, NULL) != 0) {
        fprintf(stderr, "创建扫描线程失败\n");
        cleanup_devices();
        return 1;
    }

    /* 创建MQTT线程 */
    if (pthread_create(&g_gateway.mqtt_thread, NULL,
                       mqtt_thread_func, NULL) != 0) {
        fprintf(stderr, "创建MQTT线程失败\n");
    }

    printf("\n网关运行中... 按Ctrl+C停止\n\n");

    /* 主循环 - 显示统计信息 */
    while (g_gateway.running) {
        sleep(10);

        printf("--- 运行统计 ---\n");
        printf("运行时间: %ld 秒\n", (long)(time(NULL) - g_gateway.start_time));
        printf("总读取次数: %lu\n", (unsigned long)g_gateway.total_reads);
        printf("总错误次数: %lu\n", (unsigned long)g_gateway.total_errors);

        for (int i = 0; i < g_gateway.device_count; i++) {
            device_config_t *dev = &g_gateway.devices[i];
            const char *status_str =
                dev->status == DEV_STATUS_ONLINE ? "在线" :
                dev->status == DEV_STATUS_ERROR ? "错误" : "离线";

            printf("设备 %d (%s): %s, 读取: %lu, 错误: %lu\n",
                   dev->device_id, dev->name, status_str,
                   (unsigned long)dev->read_count,
                   (unsigned long)dev->read_errors);

            /* 显示数据点值 */
            pthread_mutex_lock(&dev->lock);
            for (int j = 0; j < dev->point_count; j++) {
                data_point_t *pt = &dev->points[j];
                printf("  %s: %.2f %s%s%s\n",
                       pt->name, pt->value, pt->unit,
                       pt->alarm_high ? " [高报警!]" : "",
                       pt->alarm_low ? " [低报警!]" : "");
            }
            pthread_mutex_unlock(&dev->lock);
        }
        printf("\n");
    }

    /* 等待线程结束 */
    pthread_join(g_gateway.scan_thread, NULL);
    pthread_join(g_gateway.mqtt_thread, NULL);

    /* 清理 */
    cleanup_devices();

    printf("网关已停止\n");
    return 0;
}
```

### 11.3 Makefile编译脚本

```makefile
# 工业多协议网关 Makefile

---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
CC = gcc
CFLAGS = -Wall -O2 -pthread -D_GNU_SOURCE
LDFLAGS = -lpthread -lm

# 目标
TARGET = protocol_gateway

# 源文件
SOURCES = \
    protocol_gateway.c \
    serial_port.c \
    modbus_rtu.c \
    modbus_tcp.c \
    can_bus.c \
    canopen_sdo.c \
    mqtt_client.c \
    data_cache.c

# 对象文件
OBJECTS = $(SOURCES:.c=.o)

# 头文件
HEADERS = \
    serial_port.h \
    modbus_rtu.h \
    modbus_tcp.h \
    can_bus.h \
    canopen_sdo.h \
    mqtt_client.h \
    data_cache.h

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(OBJECTS)
 $(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 编译
%.o: %.c $(HEADERS)
 $(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
 rm -f $(OBJECTS) $(TARGET)

# 安装
install: $(TARGET)
 install -m 755 $(TARGET) /usr/local/bin/
 install -m 644 gateway.conf /etc/

# 运行测试
test: $(TARGET)
 ./$(TARGET) gateway.conf

.PHONY: all clean install test
```

---

## 12. 总结与最佳实践

### 12.1 协议选择指南

```
┌─────────────────────────────────────────────────────────────┐
│                    工业通信协议选择指南                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  应用场景                    | 推荐协议                     │
│  ─────────────────────────────────────────────────────────  │
│  简单传感器网络(温度/压力)   | Modbus RTU (RS-485)          │
│  复杂运动控制                | EtherCAT, SERCOS III         │
│  跨车间数据交换              | Modbus TCP, EtherNet/IP      │
│  大型PLC系统                 | Profinet, EtherNet/IP        │
│  机器人/伺服控制             | EtherCAT, POWERLINK          │
│  过程自动化(化工/石油)       | Profibus PA, Foundation Fldbus│
│  车载电子                    | CAN, CANopen                 │
│  物联网/云连接               | MQTT over 4G/5G/WiFi         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 12.2 开发最佳实践

1. **错误处理**: 所有通信操作都应有完善的超时和重试机制
2. **日志记录**: 记录关键操作和错误，便于故障排查
3. **线程安全**: 多线程访问共享资源时使用互斥锁
4. **资源管理**: 使用RAII模式确保资源正确释放
5. **协议测试**: 使用协议分析仪验证通信正确性
6. **电磁兼容**: 工业环境注意屏蔽和接地

### 12.3 参考资料

- Modbus Protocol Specification v1.1b3
- CAN Specification 2.0B
- CiA 301 CANopen Application Layer
- EtherCAT Technology Group Specifications
- IEC 61158 (Industrial Ethernet Standards)

---

*文档版本: 1.0.0*
*最后更新: 2024*
*作者: C语言知识库*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
