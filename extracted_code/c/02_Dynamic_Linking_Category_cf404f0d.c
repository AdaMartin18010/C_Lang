/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 83
 * Language: c
 * Block ID: cf404f0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ELF动态段结构
#include <elf.h>
#include <link.h>

// 动态段条目类型
enum DynamicTag {
    DT_NEEDED    = 1,   // 依赖的库名
    DT_PLTRELSZ  = 2,   // PLT重定位大小
    DT_PLTGOT    = 3,   // PLT/GOT地址
    DT_HASH      = 4,   // 符号哈希表
    DT_STRTAB    = 5,   // 字符串表
    DT_SYMTAB    = 6,   // 符号表
    DT_RELA      = 7,   // 重定位表
    DT_INIT      = 12,  // 初始化函数
    DT_FINI      = 13,  // 终止函数
    DT_RELASZ    = 8,   // 重定位表大小
    DT_RELAENT   = 9,   // 重定位条目大小
    DT_SYMBOLIC  = 16,  // 优先使用库内符号
    DT_GNU_HASH  = 0x6ffffef5,  // GNU哈希表
};

// 动态链接器接口
struct r_debug {
    int r_version;
    struct link_map *r_map;      // 已加载库列表
    Elf64_Addr r_brk;            // 调试事件地址
    enum {
        RT_CONSISTENT,           // 添加/删除库后
        RT_ADD,                  // 开始添加库
        RT_DELETE                // 开始删除库
    } r_state;
};

// 已加载模块描述
struct link_map {
    Elf64_Addr l_addr;           // 基址
    char *l_name;                // 库名
    Elf64_Dyn *l_ld;             // 动态段
    struct link_map *l_next;     // 下一个
    struct link_map *l_prev;     // 上一个
};
