/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 306
 * Language: c
 * Block ID: 4dfb0ec9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ELM327通信示例
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define ELM_BAUDRATE B38400
#define ELM_DEVICE "/dev/ttyUSB0"

int elm_init(const char* device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) return -1;

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, ELM_BAUDRATE);
    cfsetospeed(&options, ELM_BAUDRATE);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;

    tcsetattr(fd, TCSANOW, &options);
    tcflush(fd, TCIOFLUSH);

    return fd;
}

// 发送AT命令
int elm_send_command(int fd, const char* cmd, char* response, size_t resp_size) {
    char cmd_buffer[32];
    snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r", cmd);

    write(fd, cmd_buffer, strlen(cmd_buffer));

    // 读取响应
    usleep(100000);  // 等待100ms

    ssize_t n = read(fd, response, resp_size - 1);
    if (n > 0) {
        response[n] = '\0';
        return 0;
    }

    return -1;
}

// 初始化ELM327
int elm_reset(int fd) {
    char resp[64];
    return elm_send_command(fd, "ATZ", resp, sizeof(resp));
}

// 设置自动协议检测
int elm_set_auto_protocol(int fd) {
    char resp[64];
    return elm_send_command(fd, "ATSP0", resp, sizeof(resp));
}

// 请求PID
int elm_request_pid(int fd, uint8_t service, uint8_t pid,
                    uint8_t* data, size_t* data_len) {
    char cmd[16];
    char resp[128];

    snprintf(cmd, sizeof(cmd), "%02X%02X", service, pid);

    if (elm_send_command(fd, cmd, resp, sizeof(resp)) < 0) {
        return -1;
    }

    // 解析十六进制响应
    // 格式: "41 0C 1B 56" → 数据: [0x1B, 0x56]
    char* p = resp;
    size_t idx = 0;

    while (*p && idx < *data_len) {
        unsigned int byte;
        if (sscanf(p, "%2X", &byte) == 1) {
            data[idx++] = (uint8_t)byte;
            p += 3;  // 跳过"XX "或"XX"
        } else {
            p++;
        }
    }

    *data_len = idx;
    return 0;
}

int main(void) {
    int fd = elm_init(ELM_DEVICE);
    if (fd < 0) {
        perror("Failed to open ELM327");
        return 1;
    }

    printf("Initializing ELM327...\n");
    elm_reset(fd);
    elm_set_auto_protocol(fd);

    // 请求发动机转速
    printf("Requesting Engine RPM...\n");

    uint8_t data[8];
    size_t data_len = sizeof(data);

    if (elm_request_pid(fd, 0x01, PID_ENGINE_RPM, data, &data_len) == 0) {
        if (data_len >= 4 && data[1] == PID_ENGINE_RPM) {
            uint16_t rpm = ((data[2] << 8) | data[3]) / 4;
            printf("Engine RPM: %u\n", rpm);
        }
    }

    // 请求故障码数量
    printf("Requesting DTC count...\n");

    if (elm_request_pid(fd, 0x01, 0x01, data, &data_len) == 0) {
        uint8_t dtc_count = data[3] & 0x7F;  // 去掉MIL状态位
        printf("Stored DTCs: %d\n", dtc_count);
    }

    close(fd);
    return 0;
}
