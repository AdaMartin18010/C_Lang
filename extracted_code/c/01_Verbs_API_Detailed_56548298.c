/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 1915
 * Language: c
 * Block ID: 56548298
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA错误处理与调试
 */

#include <infiniband/verbs.h>
#include <stdio.h>

/**
 * 详细的WC错误处理
 */
void handle_wc_error(struct ibv_wc *wc) {
    fprintf(stderr, "Work Completion Error:\n");
    fprintf(stderr, "  WR ID: %lu\n", (unsigned long)wc->wr_id);
    fprintf(stderr, "  Status: %s (%d)\n",
            ibv_wc_status_str(wc->status), wc->status);
    fprintf(stderr, "  Opcode: %s\n", ibv_wc_opcode_str(wc->opcode));
    fprintf(stderr, "  QP Number: %u\n", wc->qp_num);

    switch (wc->status) {
        case IBV_WC_LOC_LEN_ERR:
            fprintf(stderr, "  原因: 本地长度错误，SGE长度超出限制\n");
            break;
        case IBV_WC_LOC_QP_OP_ERR:
            fprintf(stderr, "  原因: 本地QP操作错误，QP状态不正确\n");
            break;
        case IBV_WC_LOC_PROT_ERR:
            fprintf(stderr, "  原因: 本地保护错误，MR访问权限不足\n");
            break;
        case IBV_WC_WR_FLUSH_ERR:
            fprintf(stderr, "  原因: WR被Flush，QP进入Error状态\n");
            break;
        case IBV_WC_MW_BIND_ERR:
            fprintf(stderr, "  原因: 内存窗口绑定错误\n");
            break;
        case IBV_WC_BAD_RESP_ERR:
            fprintf(stderr, "  原因: 错误响应\n");
            break;
        case IBV_WC_LOC_ACCESS_ERR:
            fprintf(stderr, "  原因: 本地访问错误\n");
            break;
        case IBV_WC_REM_INV_REQ_ERR:
            fprintf(stderr, "  原因: 远程无效请求\n");
            break;
        case IBV_WC_REM_ACCESS_ERR:
            fprintf(stderr, "  原因: 远程访问错误，检查RKey和权限\n");
            break;
        case IBV_WC_REM_OP_ERR:
            fprintf(stderr, "  原因: 远程操作错误\n");
            break;
        case IBV_WC_RETRY_EXC_ERR:
            fprintf(stderr, "  原因: 重试次数超限，检查网络连接\n");
            break;
        case IBV_WC_RNR_RETRY_EXC_ERR:
            fprintf(stderr, "  原因: RNR重试次数超限，接收队列可能为空\n");
            break;
        case IBV_WC_LOC_RDD_VIOL_ERR:
        case IBV_WC_REM_INV_RD_REQ_ERR:
        case IBV_WC_REM_ABORT_ERR:
        case IBV_WC_FATAL_ERR:
        case IBV_WC_RESP_TIMEOUT_ERR:
        case IBV_WC_GENERAL_ERR:
        default:
            fprintf(stderr, "  原因: 其他错误\n");
            break;
    }
}

/**
 * 查询异步事件
 */
void check_async_events(struct ibv_context *ctx) {
    struct ibv_async_event event;

    /* 非阻塞检查 */
    if (ibv_get_async_event(ctx, &event) == 0) {
        printf("异步事件:\n");
        printf("  事件类型: ");

        switch (event.event_type) {
            case IBV_EVENT_CQ_ERR:
                printf("CQ错误\n");
                break;
            case IBV_EVENT_QP_FATAL:
                printf("QP致命错误\n");
                break;
            case IBV_EVENT_QP_REQ_ERR:
                printf("QP请求错误\n");
                break;
            case IBV_EVENT_QP_ACCESS_ERR:
                printf("QP访问错误\n");
                break;
            case IBV_EVENT_COMM_EST:
                printf("通信建立\n");
                break;
            case IBV_EVENT_SQ_DRAINED:
                printf("SQ排空\n");
                break;
            case IBV_EVENT_PATH_MIG:
                printf("路径迁移\n");
                break;
            case IBV_EVENT_PATH_MIG_ERR:
                printf("路径迁移错误\n");
                break;
            case IBV_EVENT_DEVICE_FATAL:
                printf("设备致命错误\n");
                break;
            case IBV_EVENT_PORT_ACTIVE:
                printf("端口激活\n");
                break;
            case IBV_EVENT_PORT_ERR:
                printf("端口错误\n");
                break;
            case IBV_EVENT_LID_CHANGE:
                printf("LID变更\n");
                break;
            case IBV_EVENT_PKEY_CHANGE:
                printf("PKey变更\n");
                break;
            case IBV_EVENT_SM_CHANGE:
                printf("子网管理器变更\n");
                break;
            case IBV_EVENT_SRQ_ERR:
                printf("SRQ错误\n");
                break;
            case IBV_EVENT_SRQ_LIMIT_REACHED:
                printf("SRQ达到限制\n");
                break;
            case IBV_EVENT_QP_LAST_WQE_REACHED:
                printf("QP最后一个WQE到达\n");
                break;
            case IBV_EVENT_CLIENT_REREGISTER:
                printf("客户端重新注册\n");
                break;
            case IBV_EVENT_GID_CHANGE:
                printf("GID变更\n");
                break;
            default:
                printf("未知事件\n");
                break;
        }

        ibv_ack_async_event(&event);
    }
}

/**
 * 检查并恢复QP错误状态
 */
int recover_qp_error(struct ibv_qp *qp) {
    struct ibv_qp_attr attr;
    struct ibv_qp_init_attr init_attr;

    /* 查询当前状态 */
    if (ibv_query_qp(qp, &attr, IBV_QP_STATE, &init_attr)) {
        return -1;
    }

    if (attr.qp_state == IBV_QPS_ERR) {
        printf("QP处于Error状态，尝试重置...\n");

        /* 将QP重置到RST状态 */
        memset(&attr, 0, sizeof(attr));
        attr.qp_state = IBV_QPS_RESET;

        if (ibv_modify_qp(qp, &attr, IBV_QP_STATE)) {
            perror("ibv_modify_qp (RESET)");
            return -1;
        }

        printf("QP已重置\n");
        return 1;  /* 需要重新初始化 */
    }

    return 0;  /* 正常 */
}

/**
 * 诊断RDMA连接
 */
void diagnose_connection(struct ibv_qp *qp, int port_num) {
    struct ibv_qp_attr attr;
    struct ibv_qp_init_attr init_attr;

    printf("\n=== 连接诊断 ===\n");

    /* QP状态 */
    if (ibv_query_qp(qp, &attr, IBV_QP_STATE, &init_attr) == 0) {
        const char *states[] = {
            "RESET", "INIT", "RTR", "RTS", "SQD", "SQE", "ERR"
        };
        printf("QP状态: %s\n",
               attr.qp_state < 7 ? states[attr.qp_state] : "UNKNOWN");
    }

    /* 路径MTU */
    if (ibv_query_qp(qp, &attr, IBV_QP_PATH_MTU, &init_attr) == 0) {
        printf("Path MTU: %d\n", attr.path_mtu);
    }

    /* 超时设置 */
    if (ibv_query_qp(qp, &attr, IBV_QP_TIMEOUT, &init_attr) == 0) {
        printf"超时: %d (约 %d μs)\n", attr.timeout,
               (1 << attr.timeout) * 4);
    }

    /* 重试次数 */
    if (ibv_query_qp(qp, &attr, IBV_QP_RETRY_CNT, &init_attr) == 0) {
        printf("重试次数: %d\n", attr.retry_cnt);
        printf("RNR重试次数: %d\n", attr.rnr_retry);
    }

    printf("================\n\n");
}

/**
 * 打印详细的设备信息
 */
void print_device_info(struct ibv_context *ctx) {
    struct ibv_device_attr attr;

    if (ibv_query_device(ctx, &attr)) {
        perror("ibv_query_device");
        return;
    }

    printf("\n=== 设备信息 ===\n");
    printf("固件版本: %s\n", attr.fw_ver);
    printf("节点GUID: 0x%016llx\n", (unsigned long long)attr.node_guid);
    printf("系统图像GUID: 0x%016llx\n",
           (unsigned long long)attr.sys_image_guid);
    printf("最大MR大小: %llu GB\n",
           (unsigned long long)attr.max_mr_size / (1024*1024*1024));
    printf("页大小能力: 0x%08x\n", attr.page_size_cap);
    printf("最大QP数: %d\n", attr.max_qp);
    printf("最大CQ数: %d\n", attr.max_cq);
    printf("最大MR数: %d\n", attr.max_mr);
    printf("最大PD数: %d\n", attr.max_pd);
    printf("最大SGE数: %d\n", attr.max_sge);
    printf("最大CQE数: %d\n", attr.max_cqe);
    printf("最大内联数据: %d\n", attr.max_inline_data);
    printf("================\n\n");
}
