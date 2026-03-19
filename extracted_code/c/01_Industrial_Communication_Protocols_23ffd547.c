/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\01_Industrial_Communication_Protocols.md
 * Line: 634
 * Language: c
 * Block ID: 23ffd547
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
