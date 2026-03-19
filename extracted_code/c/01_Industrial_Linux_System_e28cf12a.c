/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 2620
 * Language: c
 * Block ID: e28cf12a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file modbus_master.c
 * @brief Modbus RTU/TCP主站实现
 * @details 支持Modbus RTU (串口) 和 Modbus TCP (网络) 通信
 *
 * 编译: gcc -o modbus_master modbus_master.c
 * 运行: sudo ./modbus_master
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>

/* Modbus功能码 */
#define MODBUS_FC_READ_COILS            0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS  0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS 0x03
#define MODBUS_FC_READ_INPUT_REGISTERS  0x04
#define MODBUS_FC_WRITE_SINGLE_COIL     0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER 0x06
#define MODBUS_FC_WRITE_MULTIPLE_COILS  0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS 0x10

/* Modbus异常码 */
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION       0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS   0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE     0x03
#define MODBUS_EXCEPTION_SLAVE_DEVICE_FAILURE   0x04

/* 配置 */
#define MODBUS_MAX_PDU_LENGTH       253
#define MODBUS_MAX_ADU_LENGTH       256
#define MODBUS_TCP_DEFAULT_PORT     502
#define MODBUS_TCP_MAX_CONNECTIONS  5

/* 传输模式 */
typedef enum {
    MODBUS_RTU = 0,
    MODBUS_TCP = 1
} modbus_mode_t;

/* 错误码 */
typedef enum {
    MODBUS_OK = 0,
    MODBUS_ERR_INVALID_PARAM = -1,
    MODBUS_ERR_CONNECTION = -2,
    MODBUS_ERR_TIMEOUT = -3,
    MODBUS_ERR_CRC = -4,
    MODBUS_ERR_EXCEPTION = -5,
    MODBUS_ERR_BUFFER = -6
} modbus_error_t;

/* Modbus上下文 */
typedef struct {
    modbus_mode_t mode;             /* 传输模式 */
    int fd;                         /* 文件描述符 (串口或socket) */

    /* RTU参数 */
    char device[64];                /* 串口设备 */
    int baudrate;                   /* 波特率 */
    char parity;                    /* 校验位: 'N', 'E', 'O' */
    int data_bits;                  /* 数据位 */
    int stop_bits;                  /* 停止位 */
    uint8_t slave_id;               /* 从站地址 */

    /* TCP参数 */
    char ip_address[16];            /* IP地址 */
    int tcp_port;                   /* TCP端口 */
    struct sockaddr_in tcp_addr;    /* TCP地址 */

    /* 通信参数 */
    uint32_t response_timeout_ms;   /* 响应超时 */
    uint32_t byte_timeout_ms;       /* 字节间超时 */

    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
} modbus_context_t;

/**
 * @brief CRC16计算 (Modbus标准)
 */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    static const uint16_t crc_table[256] = {
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
        0x8E01, 0x4EC0, 0x4F80, 0x8F41, 0x4D00, 0x8DC1, 0x8C81, 0x4C40,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
    };

    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc = (crc >> 8) ^ crc_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

/**
 * @brief 创建RTU上下文
 */
modbus_context_t *modbus_new_rtu(const char *device, int baudrate,
                                  char parity, int data_bits, int stop_bits)
{
    modbus_context_t *ctx = calloc(1, sizeof(modbus_context_t));
    if (!ctx) {
        return NULL;
    }

    ctx->mode = MODBUS_RTU;
    ctx->fd = -1;
    strncpy(ctx->device, device, sizeof(ctx->device) - 1);
    ctx->baudrate = baudrate;
    ctx->parity = parity;
    ctx->data_bits = data_bits;
    ctx->stop_bits = stop_bits;
    ctx->slave_id = 1;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;

    return ctx;
}

/**
 * @brief 创建TCP上下文
 */
modbus_context_t *modbus_new_tcp(const char *ip_address, int port)
{
    modbus_context_t *ctx = calloc(1, sizeof(modbus_context_t));
    if (!ctx) {
        return NULL;
    }

    ctx->mode = MODBUS_TCP;
    ctx->fd = -1;
    strncpy(ctx->ip_address, ip_address, sizeof(ctx->ip_address) - 1);
    ctx->tcp_port = (port == 0) ? MODBUS_TCP_DEFAULT_PORT : port;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;

    return ctx;
}

/**
 * @brief 设置从站地址
 */
void modbus_set_slave(modbus_context_t *ctx, int slave)
{
    if (ctx) {
        ctx->slave_id = (uint8_t)slave;
    }
}

/**
 * @brief 连接RTU设备
 */
int modbus_connect_rtu(modbus_context_t *ctx)
{
    struct termios tios;

    /* 打开串口 */
    ctx->fd = open(ctx->device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (ctx->fd < 0) {
        perror("open");
        return MODBUS_ERR_CONNECTION;
    }

    /* 清除非阻塞标志 */
    int flags = fcntl(ctx->fd, F_GETFL);
    fcntl(ctx->fd, F_SETFL, flags & ~O_NONBLOCK);

    /* 配置串口 */
    tcgetattr(ctx->fd, &tios);
    cfmakeraw(&tios);

    /* 设置波特率 */
    speed_t speed;
    switch (ctx->baudrate) {
        case 9600:   speed = B9600;   break;
        case 19200:  speed = B19200;  break;
        case 38400:  speed = B38400;  break;
        case 57600:  speed = B57600;  break;
        case 115200: speed = B115200; break;
        default:     speed = B9600;   break;
    }
    cfsetispeed(&tios, speed);
    cfsetospeed(&tios, speed);

    /* 设置数据位 */
    tios.c_cflag &= ~CSIZE;
    switch (ctx->data_bits) {
        case 5: tios.c_cflag |= CS5; break;
        case 6: tios.c_cflag |= CS6; break;
        case 7: tios.c_cflag |= CS7; break;
        case 8: default: tios.c_cflag |= CS8; break;
    }

    /* 设置校验位 */
    switch (ctx->parity) {
        case 'E':
            tios.c_cflag |= PARENB;
            tios.c_cflag &= ~PARODD;
            break;
        case 'O':
            tios.c_cflag |= PARENB;
            tios.c_cflag |= PARODD;
            break;
        case 'N': default:
            tios.c_cflag &= ~PARENB;
            break;
    }

    /* 设置停止位 */
    if (ctx->stop_bits == 2) {
        tios.c_cflag |= CSTOPB;
    } else {
        tios.c_cflag &= ~CSTOPB;
    }

    /* 启用接收 */
    tios.c_cflag |= CREAD | CLOCAL;

    /* 应用配置 */
    tcsetattr(ctx->fd, TCSANOW, &tios);
    tcflush(ctx->fd, TCIOFLUSH);

    printf("[MODBUS] RTU connected: %s@%d %d%c%d\n",
           ctx->device, ctx->baudrate, ctx->data_bits, ctx->parity, ctx->stop_bits);

    return MODBUS_OK;
}

/**
 * @brief 连接TCP设备
 */
int modbus_connect_tcp(modbus_context_t *ctx)
{
    /* 创建socket */
    ctx->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->fd < 0) {
        perror("socket");
        return MODBUS_ERR_CONNECTION;
    }

    /* 配置地址 */
    memset(&ctx->tcp_addr, 0, sizeof(ctx->tcp_addr));
    ctx->tcp_addr.sin_family = AF_INET;
    ctx->tcp_addr.sin_port = htons(ctx->tcp_port);
    inet_pton(AF_INET, ctx->ip_address, &ctx->tcp_addr.sin_addr);

    /* 连接 */
    if (connect(ctx->fd, (struct sockaddr *)&ctx->tcp_addr,
                sizeof(ctx->tcp_addr)) < 0) {
        perror("connect");
        close(ctx->fd);
        ctx->fd = -1;
        return MODBUS_ERR_CONNECTION;
    }

    printf("[MODBUS] TCP connected: %s:%d\n", ctx->ip_address, ctx->tcp_port);

    return MODBUS_OK;
}

/**
 * @brief 连接
 */
int modbus_connect(modbus_context_t *ctx)
{
    if (!ctx) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    if (ctx->mode == MODBUS_RTU) {
        return modbus_connect_rtu(ctx);
    } else {
        return modbus_connect_tcp(ctx);
    }
}

/**
 * @brief 关闭连接
 */
void modbus_close(modbus_context_t *ctx)
{
    if (ctx && ctx->fd >= 0) {
        close(ctx->fd);
        ctx->fd = -1;
    }
}

/**
 * @brief 释放上下文
 */
void modbus_free(modbus_context_t *ctx)
{
    if (ctx) {
        modbus_close(ctx);
        free(ctx);
    }
}

/**
 * @brief 发送和接收数据 (RTU)
 */
static int modbus_send_recv_rtu(modbus_context_t *ctx,
                                 const uint8_t *req, int req_len,
                                 uint8_t *rsp, int rsp_max)
{
    uint8_t adu[MODBUS_MAX_ADU_LENGTH];
    int adu_len = 0;

    /* 构建ADU: 从站地址 + PDU + CRC */
    adu[0] = ctx->slave_id;
    memcpy(adu + 1, req, req_len);
    adu_len = 1 + req_len;

    /* 计算CRC */
    uint16_t crc = modbus_crc16(adu, adu_len);
    adu[adu_len++] = crc & 0xFF;
    adu[adu_len++] = (crc >> 8) & 0xFF;

    /* 清除输入缓冲区 */
    tcflush(ctx->fd, TCIFLUSH);

    /* 发送 */
    int sent = write(ctx->fd, adu, adu_len);
    if (sent != adu_len) {
        return MODBUS_ERR_CONNECTION;
    }
    ctx->tx_count++;

    /* 等待响应 (简化处理) */
    usleep(ctx->response_timeout_ms * 1000);

    /* 接收 */
    int received = read(ctx->fd, adu, sizeof(adu));
    if (received < 5) {
        return MODBUS_ERR_TIMEOUT;
    }
    ctx->rx_count++;

    /* 验证从站地址 */
    if (adu[0] != ctx->slave_id) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    /* 验证CRC */
    crc = modbus_crc16(adu, received - 2);
    if ((adu[received - 2] != (crc & 0xFF)) ||
        (adu[received - 1] != ((crc >> 8) & 0xFF))) {
        ctx->error_count++;
        return MODBUS_ERR_CRC;
    }

    /* 提取PDU */
    int pdu_len = received - 3;  /* - 从站地址 - CRC2 */
    if (pdu_len > rsp_max) {
        pdu_len = rsp_max;
    }
    memcpy(rsp, adu + 1, pdu_len);

    return pdu_len;
}

/**
 * @brief 发送和接收数据 (TCP)
 */
static int modbus_send_recv_tcp(modbus_context_t *ctx,
                                 const uint8_t *req, int req_len,
                                 uint8_t *rsp, int rsp_max)
{
    uint8_t adu[MODBUS_MAX_ADU_LENGTH];
    int adu_len = 0;

    /* 构建MBAP头: 事务ID(2) + 协议ID(2) + 长度(2) + 单元ID(1) */
    static uint16_t transaction_id = 0;
    adu[0] = (transaction_id >> 8) & 0xFF;
    adu[1] = transaction_id & 0xFF;
    transaction_id++;
    adu[2] = 0x00;  /* 协议ID高字节 */
    adu[3] = 0x00;  /* 协议ID低字节 (Modbus) */
    adu[4] = ((req_len + 1) >> 8) & 0xFF;
    adu[5] = (req_len + 1) & 0xFF;
    adu[6] = ctx->slave_id;

    memcpy(adu + 7, req, req_len);
    adu_len = 7 + req_len;

    /* 发送 */
    int sent = send(ctx->fd, adu, adu_len, 0);
    if (sent != adu_len) {
        return MODBUS_ERR_CONNECTION;
    }
    ctx->tx_count++;

    /* 接收MBAP头 */
    int received = recv(ctx->fd, adu, 7, MSG_WAITALL);
    if (received != 7) {
        return MODBUS_ERR_TIMEOUT;
    }

    /* 获取数据长度 */
    int data_len = (adu[4] << 8) | adu[5];
    data_len -= 1;  /* 减去单元ID */

    if (data_len > rsp_max) {
        data_len = rsp_max;
    }

    /* 接收数据 */
    received = recv(ctx->fd, rsp, data_len, MSG_WAITALL);
    if (received != data_len) {
        return MODBUS_ERR_TIMEOUT;
    }
    ctx->rx_count++;

    return data_len;
}

/**
 * @brief 读取线圈
 */
int modbus_read_bits(modbus_context_t *ctx, int addr, int nb, uint8_t *dest)
{
    if (!ctx || ctx->fd < 0 || nb > 2000) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_READ_COILS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    /* 检查异常 */
    if (rsp[0] & 0x80) {
        ctx->error_count++;
        return MODBUS_ERR_EXCEPTION;
    }

    /* 提取数据 */
    int byte_count = rsp[1];
    memcpy(dest, rsp + 2, byte_count);

    return nb;
}

/**
 * @brief 读取保持寄存器
 */
int modbus_read_registers(modbus_context_t *ctx, int addr, int nb, uint16_t *dest)
{
    if (!ctx || ctx->fd < 0 || nb > 125) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_READ_HOLDING_REGISTERS;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (nb >> 8) & 0xFF;
    req[4] = nb & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    /* 检查异常 */
    if (rsp[0] & 0x80) {
        ctx->error_count++;
        return MODBUS_ERR_EXCEPTION;
    }

    /* 提取数据 */
    int byte_count = rsp[1];
    for (int i = 0; i < byte_count / 2; i++) {
        dest[i] = (rsp[2 + i * 2] << 8) | rsp[3 + i * 2];
    }

    return nb;
}

/**
 * @brief 写入单个寄存器
 */
int modbus_write_register(modbus_context_t *ctx, int addr, const uint16_t value)
{
    if (!ctx || ctx->fd < 0) {
        return MODBUS_ERR_INVALID_PARAM;
    }

    uint8_t req[5];
    uint8_t rsp[MODBUS_MAX_PDU_LENGTH];

    req[0] = MODBUS_FC_WRITE_SINGLE_REGISTER;
    req[1] = (addr >> 8) & 0xFF;
    req[2] = addr & 0xFF;
    req[3] = (value >> 8) & 0xFF;
    req[4] = value & 0xFF;

    int rsp_len;
    if (ctx->mode == MODBUS_RTU) {
        rsp_len = modbus_send_recv_rtu(ctx, req, 5, rsp, sizeof(rsp));
    } else {
        rsp_len = modbus_send_recv_tcp(ctx, req, 5, rsp, sizeof(rsp));
    }

    if (rsp_len < 0) {
        return rsp_len;
    }

    return (rsp[0] & 0x80) ? MODBUS_ERR_EXCEPTION : 1;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║          Modbus Master Demo Application                  ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* RTU示例 */
    printf("[DEMO] RTU Mode Example\n");
    printf("=======================\n");

    modbus_context_t *ctx_rtu = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    if (ctx_rtu) {
        modbus_set_slave(ctx_rtu, 1);

        if (modbus_connect(ctx_rtu) == MODBUS_OK) {
            /* 读取保持寄存器 */
            uint16_t regs[10];
            int ret = modbus_read_registers(ctx_rtu, 0, 4, regs);
            if (ret > 0) {
                printf("[RTU] Read registers: ");
                for (int i = 0; i < ret; i++) {
                    printf("%.4X ", regs[i]);
                }
                printf("\n");
            }

            modbus_close(ctx_rtu);
        }

        modbus_free(ctx_rtu);
    }

    printf("\n");

    /* TCP示例 */
    printf("[DEMO] TCP Mode Example\n");
    printf("=======================\n");

    modbus_context_t *ctx_tcp = modbus_new_tcp("192.168.1.100", 502);
    if (ctx_tcp) {
        modbus_set_slave(ctx_tcp, 1);

        if (modbus_connect(ctx_tcp) == MODBUS_OK) {
            /* 读取线圈 */
            uint8_t bits[16];
            int ret = modbus_read_bits(ctx_tcp, 0, 16, bits);
            if (ret > 0) {
                printf("[TCP] Read coils: ");
                for (int i = 0; i < ret; i++) {
                    printf("%d", (bits[i / 8] >> (i % 8)) & 1);
                }
                printf("\n");
            }

            /* 读取寄存器 */
            uint16_t regs[10];
            ret = modbus_read_registers(ctx_tcp, 0, 4, regs);
            if (ret > 0) {
                printf("[TCP] Read registers: ");
                for (int i = 0; i < ret; i++) {
                    printf("%.4X ", regs[i]);
                }
                printf("\n");
            }

            /* 写入寄存器 */
            ret = modbus_write_register(ctx_tcp, 0, 0x1234);
            printf("[TCP] Write register: %s\n",
                   ret > 0 ? "OK" : "FAILED");

            modbus_close(ctx_tcp);
        }

        modbus_free(ctx_tcp);
    }

    printf("\n[DEMO] Completed\n");

    return 0;
}
