/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 276
 * Language: c
 * Block ID: 0cbdfc3e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 通用地址结构
struct sockaddr {
    sa_family_t sa_family;    // 地址族 AF_INET/AF_INET6/AF_UNIX
    char        sa_data[14];  // 地址数据
};

// IPv4地址结构
struct sockaddr_in {
    sa_family_t    sin_family; // AF_INET
    in_port_t      sin_port;   // 端口号(网络字节序)
    struct in_addr sin_addr;   // IPv4地址
    char           sin_zero[8];// 填充
};

// IPv6地址结构
struct sockaddr_in6 {
    sa_family_t     sin6_family;   // AF_INET6
    in_port_t       sin6_port;     // 端口号
    uint32_t        sin6_flowinfo; // 流信息
    struct in6_addr sin6_addr;     // IPv6地址
    uint32_t        sin6_scope_id; // 作用域ID
};

// Unix域地址结构
struct sockaddr_un {
    sa_family_t sun_family; // AF_UNIX/AF_LOCAL
    char        sun_path[108]; // 路径名
};
