/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 3290
 * Language: c
 * Block ID: a388dfbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file canopen_node.c
 * @brief CANopen节点实现
 * @details 使用SocketCAN实现CANopen协议通信
 *
 * 编译: gcc -o canopen_node canopen_node.c
 * 运行: sudo ./canopen_node (需要root权限访问CAN接口)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

/* CANopen Object Dictionary索引 */
#define OD_DEVICE_TYPE          0x1000
#define OD_ERROR_REGISTER       0x1001
#define OD_MANUFACTURER_STATUS  0x1002
#define OD_PRE_DEFINED_ERROR    0x1003
#define OD_COB_ID_SYNC          0x1005
#define OD_COMMUNICATION_CYCLE  0x1006
#define OD_MANUFACTURER_NAME    0x1018
#define OD_IDENTITY             0x1018
#define OD_STORE_PARAMETERS     0x1010
#define OD_RESTORE_PARAMETERS   0x1011

/* CANopen COB-ID定义 */
#define COB_ID_NMT              0x000
#define COB_ID_SYNC             0x080
#define COB_ID_EMCY_BASE        0x080
#define COB_ID_TPDO1_BASE       0x180
#define COB_ID_RPDO1_BASE       0x200
#define COB_ID_TPDO2_BASE       0x280
#define COB_ID_RPDO2_BASE       0x300
#define COB_ID_TPDO3_BASE       0x380
#define COB_ID_RPDO3_BASE       0x400
#define COB_ID_TPDO4_BASE       0x480
#define COB_ID_RPDO4_BASE       0x500
#define COB_ID_TSDO_BASE        0x580
#define COB_ID_RSDO_BASE        0x600
#define COB_ID_HEARTBEAT_BASE   0x700

/* NMT命令 */
#define NMT_CMD_START           0x01
#define NMT_CMD_STOP            0x02
#define NMT_CMD_PREOP           0x80
#define NMT_CMD_RESET_NODE      0x81
#define NMT_CMD_RESET_COMM      0x82

/* SDO命令字节 */
#define SDO_CCS_DOWNLOAD_INIT   1
#define SDO_CCS_DOWNLOAD_SEG    0
#define SDO_CCS_UPLOAD_INIT     2
#define SDO_CCS_UPLOAD_SEG      3
#define SDO_CCS_ABORT           4

#define SDO_SCS_UPLOAD_INIT     2
#define SDO_SCS_UPLOAD_SEG      0
#define SDO_SCS_DOWNLOAD_INIT   3
#define SDO_SCS_DOWNLOAD_SEG    1

/* 节点状态 */
typedef enum {
    NMT_STATE_BOOTUP = 0,
    NMT_STATE_STOPPED = 4,
    NMT_STATE_OPERATIONAL = 5,
    NMT_STATE_PREOPERATIONAL = 127
} canopen_nmt_state_t;

/* Object Dictionary条目 */
typedef struct {
    uint16_t index;
    uint8_t subindex;
    uint8_t access_type;    /* 0=RO, 1=WO, 2=RW, 3=CONST */
    uint8_t data_type;
    uint32_t default_value;
    uint32_t value;
    uint32_t min_value;
    uint32_t max_value;
    const char *description;
} od_entry_t;

/* CANopen节点上下文 */
typedef struct {
    int socket_fd;
    uint8_t node_id;
    canopen_nmt_state_t state;
    struct sockaddr_can addr;
    struct ifreq ifr;

    /* Object Dictionary */
    od_entry_t *od;
    uint16_t od_size;

    /* PDO配置 */
    uint32_t tpdo1_cob_id;
    uint32_t rpdo1_cob_id;
    uint16_t tpdo1_mapping[8];
    uint8_t tpdo1_mapping_count;

    /* 心跳 */
    uint16_t heartbeat_time;
    uint32_t last_heartbeat;

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
} canopen_node_t;

/* 默认Object Dictionary */
static od_entry_t default_od[] = {
    /* 设备信息 */
    {0x1000, 0x00, 0, 0x07, 0x00020192, 0x00020192, 0, 0xFFFFFFFF, "Device type"},
    {0x1001, 0x00, 0, 0x05, 0x00, 0x00, 0, 0xFF, "Error register"},
    {0x1008, 0x00, 3, 0x09, 0, 0, 0, 0, "Manufacturer device name"},

    /* 身份 */
    {0x1018, 0x00, 0, 0x05, 0x04, 0x04, 0, 0xFF, "Identity object entries"},
    {0x1018, 0x01, 0, 0x07, 0x00000000, 0x00000000, 0, 0xFFFFFFFF, "Vendor ID"},
    {0x1018, 0x02, 0, 0x07, 0x00000001, 0x00000001, 0, 0xFFFFFFFF, "Product code"},
    {0x1018, 0x03, 0, 0x07, 0x00000001, 0x00000001, 0, 0xFFFFFFFF, "Revision number"},
    {0x1018, 0x04, 0, 0x07, 0x00000000, 0x00000000, 0, 0xFFFFFFFF, "Serial number"},

    /* 心跳时间 */
    {0x1017, 0x00, 2, 0x06, 1000, 1000, 0, 65535, "Producer heartbeat time"},

    /* TPDO1通信参数 */
    {0x1800, 0x00, 0, 0x05, 0x05, 0x05, 0, 0xFF, "TPDO1 entries"},
    {0x1800, 0x01, 2, 0x07, 0x40000180, 0x40000180, 0, 0xFFFFFFFF, "TPDO1 COB-ID"},
    {0x1800, 0x02, 2, 0x05, 0xFE, 0xFE, 0, 0xFF, "TPDO1 transmission type"},
    {0x1800, 0x03, 2, 0x06, 0, 0, 0, 65535, "TPDO1 inhibit time"},
    {0x1800, 0x05, 2, 0x06, 0, 0, 0, 65535, "TPDO1 event timer"},

    /* TPDO1映射参数 */
    {0x1A00, 0x00, 0, 0x05, 0x02, 0x02, 0, 0x08, "TPDO1 mapped objects"},
    {0x1A00, 0x01, 2, 0x07, 0x60000108, 0x60000108, 0, 0xFFFFFFFF, "Map: 6000:01"},
    {0x1A00, 0x02, 2, 0x07, 0x60000210, 0x60000210, 0, 0xFFFFFFFF, "Map: 6000:02"},

    /* 应用数据 */
    {0x6000, 0x00, 0, 0x05, 0x02, 0x02, 0, 0xFF, "Application data entries"},
    {0x6000, 0x01, 2, 0x05, 0x00, 0x00, 0, 0xFF, "Digital inputs"},
    {0x6000, 0x02, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog input 1"},
    {0x6000, 0x03, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog input 2"},

    {0x6200, 0x00, 0, 0x05, 0x02, 0x02, 0, 0xFF, "Output data entries"},
    {0x6200, 0x01, 2, 0x05, 0x00, 0x00, 0, 0xFF, "Digital outputs"},
    {0x6200, 0x02, 2, 0x06, 0x0000, 0x0000, 0, 0xFFFF, "Analog output 1"},
};

#define DEFAULT_OD_SIZE (sizeof(default_od) / sizeof(default_od[0]))

/**
 * @brief 创建CANopen节点
 */
canopen_node_t *canopen_create_node(uint8_t node_id, const char *interface)
{
    canopen_node_t *node = calloc(1, sizeof(canopen_node_t));
    if (!node) {
        return NULL;
    }

    node->node_id = node_id;
    node->state = NMT_STATE_BOOTUP;

    /* 复制Object Dictionary */
    node->od_size = DEFAULT_OD_SIZE;
    node->od = malloc(sizeof(default_od));
    if (!node->od) {
        free(node);
        return NULL;
    }
    memcpy(node->od, default_od, sizeof(default_od));

    /* 创建socket */
    node->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (node->socket_fd < 0) {
        perror("socket");
        free(node->od);
        free(node);
        return NULL;
    }

    /* 配置接口 */
    strncpy(node->ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(node->socket_fd, SIOCGIFINDEX, &node->ifr) < 0) {
        perror("ioctl");
        close(node->socket_fd);
        free(node->od);
        free(node);
        return NULL;
    }

    node->addr.can_family = AF_CAN;
    node->addr.can_ifindex = node->ifr.ifr_ifindex;

    /* 绑定 */
    if (bind(node->socket_fd, (struct sockaddr *)&node->addr,
             sizeof(node->addr)) < 0) {
        perror("bind");
        close(node->socket_fd);
        free(node->od);
        free(node);
        return NULL;
    }

    /* 配置接收过滤器 */
    struct can_filter rfilter[4];
    rfilter[0].can_id = COB_ID_NMT;
    rfilter[0].can_mask = CAN_EFF_MASK;
    rfilter[1].can_id = COB_ID_RSDO_BASE + node_id;
    rfilter[1].can_mask = CAN_EFF_MASK;
    rfilter[2].can_id = COB_ID_RPDO1_BASE + node_id;
    rfilter[2].can_mask = CAN_EFF_MASK;
    rfilter[3].can_id = COB_ID_SYNC;
    rfilter[3].can_mask = CAN_EFF_MASK;

    setsockopt(node->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER,
               &rfilter, sizeof(rfilter));

    printf("[CANopen] Node %d created on interface %s\n", node_id, interface);

    return node;
}

/**
 * @brief 销毁CANopen节点
 */
void canopen_destroy_node(canopen_node_t *node)
{
    if (node) {
        if (node->socket_fd >= 0) {
            close(node->socket_fd);
        }
        free(node->od);
        free(node);
    }
}

/**
 * @brief 发送CAN帧
 */
int canopen_send(canopen_node_t *node, uint32_t cob_id,
                  const uint8_t *data, uint8_t len)
{
    struct can_frame frame;

    frame.can_id = cob_id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    int ret = write(node->socket_fd, &frame, sizeof(frame));
    if (ret == sizeof(frame)) {
        node->tx_count++;
        return 0;
    }

    node->error_count++;
    return -1;
}

/**
 * @brief 接收CAN帧
 */
int canopen_receive(canopen_node_t *node, struct can_frame *frame,
                     int timeout_ms)
{
    struct timeval tv;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(node->socket_fd, &rdfs);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(node->socket_fd + 1, &rdfs, NULL, NULL, &tv);
    if (ret < 0) {
        return -1;
    }
    if (ret == 0) {
        return 0;  /* 超时 */
    }

    ret = read(node->socket_fd, frame, sizeof(*frame));
    if (ret == sizeof(*frame)) {
        node->rx_count++;
        return 1;
    }

    return -1;
}

/**
 * @brief 发送心跳报文
 */
void canopen_send_heartbeat(canopen_node_t *node)
{
    uint8_t data = node->state;
    canopen_send(node, COB_ID_HEARTBEAT_BASE + node->node_id, &data, 1);
}

/**
 * @brief 发送启动报文
 */
void canopen_send_bootup(canopen_node_t *node)
{
    uint8_t data = 0;  /* Boot-up state */
    canopen_send(node, COB_ID_HEARTBEAT_BASE + node->node_id, &data, 1);
    printf("[CANopen] Node %d boot-up\n", node->node_id);
}

/**
 * @brief 发送TPDO
 */
void canopen_send_tpdo1(canopen_node_t *node)
{
    uint8_t data[8] = {0};
    uint8_t len = 0;

    /* 根据映射打包数据 */
    for (int i = 0; i < node->od[18].value; i++) {
        uint32_t mapping = node->od[19 + i].value;
        uint16_t index = (mapping >> 16) & 0xFFFF;
        uint8_t subindex = (mapping >> 8) & 0xFF;
        uint8_t bitlen = mapping & 0xFF;

        /* 查找OD条目 */
        for (int j = 0; j < node->od_size; j++) {
            if (node->od[j].index == index && node->od[j].subindex == subindex) {
                /* 复制数据 (简化实现，假设8位对齐) */
                if (bitlen <= 8) {
                    data[len++] = node->od[j].value & 0xFF;
                } else if (bitlen <= 16) {
                    data[len++] = node->od[j].value & 0xFF;
                    data[len++] = (node->od[j].value >> 8) & 0xFF;
                }
                break;
            }
        }
    }

    canopen_send(node, COB_ID_TPDO1_BASE + node->node_id, data, len);
}

/**
 * @brief 查找OD条目
 */
od_entry_t *canopen_find_od(canopen_node_t *node, uint16_t index, uint8_t subindex)
{
    for (int i = 0; i < node->od_size; i++) {
        if (node->od[i].index == index && node->od[i].subindex == subindex) {
            return &node->od[i];
        }
    }
    return NULL;
}

/**
 * @brief 处理SDO上传请求
 */
void canopen_handle_sdo_upload(canopen_node_t *node, const struct can_frame *req)
{
    uint8_t scs = (req->data[0] >> 5) & 0x07;
    uint16_t index = req->data[1] | (req->data[2] << 8);
    uint8_t subindex = req->data[3];

    struct can_frame rsp;
    rsp.can_id = COB_ID_TSDO_BASE + node->node_id;
    rsp.can_dlc = 8;
    memset(rsp.data, 0, 8);

    od_entry_t *entry = canopen_find_od(node, index, subindex);

    if (!entry) {
        /* 对象不存在 */
        rsp.data[0] = 0x80;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];
        rsp.data[4] = 0x06;  /* Object does not exist */
        rsp.data[5] = 0x02;
        rsp.data[6] = 0x00;
        rsp.data[7] = 0x00;
    } else if (entry->access_type == 1) {
        /* 只写对象 */
        rsp.data[0] = 0x80;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];
        rsp.data[4] = 0x01;  /* Attempt to read a write only object */
        rsp.data[5] = 0x00;
        rsp.data[6] = 0x02;
        rsp.data[7] = 0x00;
    } else {
        /* 响应上传请求 */
        rsp.data[0] = (SDO_SCS_UPLOAD_INIT << 5) |
                      ((4 - entry->data_type / 8) << 2) | 0x02;
        rsp.data[1] = req->data[1];
        rsp.data[2] = req->data[2];
        rsp.data[3] = req->data[3];

        /* 复制数据 */
        if (entry->data_type <= 8) {
            rsp.data[4] = entry->value & 0xFF;
        } else if (entry->data_type <= 16) {
            rsp.data[4] = entry->value & 0xFF;
            rsp.data[5] = (entry->value >> 8) & 0xFF;
        } else {
            rsp.data[4] = entry->value & 0xFF;
            rsp.data[5] = (entry->value >> 8) & 0xFF;
            rsp.data[6] = (entry->value >> 16) & 0xFF;
            rsp.data[7] = (entry->value >> 24) & 0xFF;
        }
    }

    write(node->socket_fd, &rsp, sizeof(rsp));
    node->tx_count++;
}

/**
 * @brief 处理SDO下载请求
 */
void canopen_handle_sdo_download(canopen_node_t *node, const struct can_frame *req)
{
    uint16_t index = req->data[1] | (req->data[2] << 8);
    uint8_t subindex = req->data[3];

    struct can_frame rsp;
    rsp.can_id = COB_ID_TSDO_BASE + node->node_id;
    rsp.can_dlc = 8;
    memset(rsp.data, 0, 8);

    od_entry_t *entry = canopen_find_od(node, index, subindex);

    if (!entry) {
        rsp.data[0] = 0x80;
        rsp.data[4] = 0x06;
        rsp.data[5] = 0x02;
    } else if (entry->access_type == 0) {
        /* 只读对象 */
        rsp.data[0] = 0x80;
        rsp.data[4] = 0x02;
        rsp.data[5] = 0x00;
        rsp.data[6] = 0x02;
    } else {
        /* 更新值 */
        uint32_t value = req->data[4] | (req->data[5] << 8) |
                        (req->data[6] << 16) | (req->data[7] << 24);

        if (value >= entry->min_value && value <= entry->max_value) {
            entry->value = value;
            rsp.data[0] = (SDO_SCS_DOWNLOAD_INIT << 5) | 0x02;
        } else {
            rsp.data[0] = 0x80;
            rsp.data[4] = 0x30;
            rsp.data[5] = 0x00;
            rsp.data[6] = 0x02;
        }
    }

    rsp.data[1] = req->data[1];
    rsp.data[2] = req->data[2];
    rsp.data[3] = req->data[3];

    write(node->socket_fd, &rsp, sizeof(rsp));
    node->tx_count++;
}

/**
 * @brief 处理NMT命令
 */
void canopen_handle_nmt(canopen_node_t *node, const struct can_frame *frame)
{
    uint8_t cmd = frame->data[0];
    uint8_t target_id = frame->data[1];

    /* 检查是否针对本节点或所有节点 */
    if (target_id != 0 && target_id != node->node_id) {
        return;
    }

    switch (cmd) {
        case NMT_CMD_START:
            node->state = NMT_STATE_OPERATIONAL;
            printf("[CANopen] Node %d -> OPERATIONAL\n", node->node_id);
            break;

        case NMT_CMD_STOP:
            node->state = NMT_STATE_STOPPED;
            printf("[CANopen] Node %d -> STOPPED\n", node->node_id);
            break;

        case NMT_CMD_PREOP:
            node->state = NMT_STATE_PREOPERATIONAL;
            printf("[CANopen] Node %d -> PRE-OPERATIONAL\n", node->node_id);
            break;

        case NMT_CMD_RESET_NODE:
            printf("[CANopen] Node %d -> RESET NODE\n", node->node_id);
            /* 重置节点 */
            break;

        case NMT_CMD_RESET_COMM:
            printf("[CANopen] Node %d -> RESET COMMUNICATION\n", node->node_id);
            /* 重置通信 */
            break;
    }
}

/**
 * @brief 处理接收到的报文
 */
void canopen_process_frame(canopen_node_t *node, const struct can_frame *frame)
{
    uint32_t cob_id = frame->can_id;

    if (cob_id == COB_ID_NMT) {
        /* NMT命令 */
        canopen_handle_nmt(node, frame);
    } else if (cob_id == COB_ID_RSDO_BASE + node->node_id) {
        /* SDO请求 */
        uint8_t ccs = (frame->data[0] >> 5) & 0x07;
        if (ccs == SDO_CCS_UPLOAD_INIT) {
            canopen_handle_sdo_upload(node, frame);
        } else if (ccs == SDO_CCS_DOWNLOAD_INIT) {
            canopen_handle_sdo_download(node, frame);
        }
    } else if (cob_id == COB_ID_RPDO1_BASE + node->node_id) {
        /* RPDO1接收 */
        printf("[CANopen] RPDO1 received: ");
        for (int i = 0; i < frame->can_dlc; i++) {
            printf("%.2X ", frame->data[i]);
        }
        printf("\n");
    } else if (cob_id == COB_ID_SYNC) {
        /* SYNC接收 - 触发TPDO */
        canopen_send_tpdo1(node);
    }
}

/**
 * @brief 主循环
 */
void canopen_run(canopen_node_t *node)
{
    struct can_frame frame;
    struct timeval last_heartbeat, now;

    gettimeofday(&last_heartbeat, NULL);

    /* 发送boot-up报文 */
    canopen_send_bootup(node);
    node->state = NMT_STATE_PREOPERATIONAL;

    /* 读取心跳时间 */
    od_entry_t *hb_entry = canopen_find_od(node, 0x1017, 0x00);
    node->heartbeat_time = hb_entry ? hb_entry->value : 1000;

    printf("[CANopen] Node %d running (heartbeat: %d ms)\n",
           node->node_id, node->heartbeat_time);

    while (1) {
        /* 接收报文 */
        int ret = canopen_receive(node, &frame, 10);
        if (ret > 0) {
            canopen_process_frame(node, &frame);
        }

        /* 发送心跳 */
        gettimeofday(&now, NULL);
        long elapsed = (now.tv_sec - last_heartbeat.tv_sec) * 1000 +
                       (now.tv_usec - last_heartbeat.tv_usec) / 1000;

        if (elapsed >= node->heartbeat_time) {
            canopen_send_heartbeat(node);
            last_heartbeat = now;
        }

        /* 模拟数据变化 */
        static int counter = 0;
        if (++counter % 100 == 0) {
            /* 更新模拟输入值 */
            od_entry_t *ai1 = canopen_find_od(node, 0x6000, 0x02);
            if (ai1) {
                ai1->value = (uint16_t)(rand() % 4096);
            }
        }
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          CANopen Node Demo Application                   ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    printf("[INFO] This demo requires a CAN interface (e.g., can0)\n");
    printf("[INFO] Setup: ip link set can0 up type can bitrate 500000\n\n");

    /* 创建节点 */
    canopen_node_t *node = canopen_create_node(1, "can0");
    if (!node) {
        fprintf(stderr, "[ERROR] Failed to create CANopen node\n");
        fprintf(stderr, "[INFO] Make sure CAN interface is available\n");
        return 1;
    }

    /* 运行节点 */
    canopen_run(node);

    /* 清理 */
    canopen_destroy_node(node);

    return 0;
}
