/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 34
 * Language: c
 * Block ID: f3c92048
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// InfiniBand 设备初始化示例
#include <infiniband/verbs.h>

struct ibv_device **dev_list;
struct ibv_device *ib_dev;
struct ibv_context *ctx;
int num_devices;

// 获取可用设备列表
dev_list = ibv_get_device_list(&num_devices);
if (!dev_list) {
    perror("Failed to get IB devices list");
    return -1;
}

// 选择第一个设备
ib_dev = dev_list[0];
if (!ib_dev) {
    fprintf(stderr, "No IB devices found\n");
    return -1;
}

// 打开设备上下文
ctx = ibv_open_device(ib_dev);
if (!ctx) {
    fprintf(stderr, "Failed to open device %s\n",
            ibv_get_device_name(ib_dev));
    return -1;
}

printf("Opened device: %s\n", ibv_get_device_name(ib_dev));
