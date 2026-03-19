/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md
 * Line: 251
 * Language: c
 * Block ID: 67906d5e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
