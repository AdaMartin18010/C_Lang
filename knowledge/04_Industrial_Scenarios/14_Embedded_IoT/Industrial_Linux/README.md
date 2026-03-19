# 工业 Linux (Industrial Linux)

> **描述**: 工业环境下Linux嵌入式系统开发指南，涵盖实时Linux(PREEMPT_RT/Xenomai)、EtherCAT主站、软PLC运行时、OPC UA服务器、工业协议栈等关键技术
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: Linux内核、嵌入式C、实时系统

---

## 概述

工业Linux是专门为工业控制场景优化的Linux系统，结合了Linux的开放性和工业应用所需的实时性、可靠性。本目录提供工业Linux系统开发的完整知识体系，从实时内核配置到工业协议实现。

现代工业自动化系统越来越倾向于使用Linux作为基础平台，利用其丰富的软件生态和强大的网络功能。理解如何在Linux上实现实时控制、工业通信和数据采集，是工业软件开发人员的核心技能。

---

## 核心内容

- **实时Linux技术**：PREEMPT_RT补丁原理与配置、Xenomai双内核架构、实时任务调度
- **EtherCAT主站开发**：IgH EtherCAT Master库使用、从站配置、过程数据交换
- **软PLC实现**：PLC运行时架构、IEC 61131-3运行时、扫描周期管理
- **OPC UA服务器**：open62541库使用、地址空间建模、安全通信
- **工业协议栈**：Modbus RTU/TCP主从站、CANopen协议、MQTT物联网通信
- **数据采集系统**：高性能数据采集架构、环形缓冲区、实时数据库
- **边缘计算框架**：边缘节点设计、规则引擎、数据预处理
- **系统可靠性**：看门狗机制、进程监控、OTA远程升级

---

## 深入理解

### 1. PREEMPT_RT实时Linux

PREEMPT_RT补丁使Linux内核成为硬实时操作系统。

```c
// PREEMPT_RT实时任务示例
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <errno.h>

#define NSEC_PER_SEC 1000000000L
#define RT_PRIORITY 80

// 计算时间差 (纳秒)
static inline long timespec_diff_ns(struct timespec* a, struct timespec* b) {
    return (a->tv_sec - b->tv_sec) * NSEC_PER_SEC + (a->tv_nsec - b->tv_nsec);
}

// 实时控制任务
void* rt_control_task(void* arg) {
    struct timespec next_period;
    struct timespec now;
    long period_ns = 1000000;  // 1ms周期
    long max_jitter = 0;
    long jitter;
    int iteration = 0;

    // 获取当前时间
    clock_gettime(CLOCK_MONOTONIC, &next_period);

    printf("RT Task started with %ld us period\n", period_ns / 1000);

    while (iteration < 10000) {
        // 计算下一次周期
        next_period.tv_nsec += period_ns;
        if (next_period.tv_nsec >= NSEC_PER_SEC) {
            next_period.tv_sec++;
            next_period.tv_nsec -= NSEC_PER_SEC;
        }

        // 等待下一个周期
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, NULL);

        // 获取实际唤醒时间
        clock_gettime(CLOCK_MONOTONIC, &now);

        // 计算抖动
        jitter = labs(timespec_diff_ns(&now, &next_period));
        if (jitter > max_jitter) {
            max_jitter = jitter;
        }

        // 执行实时控制逻辑
        // read_sensors();
        // compute_control();
        // write_actuators();

        iteration++;
    }

    printf("Max jitter: %ld ns\n", max_jitter);
    return NULL;
}

int main() {
    pthread_t rt_thread;
    pthread_attr_t attr;
    struct sched_param param;

    // 初始化线程属性
    pthread_attr_init(&attr);

    // 设置调度策略为SCHED_FIFO
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // 设置优先级
    param.sched_priority = RT_PRIORITY;
    pthread_attr_setschedparam(&attr, &param);

    // 显式设置继承调度属性
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    // 锁定内存 (防止缺页)
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
    }

    // 创建实时线程
    pthread_create(&rt_thread, &attr, rt_control_task, NULL);

    // 等待线程完成
    pthread_join(rt_thread, NULL);

    pthread_attr_destroy(&attr);
    munlockall();

    return 0;
}
```

### 2. EtherCAT主站开发

```c
// EtherCAT主站简化示例 (基于IgH EtherCAT Master)
#include <ecrt.h>

// 从站配置
#define VENDOR_ID 0x00000002  // Beckhoff
#define PRODUCT_CODE 0x044c2c52  // EL2809

// 过程数据对象 (PDO) 映射
static ec_pdo_entry_reg_t domain1_regs[] = {
    {0, 0, VENDOR_ID, PRODUCT_CODE, 0x7000, 0x01, &off_dig_out},  // 数字输出
    {}
};

// 应用层接口
typedef struct {
    ec_master_t* master;
    ec_domain_t* domain1;
    uint8_t* domain1_pd;  // 过程数据映射
    ec_slave_config_t* sc_dig_out;
    unsigned int off_dig_out;

    // 周期统计
    uint32_t cycle_count;
    uint32_t overruns;
} EtherCATMaster;

int ec_init(EtherCATMaster* ec) {
    // 请求主站
    ec->master = ecrt_request_master(0);
    if (!ec->master) {
        return -1;
    }

    // 创建域
    ec->domain1 = ecrt_master_create_domain(ec->master);
    if (!ec->domain1) {
        return -1;
    }

    // 配置从站
    ec->sc_dig_out = ecrt_master_slave_config(ec->master, 0, 0,
                                               VENDOR_ID, PRODUCT_CODE);
    if (!ec->sc_dig_out) {
        return -1;
    }

    // 配置PDO映射
    static ec_pdo_entry_t pdo_entries[] = {
        {0x7000, 0x01, 8},  // 8位输出
    };
    static ec_pdo_info_t pdos[] = {
        {0x1600, 1, pdo_entries},
    };
    static ec_sync_info_t syncs[] = {
        {0, EC_DIR_OUTPUT, 1, pdos, EC_WD_ENABLE},
        {0xff}
    };

    if (ecrt_slave_config_pdos(ec->sc_dig_out, EC_END, syncs)) {
        return -1;
    }

    // 注册PDO条目
    if (ecrt_domain_reg_pdo_entry_list(ec->domain1, domain1_regs)) {
        return -1;
    }

    // 激活主站
    if (ecrt_master_activate(ec->master)) {
        return -1;
    }

    // 获取过程数据指针
    ec->domain1_pd = ecrt_domain_data(ec->domain1);

    return 0;
}

void ec_cycle(EtherCATMaster* ec) {
    struct timespec wakeup_time;

    clock_gettime(CLOCK_MONOTONIC, &wakeup_time);

    while (1) {
        // 1ms周期
        wakeup_time.tv_nsec += 1000000;
        if (wakeup_time.tv_nsec >= 1000000000) {
            wakeup_time.tv_sec++;
            wakeup_time.tv_nsec -= 1000000000;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeup_time, NULL);

        // 接收数据
        ecrt_master_receive(ec->master);
        ecrt_domain_process(ec->domain1);

        // 应用逻辑: 写入输出
        EC_WRITE_U8(ec->domain1_pd + ec->off_dig_out, 0xFF);  // 所有输出ON

        // 发送数据
        ecrt_domain_queue(ec->domain1);
        ecrt_master_send(ec->master);

        ec->cycle_count++;
    }
}

void ec_cleanup(EtherCATMaster* ec) {
    ecrt_release_master(ec->master);
}
```

### 3. Modbus RTU主站实现

```c
// Modbus RTU主站 (串口通信)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

#define MODBUS_BAUDRATE B9600
#define MODBUS_DATABITS CS8
#define MODBUS_PARITY 0
#define MODBUS_STOPBITS 1

// CRC16计算
uint16_t modbus_crc16(uint8_t* data, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 读取保持寄存器 (功能码0x03)
int modbus_read_registers(int fd, uint8_t slave_id,
                          uint16_t start_addr, uint16_t num_regs,
                          uint16_t* values) {
    uint8_t request[8];
    uint8_t response[256];

    // 构建请求帧
    request[0] = slave_id;
    request[1] = 0x03;  // 功能码
    request[2] = (start_addr >> 8) & 0xFF;
    request[3] = start_addr & 0xFF;
    request[4] = (num_regs >> 8) & 0xFF;
    request[5] = num_regs & 0xFF;

    uint16_t crc = modbus_crc16(request, 6);
    request[6] = crc & 0xFF;
    request[7] = (crc >> 8) & 0xFF;

    // 发送请求
    write(fd, request, 8);

    // 读取响应
    // 响应: 从机ID + 功能码 + 字节数 + 数据 + CRC
    int bytes_read = read(fd, response, 5 + num_regs * 2 + 2);

    if (bytes_read < 5) {
        return -1;  // 超时或错误
    }

    // 校验CRC
    uint16_t resp_crc = (response[bytes_read-1] << 8) | response[bytes_read-2];
    if (modbus_crc16(response, bytes_read-2) != resp_crc) {
        return -2;  // CRC错误
    }

    // 解析数据
    uint8_t byte_count = response[2];
    for (int i = 0; i < num_regs; i++) {
        values[i] = (response[3 + i*2] << 8) | response[4 + i*2];
    }

    return 0;
}

// 配置串口
int modbus_serial_init(const char* device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) return -1;

    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, MODBUS_BAUDRATE);
    cfsetospeed(&options, MODBUS_BAUDRATE);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;  // 无校验
    options.c_cflag &= ~CSTOPB;  // 1停止位
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      // 8数据位

    tcsetattr(fd, TCSANOW, &options);
    tcflush(fd, TCIOFLUSH);

    return fd;
}
```

---

## 文件列表

| 文件 | 描述 |
|------|------|
| `01_Industrial_Linux_System.md` | 工业Linux系统开发完整指南 |
| `preempt_rt_task.c` | PREEMPT_RT实时任务示例 |
| `xenomai_task.c` | Xenomai硬实时编程示例 |
| `ethercat_master.c` | EtherCAT主站开发示例 |
| `soft_plc_runtime.c` | 软PLC运行时代码 |
| `opcua_server.c` | OPC UA服务器实现 |

---

## 学习路径

1. **基础阶段**：掌握嵌入式Linux基本开发，理解设备树、驱动开发
2. **实时阶段**：学习PREEMPT_RT配置，编写实时应用程序
3. **协议阶段**：实现Modbus、CANopen等工业通信协议
4. **集成阶段**：整合实时控制、通信、数据采集功能

---

## 应用场景

- 工业PLC控制器开发
- 运动控制系统
- 工业数据采集网关
- 边缘计算节点
- SCADA系统

---

## 权威参考

- [1] Linux Kernel Documentation - Real-Time
- [2] EtherCAT Technology Group Specification
- [3] IEC 61131-3 Standard

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
