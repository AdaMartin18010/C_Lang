/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 404
 * Language: c
 * Block ID: e85a9109
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查询端口信息
struct ibv_port_attr port_attr;
ret = ibv_query_port(ctx, 1, &port_attr);
if (ret) {
    fprintf(stderr, "Failed to query port\n");
    return -1;
}

printf("Port Info:\n");
printf("  State: %d\n", port_attr.state);
printf("  LID: %d\n", port_attr.lid);
printf("  MTU: %d\n", port_attr.active_mtu);
printf("  Speed: %d\n", port_attr.active_speed);
