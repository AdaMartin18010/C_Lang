/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 146
 * Language: c
 * Block ID: 7f465814
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// server.h - 核心数据结构定义
// 所有模块共享的数据结构和函数声明

#ifndef __SERVER_H
#define __SERVER_H

#include "fmacros.h"
#include "config.h"
#include "solarisfixes.h"

// 版本定义
#define REDIS_VERSION "7.2.0"
#define REDIS_VERSION_NUM 0x00070200

// 核心数据结构 - 服务器状态
struct redisServer {
    // 通用配置
    pid_t pid;
    char *configfile;
    int hz;

    // 网络相关
    int port;
    int tcp_backlog;
    char *bindaddr[CONFIG_BINDADDR_MAX];
    int bindaddr_count;

    // 数据库
    redisDb *db;
    int dbnum;

    // 命令表
    struct dict *commands;
    struct dict *orig_commands;

    // 客户端
    list *clients;
    list *clients_to_close;

    // 持久化
    long long dirty;
    time_t lastsave;
    int saveparamsnum;
    struct saveparam *saveparams;

    // 统计信息
    long long stat_keyspace_hits;
    long long stat_keyspace_misses;

    // AOF
    int aof_state;
    int aof_fsync;
    char *aof_filename;

    // 复制
    char *masterhost;
    int masterport;

    // 集群
    int cluster_enabled;

    // 内存管理
    size_t maxmemory;
    int maxmemory_policy;
    int maxmemory_samples;
};

// 客户端结构
typedef struct client {
    uint64_t id;
    int fd;
    sds querybuf;
    int argc;
    robj **argv;
    struct redisCommand *cmd;
    int lastcmd;
    list *reply;
    unsigned long long obuf_soft_limit_reached_time;
    int flags;
    int authenticated;
} client;

// Redis 对象系统
typedef struct redisObject {
    unsigned type:4;
    unsigned encoding:4;
    unsigned lru:LRU_BITS;
    int refcount;
    void *ptr;
} robj;

#endif
