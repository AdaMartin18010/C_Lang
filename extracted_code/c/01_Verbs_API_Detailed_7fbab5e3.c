/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 223
 * Language: c
 * Block ID: 7fbab5e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA设备初始化完整实现
 * 包含设备发现、上下文创建、PD分配
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <infiniband/verbs.h>
#include <errno.h>

#define CQ_SIZE 1024
#define MAX_WR 256

/* RDMA上下文结构 */
typedef struct {
    struct ibv_context *ctx;        /* 设备上下文 */
    struct ibv_pd *pd;              /* 保护域 */
    struct ibv_cq *cq;              /* 发送/接收完成队列 */
    struct ibv_qp *qp;              /* 队列对 */
    struct ibv_comp_channel *comp_channel;  /* 完成事件通道 */
    int port_num;                   /* 端口号 */
    int gid_idx;                    /* GID索引 */
} rdma_context_t;

/* 设备信息结构 */
typedef struct {
    char name[64];
    uint64_t node_guid;
    uint8_t port_count;
    enum ibv_node_type node_type;
    enum ibv_transport_type transport_type;
} device_info_t;

/**
 * 发现并列出所有RDMA设备
 */
int list_rdma_devices(device_info_t **devices, int *num_devs) {
    struct ibv_device **dev_list;
    int num_devices, i;

    /* 获取设备列表 */
    dev_list = ibv_get_device_list(&num_devices);
    if (!dev_list || num_devices == 0) {
        fprintf(stderr, "No RDMA devices found\n");
        return -1;
    }

    *devices = calloc(num_devices, sizeof(device_info_t));
    if (!*devices) {
        ibv_free_device_list(dev_list);
        return -ENOMEM;
    }

    printf("发现 %d 个RDMA设备:\n", num_devices);
    printf("%-20s %-20s %-10s %-15s\n",
           "设备名称", "Node GUID", "端口数", "传输类型");
    printf("%-20s %-20s %-10s %-15s\n",
           "--------", "---------", "------", "--------");

    for (i = 0; i < num_devices; i++) {
        struct ibv_device *dev = dev_list[i];
        device_info_t *info = &(*devices)[i];

        strncpy(info->name, ibv_get_device_name(dev), 63);
        info->node_guid = ibv_get_device_guid(dev);
        info->node_type = dev->node_type;
        info->transport_type = dev->transport_type;

        const char *trans_str;
        switch (dev->transport_type) {
            case IBV_TRANSPORT_IB: trans_str = "InfiniBand"; break;
            case IBV_TRANSPORT_IWARP: trans_str = "iWARP"; break;
            case IBV_TRANSPORT_UNSPEC: trans_str = "Unspecified"; break;
            default: trans_str = "Unknown"; break;
        }

        printf("%-20s %016llx %-10d %-15s\n",
               info->name,
               (unsigned long long)info->node_guid,
               info->port_count,
               trans_str);
    }

    *num_devs = num_devices;
    ibv_free_device_list(dev_list);
    return 0;
}

/**
 * 打开指定RDMA设备
 */
struct ibv_context* open_rdma_device(const char *dev_name) {
    struct ibv_device **dev_list;
    struct ibv_context *ctx = NULL;
    int num_devices, i;

    dev_list = ibv_get_device_list(&num_devices);
    if (!dev_list) {
        perror("ibv_get_device_list");
        return NULL;
    }

    for (i = 0; i < num_devices; i++) {
        if (strcmp(ibv_get_device_name(dev_list[i]), dev_name) == 0) {
            ctx = ibv_open_device(dev_list[i]);
            if (!ctx) {
                perror("ibv_open_device");
            }
            break;
        }
    }

    if (!ctx && i == num_devices) {
        fprintf(stderr, "Device %s not found\n", dev_name);
    }

    ibv_free_device_list(dev_list);
    return ctx;
}

/**
 * 查询端口属性
 */
int query_port_info(struct ibv_context *ctx, int port_num,
                    struct ibv_port_attr *port_attr) {
    int ret = ibv_query_port(ctx, port_num, port_attr);
    if (ret) {
        perror("ibv_query_port");
        return ret;
    }

    printf("\n端口 %d 信息:\n", port_num);
    printf("  状态: %s\n",
           port_attr->state == IBV_PORT_ACTIVE ? "Active" : "Inactive");
    printf("  最大MTU: %d\n", port_attr->max_mtu);
    printf("  活动MTU: %d\n", port_attr->active_mtu);
    printf("  LID: 0x%04x\n", port_attr->lid);
    printf("  最大消息大小: %u\n", port_attr->max_msg_sz);
    printf("  最大VL: %d\n", port_attr->max_vl_num);

    return 0;
}

/**
 * 完整的RDMA初始化
 */
rdma_context_t* rdma_init_full(const char *dev_name, int port_num) {
    rdma_context_t *rdma = calloc(1, sizeof(rdma_context_t));
    if (!rdma) {
        perror("calloc");
        return NULL;
    }

    /* 1. 打开设备 */
    if (dev_name) {
        rdma->ctx = open_rdma_device(dev_name);
    } else {
        /* 使用第一个可用设备 */
        struct ibv_device **dev_list;
        int num_devices;
        dev_list = ibv_get_device_list(&num_devices);
        if (num_devices > 0) {
            rdma->ctx = ibv_open_device(dev_list[0]);
        }
        ibv_free_device_list(dev_list);
    }

    if (!rdma->ctx) {
        fprintf(stderr, "Failed to open RDMA device\n");
        free(rdma);
        return NULL;
    }

    printf("成功打开RDMA设备\n");

    /* 2. 查询设备属性 */
    struct ibv_device_attr device_attr;
    if (ibv_query_device(rdma->ctx, &device_attr) == 0) {
        printf("设备属性:\n");
        printf("  固件版本: %s\n", device_attr.fw_ver);
        printf("  最大QP数: %d\n", device_attr.max_qp);
        printf("  最大CQ数: %d\n", device_attr.max_cq);
        printf("  最大MR数: %d\n", device_attr.max_mr);
        printf("  最大PD数: %d\n", device_attr.max_pd);
    }

    /* 3. 分配保护域 */
    rdma->pd = ibv_alloc_pd(rdma->ctx);
    if (!rdma->pd) {
        perror("ibv_alloc_pd");
        goto err_pd;
    }
    printf("成功分配保护域\n");

    /* 4. 创建完成事件通道（用于异步通知） */
    rdma->comp_channel = ibv_create_comp_channel(rdma->ctx);
    if (!rdma->comp_channel) {
        perror("ibv_create_comp_channel");
        /* 非致命错误，继续 */
    }

    /* 5. 创建完成队列 */
    rdma->cq = ibv_create_cq(rdma->ctx, CQ_SIZE, NULL,
                             rdma->comp_channel, 0);
    if (!rdma->cq) {
        perror("ibv_create_cq");
        goto err_cq;
    }
    printf("成功创建CQ，大小: %d\n", CQ_SIZE);

    /* 6. 请求CQ通知 */
    if (rdma->comp_channel) {
        ibv_req_notify_cq(rdma->cq, 0);
    }

    /* 7. 设置端口 */
    rdma->port_num = port_num > 0 ? port_num : 1;

    struct ibv_port_attr port_attr;
    query_port_info(rdma->ctx, rdma->port_num, &port_attr);

    /* 8. 查找GID索引 */
    rdma->gid_idx = 0;  /* 默认使用GID 0 */

    printf("\nRDMA初始化完成!\n");
    return rdma;

err_cq:
    if (rdma->comp_channel) ibv_destroy_comp_channel(rdma->comp_channel);
    ibv_dealloc_pd(rdma->pd);
err_pd:
    ibv_close_device(rdma->ctx);
    free(rdma);
    return NULL;
}

/**
 * 清理RDMA资源
 */
void rdma_cleanup(rdma_context_t *rdma) {
    if (!rdma) return;

    if (rdma->qp) {
        ibv_destroy_qp(rdma->qp);
        printf("销毁QP\n");
    }
    if (rdma->cq) {
        ibv_destroy_cq(rdma->cq);
        printf("销毁CQ\n");
    }
    if (rdma->comp_channel) {
        ibv_destroy_comp_channel(rdma->comp_channel);
    }
    if (rdma->pd) {
        ibv_dealloc_pd(rdma->pd);
        printf("释放PD\n");
    }
    if (rdma->ctx) {
        ibv_close_device(rdma->ctx);
        printf("关闭设备\n");
    }
    free(rdma);
}
