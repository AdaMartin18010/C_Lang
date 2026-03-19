/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 529
 * Language: c
 * Block ID: 3cd7ef86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 网络地址分配
// 16位网络地址格式 (Cskip路由)
// Cm: 子节点最大数, Rm: 子路由器最大数, Lm: 网络最大深度

typedef uint16_t nwk_addr_t;

// 网络层帧控制
typedef union {
    uint16_t value;
    struct {
        uint16_t frame_type : 2;
        uint16_t protocol_version : 4;
        uint16_t discover_route : 2;
        uint16_t multicast : 1;
        uint16_t security : 1;
        uint16_t source_route : 1;
        uint16_t dest_ieee_addr : 1;
        uint16_t src_ieee_addr : 1;
        uint16_t reserved : 3;
    } bits;
} nwk_frame_ctrl_t;

// 网络层帧头
typedef struct {
    nwk_frame_ctrl_t frame_ctrl;
    nwk_addr_t dest_addr;
    nwk_addr_t src_addr;
    uint8_t radius;           // 最大跳数，每转发一次减1
    uint8_t seq_num;          // 序列号，用于去重
    uint64_t dest_ieee_addr;  // 可选
    uint64_t src_ieee_addr;   // 可选
} nwk_header_t;

// 邻居表条目
typedef struct {
    uint64_t ext_addr;        // 扩展地址
    nwk_addr_t nwk_addr;      // 网络地址
    uint8_t device_type;      // 0=协调器, 1=路由器, 2=终端
    uint8_t rx_on_when_idle;  // 空闲时接收
    uint8_t relationship;     // 关系
    uint8_t depth;            // 网络深度
    uint8_t lqi;              // 链路质量
    uint8_t outgoing_cost;    // 出链路成本
    uint8_t age;              // 老化计数
} neighbor_entry_t;

// 路由表条目
typedef struct {
    nwk_addr_t dest_addr;     // 目的地址
    uint8_t status;           // 状态: 0=激活, 1=发现中, 2=发现失败, 3=不活跃
    uint8_t no_route_cache;   // 不缓存路由
    uint8_t many_to_one;      // 多对一标志
    uint8_t route_record_req; // 需要路由记录
    nwk_addr_t next_hop_addr; // 下一跳地址
} route_entry_t;

// NWK PIB属性
typedef struct {
    uint64_t ext_pan_id;      // 扩展PAN ID
    nwk_addr_t nwk_addr;      // 网络地址
    nwk_addr_t parent_addr;   // 父节点地址
    uint8_t depth;            // 网络深度
    uint8_t channel;          // 当前信道
    uint8_t protocol_version; // 协议版本
    uint8_t stack_profile;    // 协议栈配置文件
    uint8_t max_children;     // 最大子节点数
    uint8_t max_routers;      // 最大路由器数
    uint8_t max_depth;        // 最大网络深度
    uint8_t beacon_order;     // 信标阶
    uint8_t tx_total;         // 传输总数
    uint8_t tx_fail;          // 传输失败数
} nwk_pib_t;
