/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 5064
 * Language: c
 * Block ID: 9ffff4e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file reliability_framework.c
 * @brief 系统可靠性框架实现
 * @details 包含看门狗、心跳检测、故障恢复等功能
 *
 * 编译: gcc -o reliability_framework reliability_framework.c
 * 运行: sudo ./reliability_framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

/* 看门狗配置 */
#define WATCHDOG_DEV        "/dev/watchdog"
#define WATCHDOG_TIMEOUT    30      /* 看门狗超时时间(秒) */

/* 心跳配置 */
#define HEARTBEAT_INTERVAL_MS   1000    /* 心跳间隔 */
#define HEARTBEAT_TIMEOUT_MS    5000    /* 心跳超时 */

/* 健康检查项 */
typedef enum {
    HEALTH_CPU = 0,
    HEALTH_MEMORY,
    HEALTH_DISK,
    HEALTH_NETWORK,
    HEALTH_APPLICATION,
    HEALTH_COUNT
} health_item_t;

/* 健康状态 */
typedef enum {
    STATUS_HEALTHY = 0,
    STATUS_WARNING,
    STATUS_CRITICAL,
    STATUS_FAILED
} health_status_t;

/* 健康检查结构 */
typedef struct {
    health_item_t item;
    health_status_t status;
    char description[128];
    uint64_t last_check;
    uint64_t failure_count;
} health_check_t;

/* 看门狗上下文 */
typedef struct {
    int fd;
    int timeout;
    bool enabled;
} watchdog_t;

/* 心跳上下文 */
typedef struct {
    uint64_t last_beat;
    uint64_t interval_ms;
    uint64_t timeout_ms;
    bool alive;
} heartbeat_t;

/* 可靠性管理器 */
typedef struct {
    watchdog_t watchdog;
    heartbeat_t heartbeat;
    health_check_t health_checks[HEALTH_COUNT];
    bool running;
    void (*on_failure)(health_item_t, const char *);
    void (*on_recovery)(health_item_t);
} reliability_manager_t;

static reliability_manager_t g_manager = {0};

/**
 * @brief 获取当前时间 (毫秒)
 */
static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * @brief 初始化硬件看门狗
 */
static int watchdog_init(watchdog_t *wd)
{
    wd->fd = open(WATCHDOG_DEV, O_RDWR);
    if (wd->fd < 0) {
        fprintf(stderr, "[WATCHDOG] Failed to open %s: %s\n",
                WATCHDOG_DEV, strerror(errno));
        fprintf(stderr, "[WATCHDOG] Running in software-only mode\n");
        wd->enabled = false;
        return 0;  /* 软看门狗模式 */
    }

    /* 获取当前超时时间 */
    int timeout;
    if (ioctl(wd->fd, WDIOC_GETTIMEOUT, &timeout) == 0) {
        printf("[WATCHDOG] Current timeout: %d seconds\n", timeout);
    }

    /* 设置超时时间 */
    wd->timeout = WATCHDOG_TIMEOUT;
    if (ioctl(wd->fd, WDIOC_SETTIMEOUT, &wd->timeout) == 0) {
        printf("[WATCHDOG] Timeout set to %d seconds\n", wd->timeout);
    }

    /* 获取支持的功能 */
    struct watchdog_info info;
    if (ioctl(wd->fd, WDIOC_GETSUPPORT, &info) == 0) {
        printf("[WATCHDOG] Identity: %s\n", info.identity);
    }

    wd->enabled = true;
    printf("[WATCHDOG] Hardware watchdog initialized\n");

    return 0;
}

/**
 * @brief 喂狗
 */
static void watchdog_feed(watchdog_t *wd)
{
    if (wd->enabled && wd->fd >= 0) {
        int ret = ioctl(wd->fd, WDIOC_KEEPALIVE, 0);
        if (ret < 0) {
            perror("WDIOC_KEEPALIVE");
        }
    }
}

/**
 * @brief 禁用看门狗 (正常关闭时)
 */
static void watchdog_disable(watchdog_t *wd)
{
    if (wd->enabled && wd->fd >= 0) {
        /* 发送 'V' 魔法字符禁用看门狗 */
        write(wd->fd, "V", 1);
        close(wd->fd);
        wd->enabled = false;
        printf("[WATCHDOG] Disabled\n");
    }
}

/**
 * @brief 检查CPU健康
 */
static health_status_t check_cpu_health(void)
{
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    double load1, load5, load15;
    if (fscanf(fp, "%lf %lf %lf", &load1, &load5, &load15) != 3) {
        fclose(fp);
        return STATUS_FAILED;
    }
    fclose(fp);

    /* 简单判断: 1分钟负载 > 8 为严重, > 4 为警告 */
    if (load1 > 8.0) {
        return STATUS_CRITICAL;
    } else if (load1 > 4.0) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查内存健康
 */
static health_status_t check_memory_health(void)
{
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    char line[256];
    uint64_t total = 0, available = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %lu", &total) == 1) {
            /* kB */
        }
        if (sscanf(line, "MemAvailable: %lu", &available) == 1) {
            /* kB */
        }
    }
    fclose(fp);

    if (total == 0) {
        return STATUS_FAILED;
    }

    /* 计算可用内存比例 */
    double ratio = (double)available / total;

    if (ratio < 0.1) {
        return STATUS_CRITICAL;
    } else if (ratio < 0.2) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查磁盘健康
 */
static health_status_t check_disk_health(void)
{
    /* 检查根分区可用空间 */
    FILE *fp = popen("df / | tail -1 | awk '{print $5}' | tr -d '%'", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    int usage;
    if (fscanf(fp, "%d", &usage) != 1) {
        pclose(fp);
        return STATUS_FAILED;
    }
    pclose(fp);

    if (usage > 95) {
        return STATUS_CRITICAL;
    } else if (usage > 85) {
        return STATUS_WARNING;
    }

    return STATUS_HEALTHY;
}

/**
 * @brief 检查网络健康
 */
static health_status_t check_network_health(void)
{
    /* 简单检查: 默认路由是否存在 */
    FILE *fp = fopen("/proc/net/route", "r");
    if (!fp) {
        return STATUS_FAILED;
    }

    char line[256];
    bool has_default_route = false;

    /* 跳过标题行 */
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        char iface[16];
        unsigned int dest;
        if (sscanf(line, "%s %x", iface, &dest) == 2) {
            if (dest == 0) {  /* 默认路由 */
                has_default_route = true;
                break;
            }
        }
    }
    fclose(fp);

    return has_default_route ? STATUS_HEALTHY : STATUS_WARNING;
}

/**
 * @brief 执行健康检查
 */
static void perform_health_checks(reliability_manager_t *mgr)
{
    uint64_t now = get_time_ms();

    /* CPU */
    health_status_t cpu_status = check_cpu_health();
    if (cpu_status != mgr->health_checks[HEALTH_CPU].status) {
        if (cpu_status == STATUS_HEALTHY) {
            if (mgr->on_recovery) {
                mgr->on_recovery(HEALTH_CPU);
            }
        } else {
            if (mgr->on_failure) {
                mgr->on_failure(HEALTH_CPU, "CPU load high");
            }
        }
    }
    mgr->health_checks[HEALTH_CPU].status = cpu_status;
    mgr->health_checks[HEALTH_CPU].last_check = now;

    /* Memory */
    health_status_t mem_status = check_memory_health();
    if (mem_status != mgr->health_checks[HEALTH_MEMORY].status) {
        if (mem_status == STATUS_HEALTHY) {
            if (mgr->on_recovery) mgr->on_recovery(HEALTH_MEMORY);
        } else {
            if (mgr->on_failure) mgr->on_failure(HEALTH_MEMORY, "Memory low");
        }
    }
    mgr->health_checks[HEALTH_MEMORY].status = mem_status;
    mgr->health_checks[HEALTH_MEMORY].last_check = now;

    /* Disk */
    health_status_t disk_status = check_disk_health();
    mgr->health_checks[HEALTH_DISK].status = disk_status;
    mgr->health_checks[HEALTH_DISK].last_check = now;

    /* Network */
    health_status_t net_status = check_network_health();
    mgr->health_checks[HEALTH_NETWORK].status = net_status;
    mgr->health_checks[HEALTH_NETWORK].last_check = now;
}

/**
 * @brief 打印健康状态
 */
static void print_health_status(const reliability_manager_t *mgr)
{
    const char *status_str[] = {"HEALTHY", "WARNING", "CRITICAL", "FAILED"};
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};

    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║              System Health Status                        ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");

    for (int i = 0; i < HEALTH_COUNT; i++) {
        printf("║ %-12s: %-10s\n",
               item_str[i],
               status_str[mgr->health_checks[i].status]);
    }

    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Watchdog:    %s\n",
           mgr->watchdog.enabled ? "ENABLED" : "SOFTWARE ONLY");
    printf("║ Heartbeat:    %s\n",
           mgr->heartbeat.alive ? "ALIVE" : "TIMEOUT");
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief 故障回调
 */
static void on_health_failure(health_item_t item, const char *desc)
{
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};
    fprintf(stderr, "[HEALTH] %s FAILURE: %s\n", item_str[item], desc);

    /* 可以在这里添加告警通知 */
}

/**
 * @brief 恢复回调
 */
static void on_health_recovery(health_item_t item)
{
    const char *item_str[] = {"CPU", "Memory", "Disk", "Network", "Application"};
    printf("[HEALTH] %s RECOVERED\n", item_str[item]);
}

/**
 * @brief 信号处理
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Shutting down gracefully...\n");
    g_manager.running = false;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║        System Reliability Framework Demo                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 初始化 */
    g_manager.on_failure = on_health_failure;
    g_manager.on_recovery = on_health_recovery;
    g_manager.heartbeat.interval_ms = HEARTBEAT_INTERVAL_MS;
    g_manager.heartbeat.timeout_ms = HEARTBEAT_TIMEOUT_MS;
    g_manager.running = true;

    /* 初始化看门狗 */
    if (watchdog_init(&g_manager.watchdog) != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize watchdog\n");
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("[INFO] System reliability monitoring started\n");
    printf("[INFO] Press Ctrl+C to stop\n\n");

    uint64_t last_health_check = 0;
    uint64_t last_status_print = 0;

    /* 主循环 */
    while (g_manager.running) {
        uint64_t now = get_time_ms();

        /* 喂狗 */
        watchdog_feed(&g_manager.watchdog);
        g_manager.heartbeat.last_beat = now;
        g_manager.heartbeat.alive = true;

        /* 执行健康检查 (每5秒) */
        if (now - last_health_check >= 5000) {
            perform_health_checks(&g_manager);
            last_health_check = now;
        }

        /* 打印状态 (每10秒) */
        if (now - last_status_print >= 10000) {
            print_health_status(&g_manager);
            last_status_print = now;
        }

        usleep(100000);  /* 100ms */
    }

    /* 清理 */
    watchdog_disable(&g_manager.watchdog);

    printf("\n[INFO] System reliability monitoring stopped\n");

    return 0;
}
