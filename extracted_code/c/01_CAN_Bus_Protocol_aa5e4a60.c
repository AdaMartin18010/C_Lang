/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\01_CAN_Bus_Protocol.md
 * Line: 177
 * Language: c
 * Block ID: aa5e4a60
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
