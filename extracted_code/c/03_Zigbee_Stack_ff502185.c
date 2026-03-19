/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 606
 * Language: c
 * Block ID: ff502185
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 按需距离矢量路由 (AODV简化版)
 * Zigbee使用簇树路由 + AODV按需路由
 */

// 簇树路由计算下一跳
nwk_addr_t nwk_ctree_route(nwk_addr_t dest_addr) {
    // 如果目的地址是自身子节点
    if (is_child(dest_addr)) {
        return dest_addr;  // 直接发送
    }

    // 如果目的地址是父节点或父节点的后代
    if (is_parent_descendant(dest_addr)) {
        return nwk_pib.parent_addr;  // 发给父节点
    }

    // 计算公共祖先，路由到对应子路由器
    nwk_addr_t next_hop = find_common_ancestor_next_hop(dest_addr);
    return next_hop;
}

// Cskip计算 (确定子路由器地址空间大小)
uint16_t nwk_cskip(uint8_t d) {
    // d: 当前深度
    // Cm: 最大子节点数, Rm: 最大路由器数, Lm: 最大深度
    if (d >= nwk_pib.max_depth) {
        return 0;
    }

    uint16_t cm = nwk_pib.max_children;
    uint16_t rm = nwk_pib.max_routers;
    uint16_t lm = nwk_pib.max_depth;

    if (rm == 0) {
        return 1 + cm * (lm - d - 1);
    } else {
        return (1 + cm - rm - cm * pow(rm, lm - d - 1)) / (1 - rm);
    }
}

// 路由发现
void nwk_route_discovery(nwk_addr_t dest_addr) {
    // 创建路由请求命令帧
    nwk_route_req_cmd_t route_req;
    route_req.command_frame_id = NWK_CMD_ROUTE_REQUEST;
    route_req.command_options = 0;
    route_req.route_request_id = get_next_route_request_id();
    route_req.dest_addr = dest_addr;
    route_req.path_cost = 0;

    // 广播路由请求
    nwk_broadcast(NWK_FRAME_COMMAND, &route_req, sizeof(route_req));
}

// 处理路由请求
void nwk_handle_route_req(nwk_route_req_cmd_t* req, nwk_addr_t src) {
    // 更新路由发现表
    update_route_discovery(req->route_request_id, src, req->path_cost);

    // 如果是目的节点或知道路由，发送路由应答
    if (req->dest_addr == nwk_pib.nwk_addr || has_route(req->dest_addr)) {
        nwk_send_route_reply(req, src);
    } else {
        // 转发路由请求，增加路径成本
        req->path_cost += link_cost_calc(src);
        nwk_broadcast(NWK_FRAME_COMMAND, req, sizeof(*req));
    }
}
