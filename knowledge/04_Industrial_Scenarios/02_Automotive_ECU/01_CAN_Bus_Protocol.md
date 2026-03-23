# CAN总线协议C语言实现

## 目录

- [CAN总线协议C语言实现](#can总线协议c语言实现)
  - [目录](#目录)
  - [概述](#概述)
    - [CAN主要特性](#can主要特性)
  - [CAN协议帧结构详解](#can协议帧结构详解)
    - [标准帧格式（CAN 2.0A）](#标准帧格式can-20a)
    - [扩展帧格式（CAN 2.0B）](#扩展帧格式can-20b)
    - [帧类型说明](#帧类型说明)
  - [位定时和波特率计算](#位定时和波特率计算)
    - [位时间组成](#位时间组成)
    - [波特率计算公式](#波特率计算公式)
    - [采样点计算](#采样点计算)
  - [SocketCAN编程接口](#socketcan编程接口)
    - [SocketCAN是Linux内核提供的CAN协议实现，将CAN设备抽象为网络接口](#socketcan是linux内核提供的can协议实现将can设备抽象为网络接口)
    - [核心数据结构](#核心数据结构)
    - [常用ioctl命令](#常用ioctl命令)
  - [C代码示例](#c代码示例)
    - [完整CAN通信实现](#完整can通信实现)
    - [编译与运行](#编译与运行)
    - [预期输出](#预期输出)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 概述

CAN（Controller Area Network，控制器局域网）是由Bosch公司于1986年开发的串行通信协议，最初用于汽车电子系统，现广泛应用于工业自动化、航空航天、医疗设备等领域。

### CAN主要特性

| 特性 | 说明 |
|------|------|
| 传输介质 | 双绞线（CAN_H / CAN_L） |
| 拓扑结构 | 总线型，多主架构 |
| 最大节点 | 理论110个，实际通常<32个 |
| 传输速率 | 最高1 Mbps（ISO 11898） |
| 传输距离 | 与速率相关，40m@1Mbps，1km@50kbps |
| 仲裁机制 | CSMA/CD+AMP（非破坏性仲裁） |

## CAN协议帧结构详解

### 标准帧格式（CAN 2.0A）

```
标准帧（11位标识符）- 最大长度：108位

┌────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐
│  起始  │ 仲裁场 │ 控制场 │ 数据场 │ CRC场  │  ACK场 │  结束  │  帧间  │
│  位(SOF)│(11+1位)│  (6位) │(0-64位)│(16位)  │  (2位) │(7位)   │ 间隔   │
└────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘

仲裁场结构：
┌───────────────────────────────┬────────┐
│      标识符（11位）            │  RTR   │
│  ID10 ID9 ... ID2 ID1 ID0     │ (1位)  │
└───────────────────────────────┴────────┘
```

### 扩展帧格式（CAN 2.0B）

```
扩展帧（29位标识符）- 最大长度：128位

┌─────┬───────────────────────────────────────┬─────┬─────┬─────┬────────┐
│ SOF │ 仲裁场（29位标识符 + SRR + IDE + RTR）│控制场│数据场│ CRC │  ACK   │
└─────┴───────────────────────────────────────┴─────┴─────┴─────┴────────┘

仲裁场结构：
┌──────────┬────┬────┬──────────┬────┬────┐
│ 基础ID    │SRR │ IDE│ 扩展ID    │RTR │ r1 │
│ (11位)    │(1) │ (1)│ (18位)    │(1) │(1) │
└──────────┴────┴────┴──────────┴────┴────┘
```

### 帧类型说明

| 帧类型 | 用途 | 数据长度 |
|--------|------|----------|
| 数据帧 | 传输数据 | 0-8字节 |
| 远程帧 | 请求数据 | 0字节 |
| 错误帧 | 错误通知 | - |
| 过载帧 | 延迟下一帧 | - |

## 位定时和波特率计算

### 位时间组成

```
┌─────────┬─────────┬─────────┬─────────┐
│  同步段  │ 传播段   │ 相位段1  │ 相位段2  │
│  (1Tq)  │(1-8Tq)  │(1-8Tq)  │(1-8Tq)  │
└─────────┴──────────┴─────────┴─────────┘

总位时间 = 1 + PROP_SEG + PHASE_SEG1 + PHASE_SEG2 (单位: Tq)
```

### 波特率计算公式

```
波特率 = CAN时钟频率 / (预分频器 × 位时间Tq数)

示例：8MHz时钟，500kbps
- 目标Tq数 = 8MHz / (预分频器 × 500k)
- 取预分频器=1，则Tq数 = 16
- 位时间配置：1 + 4 + 8 + 3 = 16 Tq
```

### 采样点计算

```
采样点位置 = (同步段 + 传播段 + 相位段1) / 总Tq数

推荐采样点：
- 高速CAN (>500kbps): 75-80%
- 低速CAN (<500kbps): 85-90%
```

## SocketCAN编程接口

### SocketCAN是Linux内核提供的CAN协议实现，将CAN设备抽象为网络接口

### 核心数据结构

```c
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

/* CAN帧结构体 - 定义在linux/can.h */
struct can_frame {
    canid_t can_id;  /* 32位CAN ID + EFF/RTR/ERR标志 */
    __u8    can_dlc; /* 数据长度码 0-8 */
    __u8    __pad;   /* 填充 */
    __u8    __res0;  /* 保留 */
    __u8    __res1;  /* 保留 */
    __u8    data[8] __attribute__((aligned(8))); /* 数据 */
};

/* CAN ID标志位 */
#define CAN_EFF_FLAG 0x80000000U /* 扩展帧标志 */
#define CAN_RTR_FLAG 0x40000000U /* 远程帧标志 */
#define CAN_ERR_FLAG 0x20000000U /* 错误帧标志 */
#define CAN_SFF_MASK 0x000007FFU /* 标准帧ID掩码 */
#define CAN_EFF_MASK 0x1FFFFFFFU /* 扩展帧ID掩码 */
```

### 常用ioctl命令

```c
/* 设置CAN接口参数 */
#define SIOCGIFINDEX    /* 获取接口索引 */
#define SIOCSCANBITRATE /* 设置比特率 */
#define SIOCSCANMODE    /* 设置模式 */

/* CAN模式 */
enum can_mode {
    CAN_MODE_STOP = 0,      /* 停止模式 */
    CAN_MODE_START,         /* 启动模式 */
    CAN_MODE_SLEEP,         /* 休眠模式 */
    CAN_MODE_LOOPBACK,      /* 环回模式 */
    CAN_MODE_LISTENONLY,    /* 仅监听模式 */
    CAN_MODE_ERROR_ACTIVE,  /* 错误主动模式 */
    CAN_MODE_ERROR_PASSIVE  /* 错误被动模式 */
};
```

## C代码示例

### 完整CAN通信实现

```c
/*
 * CAN总线协议C语言实现
 * 支持标准帧/扩展帧发送接收
 * 遵循C17标准和MISRA规范
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* 模拟SocketCAN环境（非Linux平台时使用） */
#ifdef __linux__
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#else
/* 模拟定义用于编译测试 */
typedef uint32_t canid_t;

struct can_frame {
    canid_t can_id;
    uint8_t can_dlc;
    uint8_t __pad;
    uint8_t __res0;
    uint8_t __res1;
    uint8_t data[8];
};

#define CAN_EFF_FLAG 0x80000000U
#define CAN_RTR_FLAG 0x40000000U
#define CAN_ERR_FLAG 0x20000000U
#define CAN_SFF_MASK 0x000007FFU
#define CAN_EFF_MASK 0x1FFFFFFFU
#define PF_CAN 29
#define CAN_RAW 1
#define SIOCGIFINDEX 0x8933

struct sockaddr_can {
    uint16_t can_family;
    int32_t  can_ifindex;
    uint32_t can_addr;
};

struct ifreq {
    char ifr_name[16];
    union {
        int ifr_ifindex;
    };
};
#endif /* __linux__ */

/* MISRA C:2012 合规常量定义 */
#define CAN_MAX_DLC          (8U)
#define CAN_MAX_DATA_LEN     (8U)
#define CAN_SFF_ID_BITS      (11U)
#define CAN_EFF_ID_BITS      (29U)
#define CAN_MAX_FRAME_SIZE   (16U)
#define CAN_TIMEOUT_MS       (1000U)

/* CAN波特率预定义 */
typedef enum {
    CAN_BITRATE_10K  = 10000U,
    CAN_BITRATE_20K  = 20000U,
    CAN_BITRATE_50K  = 50000U,
    CAN_BITRATE_100K = 100000U,
    CAN_BITRATE_125K = 125000U,
    CAN_BITRATE_250K = 250000U,
    CAN_BITRATE_500K = 500000U,
    CAN_BITRATE_800K = 800000U,
    CAN_BITRATE_1M   = 1000000U
} can_bitrate_t;

/* CAN错误码定义 */
typedef enum {
    CAN_ERR_NONE = 0,
    CAN_ERR_SOCKET,
    CAN_ERR_INTERFACE,
    CAN_ERR_BIND,
    CAN_ERR_PARAM,
    CAN_ERR_TIMEOUT,
    CAN_ERR_BUS_OFF,
    CAN_ERR_OVERFLOW
} can_error_t;

/* CAN设备句柄 */
typedef struct {
    int32_t sockfd;
    int32_t ifindex;
    char ifname[16];
    can_bitrate_t bitrate;
    bool is_extended;
    bool is_running;
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t err_count;
} can_handle_t;

/* CAN总线统计 */
typedef struct {
    uint32_t tx_frames;
    uint32_t rx_frames;
    uint32_t tx_errors;
    uint32_t rx_errors;
    uint32_t bus_off_count;
    uint32_t error_warning;
    uint32_t error_passive;
} can_stats_t;

/* 函数声明 */
static can_error_t can_init(can_handle_t *handle, const char *ifname);
static void can_deinit(can_handle_t *handle);
static can_error_t can_send(can_handle_t *handle, const struct can_frame *frame);
static can_error_t can_receive(can_handle_t *handle, struct can_frame *frame,
                                uint32_t timeout_ms);
static void can_print_frame(const struct can_frame *frame, bool is_tx);
static const char* can_error_string(can_error_t err);

/* 初始化CAN设备 */
static can_error_t can_init(can_handle_t *handle, const char *ifname)
{
    if ((handle == NULL) || (ifname == NULL)) {
        return CAN_ERR_PARAM;
    }

    (void)memset(handle, 0, sizeof(can_handle_t));

    /* 复制接口名称 */
    (void)strncpy(handle->ifname, ifname, sizeof(handle->ifname) - 1U);
    handle->ifname[sizeof(handle->ifname) - 1U] = '\0';

#ifdef __linux__
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* 创建CAN socket */
    handle->sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (handle->sockfd < 0) {
        perror("socket");
        return CAN_ERR_SOCKET;
    }

    /* 获取接口索引 */
    (void)strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1U);
    if (ioctl(handle->sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        (void)close(handle->sockfd);
        return CAN_ERR_INTERFACE;
    }
    handle->ifindex = ifr.ifr_ifindex;

    /* 绑定socket */
    (void)memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = handle->ifindex;

    if (bind(handle->sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        (void)close(handle->sockfd);
        return CAN_ERR_BIND;
    }

    /* 设置非阻塞模式 */
    int flags = fcntl(handle->sockfd, F_GETFL, 0);
    (void)fcntl(handle->sockfd, F_SETFL, flags | O_NONBLOCK);
#else
    /* 模拟模式 */
    handle->sockfd = 1;  /* 模拟有效描述符 */
    handle->ifindex = 1;
    printf("[模拟模式] CAN接口 %s 初始化成功\n", ifname);
#endif

    handle->is_running = true;
    handle->bitrate = CAN_BITRATE_500K;

    return CAN_ERR_NONE;
}

/* 关闭CAN设备 */
static void can_deinit(can_handle_t *handle)
{
    if (handle == NULL) {
        return;
    }

    handle->is_running = false;

#ifdef __linux__
    if (handle->sockfd >= 0) {
        (void)close(handle->sockfd);
        handle->sockfd = -1;
    }
#else
    printf("[模拟模式] CAN接口 %s 已关闭\n", handle->ifname);
#endif
}

/* 发送CAN帧 */
static can_error_t can_send(can_handle_t *handle, const struct can_frame *frame)
{
    if ((handle == NULL) || (frame == NULL)) {
        return CAN_ERR_PARAM;
    }

    if (!handle->is_running) {
        return CAN_ERR_BUS_OFF;
    }

    /* 验证DLC */
    if (frame->can_dlc > CAN_MAX_DLC) {
        return CAN_ERR_PARAM;
    }

#ifdef __linux__
    ssize_t nbytes = write(handle->sockfd, frame, sizeof(struct can_frame));
    if (nbytes != (ssize_t)sizeof(struct can_frame)) {
        return CAN_ERR_SOCKET;
    }
#else
    /* 模拟发送 */
    (void)usleep(1000);  /* 模拟传输延迟 */
#endif

    handle->tx_count++;

    return CAN_ERR_NONE;
}

/* 接收CAN帧 */
static can_error_t can_receive(can_handle_t *handle, struct can_frame *frame,
                                uint32_t timeout_ms)
{
    if ((handle == NULL) || (frame == NULL)) {
        return CAN_ERR_PARAM;
    }

    if (!handle->is_running) {
        return CAN_ERR_BUS_OFF;
    }

#ifdef __linux__
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(handle->sockfd, &readfds);

    tv.tv_sec = (time_t)(timeout_ms / 1000U);
    tv.tv_usec = (suseconds_t)((timeout_ms % 1000U) * 1000U);

    int ret = select(handle->sockfd + 1, &readfds, NULL, NULL, &tv);
    if (ret < 0) {
        return CAN_ERR_SOCKET;
    } else if (ret == 0) {
        return CAN_ERR_TIMEOUT;
    }

    ssize_t nbytes = read(handle->sockfd, frame, sizeof(struct can_frame));
    if (nbytes < 0) {
        return CAN_ERR_SOCKET;
    } else if ((size_t)nbytes != sizeof(struct can_frame)) {
        return CAN_ERR_PARAM;
    }
#else
    /* 模拟接收 */
    (void)usleep(timeout_ms * 1000U);
    return CAN_ERR_TIMEOUT;
#endif

    handle->rx_count++;

    return CAN_ERR_NONE;
}

/* 打印CAN帧 */
static void can_print_frame(const struct can_frame *frame, bool is_tx)
{
    if (frame == NULL) {
        return;
    }

    const char *direction = is_tx ? "TX" : "RX";

    /* 判断帧类型 */
    const bool is_extended = (frame->can_id & CAN_EFF_FLAG) != 0U;
    const bool is_remote = (frame->can_id & CAN_RTR_FLAG) != 0U;
    const bool is_error = (frame->can_id & CAN_ERR_FLAG) != 0U;

    /* 提取ID */
    const uint32_t can_id = frame->can_id &
                           (is_extended ? CAN_EFF_MASK : CAN_SFF_MASK);

    printf("[%s] ", direction);

    if (is_error) {
        printf("ERROR FRAME  ID=0x%08X", can_id);
    } else {
        /* 打印ID */
        if (is_extended) {
            printf("EFF ID=0x%08X", can_id);
        } else {
            printf("SFF ID=0x%03X", can_id);
        }

        /* 打印类型 */
        if (is_remote) {
            printf(" [RTR]");
        }

        /* 打印数据 */
        if (!is_remote && (frame->can_dlc > 0U)) {
            printf(" DLC=%u Data=", frame->can_dlc);
            for (uint8_t i = 0U; i < frame->can_dlc; i++) {
                printf("%02X ", frame->data[i]);
            }
        }
    }

    printf("\n");
}

/* 错误码转字符串 */
static const char* can_error_string(can_error_t err)
{
    switch (err) {
        case CAN_ERR_NONE:     return "无错误";
        case CAN_ERR_SOCKET:   return "Socket错误";
        case CAN_ERR_INTERFACE:return "接口错误";
        case CAN_ERR_BIND:     return "绑定错误";
        case CAN_ERR_PARAM:    return "参数错误";
        case CAN_ERR_TIMEOUT:  return "超时";
        case CAN_ERR_BUS_OFF:  return "总线关闭";
        case CAN_ERR_OVERFLOW: return "溢出";
        default:               return "未知错误";
    }
}

/* 构建标准数据帧 */
static void can_build_sff_frame(struct can_frame *frame,
                                 uint16_t id,
                                 const uint8_t *data,
                                 uint8_t len)
{
    if ((frame == NULL) || ((data == NULL) && (len > 0U))) {
        return;
    }

    (void)memset(frame, 0, sizeof(struct can_frame));

    /* 标准帧ID（11位） */
    frame->can_id = (uint32_t)(id & 0x7FFU);
    frame->can_dlc = (len > CAN_MAX_DLC) ? CAN_MAX_DLC : len;

    if ((data != NULL) && (len > 0U)) {
        (void)memcpy(frame->data, data, frame->can_dlc);
    }
}

/* 构建扩展数据帧 */
static void can_build_eff_frame(struct can_frame *frame,
                                 uint32_t id,
                                 const uint8_t *data,
                                 uint8_t len)
{
    if ((frame == NULL) || ((data == NULL) && (len > 0U))) {
        return;
    }

    (void)memset(frame, 0, sizeof(struct can_frame));

    /* 扩展帧ID（29位）+ 标志 */
    frame->can_id = (id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    frame->can_dlc = (len > CAN_MAX_DLC) ? CAN_MAX_DLC : len;

    if ((data != NULL) && (len > 0U)) {
        (void)memcpy(frame->data, data, frame->can_dlc);
    }
}

/* 构建远程帧 */
static void can_build_rtr_frame(struct can_frame *frame,
                                 uint16_t id,
                                 uint8_t dlc,
                                 bool is_extended)
{
    if (frame == NULL) {
        return;
    }

    (void)memset(frame, 0, sizeof(struct can_frame));

    if (is_extended) {
        frame->can_id = ((uint32_t)id & CAN_EFF_MASK) | CAN_EFF_FLAG | CAN_RTR_FLAG;
    } else {
        frame->can_id = ((uint32_t)id & CAN_SFF_MASK) | CAN_RTR_FLAG;
    }
    frame->can_dlc = dlc;
}

/* ==================== 主函数 ==================== */

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("=== CAN总线协议演示程序 ===\n\n");

    can_handle_t can;
    can_error_t err;

    /* 初始化CAN接口 */
    const char *ifname = (argc > 1) ? argv[1] : "vcan0";
    printf("初始化CAN接口: %s\n", ifname);

    err = can_init(&can, ifname);
    if (err != CAN_ERR_NONE) {
        printf("初始化失败: %s\n", can_error_string(err));
        return 1;
    }
    printf("初始化成功!\n\n");

    /* ============ 测试1: 发送标准帧 ============ */
    printf("测试1: 发送标准数据帧 (ID=0x123)\n");

    struct can_frame tx_frame;
    uint8_t data1[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};

    can_build_sff_frame(&tx_frame, 0x123U, data1, 8U);
    can_print_frame(&tx_frame, true);

    err = can_send(&can, &tx_frame);
    if (err != CAN_ERR_NONE) {
        printf("发送失败: %s\n", can_error_string(err));
    } else {
        printf("发送成功!\n");
    }

    /* ============ 测试2: 发送扩展帧 ============ */
    printf("\n测试2: 发送扩展数据帧 (ID=0x18FF1234)\n");

    uint8_t data2[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    can_build_eff_frame(&tx_frame, 0x18FF1234U, data2, 4U);
    can_print_frame(&tx_frame, true);

    err = can_send(&can, &tx_frame);
    if (err != CAN_ERR_NONE) {
        printf("发送失败: %s\n", can_error_string(err));
    } else {
        printf("发送成功!\n");
    }

    /* ============ 测试3: 发送远程帧 ============ */
    printf("\n测试3: 发送远程帧 (ID=0x456, 请求8字节)\n");

    can_build_rtr_frame(&tx_frame, 0x456U, 8U, false);
    can_print_frame(&tx_frame, true);

    err = can_send(&can, &tx_frame);
    if (err != CAN_ERR_NONE) {
        printf("发送失败: %s\n", can_error_string(err));
    } else {
        printf("发送成功!\n");
    }

    /* ============ 测试4: 模拟ECU通信 ============ */
    printf("\n测试4: 模拟ECU周期性发送\n");
    printf("发送3帧引擎数据...\n");

    for (int i = 0; i < 3; i++) {
        uint8_t engine_data[8];
        engine_data[0] = (uint8_t)(i * 10);      /* 引擎转速低字节 */
        engine_data[1] = 0x00;                   /* 引擎转速高字节 */
        engine_data[2] = (uint8_t)(50 + i * 5);  /* 冷却液温度 */
        engine_data[3] = (uint8_t)(90 + i);      /* 油门位置 */
        engine_data[4] = 0x00;
        engine_data[5] = 0x00;
        engine_data[6] = 0x00;
        engine_data[7] = (uint8_t)i;             /* 帧计数器 */

        can_build_sff_frame(&tx_frame, 0x0C6U, engine_data, 8U);
        can_print_frame(&tx_frame, true);

        err = can_send(&can, &tx_frame);
        if (err != CAN_ERR_NONE) {
            printf("  第%d帧发送失败\n", i + 1);
        }

        (void)usleep(100000);  /* 100ms间隔 */
    }

    /* ============ 测试5: 接收帧 ============ */
    printf("\n测试5: 尝试接收帧 (超时1秒)\n");

    struct can_frame rx_frame;
    err = can_receive(&can, &rx_frame, 1000U);

    if (err == CAN_ERR_TIMEOUT) {
        printf("接收超时（正常，无其他节点发送）\n");
    } else if (err == CAN_ERR_NONE) {
        can_print_frame(&rx_frame, false);
    } else {
        printf("接收错误: %s\n", can_error_string(err));
    }

    /* ============ 统计信息 ============ */
    printf("\n=== 通信统计 ===\n");
    printf("发送帧数: %u\n", can.tx_count);
    printf("接收帧数: %u\n", can.rx_count);
    printf("错误计数: %u\n", can.err_count);
    printf("接口名称: %s\n", can.ifname);
    printf("接口索引: %d\n", can.ifindex);

    /* 清理 */
    can_deinit(&can);
    printf("\n程序结束\n");

    return 0;
}
```

### 编译与运行

```bash
# Linux平台编译
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
gcc -std=c17 -Wall -Wextra -D_GNU_SOURCE -o can_demo can_demo.c

# 运行（需要root权限访问CAN接口）
sudo ./can_demo can0

# 或使用虚拟CAN接口测试
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
./can_demo vcan0
```

### 预期输出

```
=== CAN总线协议演示程序 ===

初始化CAN接口: vcan0
初始化成功!

测试1: 发送标准数据帧 (ID=0x123)
[TX] SFF ID=0x123 DLC=8 Data=01 02 03 04 05 06 07 08
发送成功!

测试2: 发送扩展数据帧 (ID=0x18FF1234)
[TX] EFF ID=0x18FF1234 DLC=4 Data=AA BB CC DD
发送成功!

...

=== 通信统计 ===
发送帧数: 6
接收帧数: 0
错误计数: 0
```

## 总结

CAN总线协议的关键技术要点：

| 方面 | 要点 |
|------|------|
| 帧格式 | 标准帧11位ID，扩展帧29位ID |
| 仲裁机制 | 按ID优先级非破坏性仲裁 |
| 位定时 | 同步段+传播段+相位段1+相位段2 |
| 错误处理 | 自动重发，错误计数器管理节点状态 |
| Linux支持 | SocketCAN提供标准socket接口 |

**开发注意事项**：

1. 严格检查DLC范围（0-8）
2. 区分标准帧和扩展帧的ID掩码
3. 处理总线关闭（Bus Off）恢复
4. 注意字节序（CAN使用大端序）
5. 遵循MISRA C规范进行安全关键开发

本实现提供了完整的CAN通信框架，可作为汽车ECU开发的基础模块。


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
