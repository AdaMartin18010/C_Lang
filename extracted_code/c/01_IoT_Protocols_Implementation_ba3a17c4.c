/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 3319
 * Language: c
 * Block ID: ba3a17c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file can_driver.c
 * @brief CAN驱动实现 - 基于SocketCAN (Linux) 或虚拟实现
 */

#include "can_driver.h"
#include <string.h>
#include <stdio.h>

#ifdef __linux__
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <linux/can.h>
    #include <linux/can/raw.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

/* 常用波特率配置 */
const can_bit_timing_t can_timing_125k = {
    .bitrate = 125000,
    .prescaler = 8,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_250k = {
    .bitrate = 250000,
    .prescaler = 4,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_500k = {
    .bitrate = 500000,
    .prescaler = 2,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_1m = {
    .bitrate = 1000000,
    .prescaler = 1,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

/* 设备结构 */
struct can_device {
    uint8_t controller_id;
    can_bit_timing_t timing;
    can_bus_state_t state;

#ifdef __linux__
    int socket_fd;
    struct sockaddr_can addr;
#endif

    /* 回调 */
    can_rx_callback_t rx_callback;
    void *rx_user_data;
    can_error_callback_t error_callback;
    void *error_user_data;

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
};

/* DLC到长度转换 */
uint16_t can_dlc_to_len(uint8_t dlc) {
    static const uint8_t dlc_table[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    if (dlc <= 8) return dlc_table[dlc];
    return 8;
}

/* 长度到DLC转换 */
uint8_t can_len_to_dlc(uint16_t len) {
    if (len > 8) len = 8;
    return (uint8_t)len;
}

/* 验证ID有效性 */
bool can_is_valid_id(uint32_t id, can_id_type_t type) {
    if (type == CAN_ID_STANDARD) {
        return id <= CAN_MAX_STD_ID;
    } else {
        return id <= CAN_MAX_EXT_ID;
    }
}

/* 初始化CAN帧 */
void can_frame_init(can_frame_t *frame, uint32_t id, can_id_type_t id_type,
                     const uint8_t *data, uint8_t len) {
    memset(frame, 0, sizeof(can_frame_t));
    frame->id = id;
    frame->id_type = id_type;
    frame->frame_type = CAN_FRAME_DATA;
    frame->rtr = false;

    if (len > CAN_MAX_DLC) len = CAN_MAX_DLC;
    frame->dlc = len;

    if (data != NULL && len > 0) {
        memcpy(frame->data, data, len);
    }
}

/* 初始化CAN设备 */
can_device_t* can_init(uint8_t controller_id, const can_bit_timing_t *timing) {
#ifdef __linux__
    can_device_t *dev = (can_device_t *)malloc(sizeof(can_device_t));
    if (dev == NULL) return NULL;

    memset(dev, 0, sizeof(can_device_t));
    dev->controller_id = controller_id;

    if (timing != NULL) {
        dev->timing = *timing;
    } else {
        dev->timing = can_timing_500k;
    }

    /* 创建SocketCAN socket */
    dev->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (dev->socket_fd < 0) {
        free(dev);
        return NULL;
    }

    /* 获取接口索引 */
    struct ifreq ifr;
    snprintf(ifr.ifr_name, IFNAMSIZ, "can%d", controller_id);
    if (ioctl(dev->socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        close(dev->socket_fd);
        free(dev);
        return NULL;
    }

    dev->addr.can_family = AF_CAN;
    dev->addr.can_ifindex = ifr.ifr_ifindex;

    /* 绑定 */
    if (bind(dev->socket_fd, (struct sockaddr *)&dev->addr, sizeof(dev->addr)) < 0) {
        close(dev->socket_fd);
        free(dev);
        return NULL;
    }

    /* 设置为非阻塞 */
    int flags = fcntl(dev->socket_fd, F_GETFL, 0);
    fcntl(dev->socket_fd, F_SETFL, flags | O_NONBLOCK);

    dev->state = CAN_BUS_ERROR_ACTIVE;
    return dev;
#else
    /* 非Linux平台返回虚拟设备 */
    can_device_t *dev = (can_device_t *)malloc(sizeof(can_device_t));
    if (dev == NULL) return NULL;
    memset(dev, 0, sizeof(can_device_t));
    dev->controller_id = controller_id;
    if (timing != NULL) {
        dev->timing = *timing;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    return dev;
#endif
}

/* 关闭CAN设备 */
void can_deinit(can_device_t *dev) {
    if (dev == NULL) return;

#ifdef __linux__
    if (dev->socket_fd >= 0) {
        close(dev->socket_fd);
    }
#endif
    free(dev);
}

/* 发送CAN帧 */
int can_send(can_device_t *dev, const can_frame_t *frame, uint32_t timeout_ms) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }

    if (!can_is_valid_id(frame->id, frame->id_type)) {
        return CAN_ERR_INVALID_PARAM;
    }

#ifdef __linux__
    struct can_frame sock_frame;
    memset(&sock_frame, 0, sizeof(sock_frame));

    /* 设置ID */
    if (frame->id_type == CAN_ID_EXTENDED) {
        sock_frame.can_id = frame->id | CAN_EFF_FLAG;
    } else {
        sock_frame.can_id = frame->id;
    }

    if (frame->rtr) {
        sock_frame.can_id |= CAN_RTR_FLAG;
    }

    sock_frame.can_dlc = frame->dlc;
    memcpy(sock_frame.data, frame->data, frame->dlc);

    ssize_t sent = write(dev->socket_fd, &sock_frame, sizeof(sock_frame));
    if (sent != sizeof(sock_frame)) {
        if (errno == ENOBUFS) {
            return CAN_ERR_BUSY;
        }
        return CAN_ERR_BUS_OFF;
    }

    dev->tx_count++;
    return CAN_ERR_OK;
#else
    /* 虚拟实现 */
    printf("CAN TX: ID=0x%08X, DLC=%d, Data=", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        printf("%02X ", frame->data[i]);
    }
    printf("\n");
    dev->tx_count++;
    return CAN_ERR_OK;
#endif
}

/* 接收CAN帧 */
int can_receive(can_device_t *dev, can_frame_t *frame, uint32_t timeout_ms) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }

#ifdef __linux__
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(dev->socket_fd, &rfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(dev->socket_fd + 1, &rfds, NULL, NULL, &tv);
    if (ret < 0) {
        return CAN_ERR_INVALID_PARAM;
    }
    if (ret == 0) {
        return CAN_ERR_TIMEOUT;
    }

    return can_try_receive(dev, frame);
#else
    (void)timeout_ms;
    return CAN_ERR_TIMEOUT;
#endif
}

/* 非阻塞接收 */
int can_try_receive(can_device_t *dev, can_frame_t *frame) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }

#ifdef __linux__
    struct can_frame sock_frame;
    ssize_t recvd = read(dev->socket_fd, &sock_frame, sizeof(sock_frame));

    if (recvd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return CAN_ERR_TIMEOUT;
        }
        return CAN_ERR_INVALID_PARAM;
    }

    if (recvd != sizeof(sock_frame)) {
        return CAN_ERR_INVALID_PARAM;
    }

    /* 转换帧 */
    memset(frame, 0, sizeof(can_frame_t));

    if (sock_frame.can_id & CAN_EFF_FLAG) {
        frame->id = sock_frame.can_id & CAN_EFF_MASK;
        frame->id_type = CAN_ID_EXTENDED;
    } else {
        frame->id = sock_frame.can_id & CAN_SFF_MASK;
        frame->id_type = CAN_ID_STANDARD;
    }

    frame->rtr = (sock_frame.can_id & CAN_RTR_FLAG) != 0;
    frame->dlc = sock_frame.can_dlc;
    memcpy(frame->data, sock_frame.data, sock_frame.can_dlc);
    frame->frame_type = CAN_FRAME_DATA;

    dev->rx_count++;

    /* 调用回调 */
    if (dev->rx_callback != NULL) {
        dev->rx_callback(frame, dev->rx_user_data);
    }

    return CAN_ERR_OK;
#else
    (void)frame;
    return CAN_ERR_TIMEOUT;
#endif
}

/* 设置滤波器 */
int can_set_filter(can_device_t *dev, uint8_t filter_idx, const can_filter_t *filter) {
#ifdef __linux__
    if (dev == NULL || filter == NULL || filter_idx >= 32) {
        return CAN_ERR_INVALID_PARAM;
    }

    struct can_filter sock_filter;
    sock_filter.can_id = filter->id;
    sock_filter.can_mask = filter->mask;

    if (filter->id_type == CAN_ID_EXTENDED) {
        sock_filter.can_id |= CAN_EFF_FLAG;
        sock_filter.can_mask |= CAN_EFF_FLAG;
    }

    if (!filter->enabled) {
        sock_filter.can_id = 0;
        sock_filter.can_mask = 0;
    }

    if (setsockopt(dev->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
                   &sock_filter, sizeof(sock_filter)) < 0) {
        return CAN_ERR_INVALID_PARAM;
    }

    return CAN_ERR_OK;
#else
    (void)dev;
    (void)filter_idx;
    (void)filter;
    return CAN_ERR_OK;
#endif
}

/* 注册接收回调 */
int can_register_rx_callback(can_device_t *dev, can_rx_callback_t callback, void *user_data) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }

    dev->rx_callback = callback;
    dev->rx_user_data = user_data;
    return CAN_ERR_OK;
}

/* 启动CAN */
int can_start(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    return CAN_ERR_OK;
}

/* 停止CAN */
int can_stop(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    return CAN_ERR_OK;
}

/* 重置CAN */
int can_reset(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    dev->tx_count = 0;
    dev->rx_count = 0;
    dev->error_count = 0;
    return CAN_ERR_OK;
}

/* 检查总线关闭状态 */
bool can_is_bus_off(can_device_t *dev) {
    if (dev == NULL) return false;
    return dev->state == CAN_BUS_OFF;
}
