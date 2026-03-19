/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 724
 * Language: c
 * Block ID: 16432cb5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file ethercat_master.c
 * @brief IgH EtherCAT主站开发示例
 * @details 演示如何使用IgH EtherCAT Master库开发主站应用
 *
 * 依赖: libethercat-devel
 * 编译: gcc -o ethercat_master ethercat_master.c -lethercat -lpthread
 * 运行: sudo ./ethercat_master
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <sys/mman.h>
#include <ecrt.h>

/* EtherCAT配置 */
#define ECAT_DEVICE         "eth0"          /* EtherCAT网卡 */
#define TASK_FREQUENCY      1000            /* 任务频率: 1kHz */
#define TASK_PERIOD_NS      (1000000000 / TASK_FREQUENCY)

/* 从站配置示例 - Beckhoff EL2008 (8路数字输出) */
#define EL2008_VENDOR_ID    0x00000002
#define EL2008_PRODUCT_ID   0x07D83052

/* 从站配置示例 - Beckhoff EL1018 (8路数字输入) */
#define EL1018_VENDOR_ID    0x00000002
#define EL1018_PRODUCT_ID   0x03FA3052

/* 从站位置 */
#define SLAVE_DO_POSITION   0, 0    /* 总线位置0, 从站位置0 */
#define SLAVE_DI_POSITION   0, 1    /* 总线位置0, 从站位置1 */

/* 全局变量 */
static volatile int g_running = 1;
static ec_master_t *g_master = NULL;
static ec_master_state_t g_master_state = {};

/* 从站句柄 */
static ec_slave_config_t *g_sc_el2008 = NULL;
static ec_slave_config_t *g_sc_el1018 = NULL;
static ec_slave_config_state_t g_sc_el2008_state = {};
static ec_slave_config_state_t g_sc_el1018_state = {};

/* 过程数据域 */
static ec_domain_t *g_domain = NULL;
static ec_domain_state_t g_domain_state = {};
static uint8_t *g_domain_pd = NULL;  /* 过程数据指针 */

/* 过程数据偏移 */
static unsigned int g_off_el2008_out = 0;
static unsigned int g_off_el1018_in = 0;

/* 应用周期计数 */
static uint32_t g_cycle_count = 0;

/* 性能统计 */
static long long g_min_cycle_time = 1000000000LL;
static long long g_max_cycle_time = 0;
static long long g_total_cycle_time = 0;
static int g_stat_count = 0;

/**
 * @brief PDO条目配置
 * @details EL2008数字输出模块的PDO配置
 */
static const ec_pdo_entry_reg_t g_domain_regs[] = {
    /* EL2008: 8路数字输出 */
    {SLAVE_DO_POSITION, EL2008_VENDOR_ID, EL2008_PRODUCT_ID,
     0x7000, 0x01, &g_off_el2008_out},
    /* EL1018: 8路数字输入 */
    {SLAVE_DI_POSITION, EL1018_VENDOR_ID, EL1018_PRODUCT_ID,
     0x6000, 0x01, &g_off_el1018_in},
    {}
};

/* EL2008输出PDO映射 */
static const ec_pdo_entry_info_t g_el2008_entries[] = {
    {0x7000, 0x01, 8}   /* 8位输出 */
};

static const ec_pdo_info_t g_el2008_pdos[] = {
    {0x1600, 1, g_el2008_entries}
};

static const ec_sync_info_t g_el2008_syncs[] = {
    {0, EC_DIR_OUTPUT, 1, g_el2008_pdos, EC_WD_ENABLE},
    {0xff}  /* 结束标记 */
};

/* EL1018输入PDO映射 */
static const ec_pdo_entry_info_t g_el1018_entries[] = {
    {0x6000, 0x01, 8}   /* 8位输入 */
};

static const ec_pdo_info_t g_el1018_pdos[] = {
    {0x1A00, 1, g_el1018_entries}
};

static const ec_sync_info_t g_el1018_syncs[] = {
    {0, EC_DIR_INPUT, 1, g_el1018_pdos, EC_WD_DISABLE},
    {0xff}  /* 结束标记 */
};

/**
 * @brief 获取当前时间 (纳秒)
 */
static inline long long get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Received signal, stopping...\n");
    g_running = 0;
}

/**
 * @brief 检查主站状态
 */
static void check_master_state(void)
{
    ec_master_state_t ms;
    ecrt_master_state(g_master, &ms);

    if (ms.slaves_responding != g_master_state.slaves_responding) {
        printf("[INFO] %u slave(s) responding\n", ms.slaves_responding);
    }

    if (ms.al_states != g_master_state.al_states) {
        printf("[INFO] AL states: 0x%02X\n", ms.al_states);
    }

    if (ms.link_up != g_master_state.link_up) {
        printf("[INFO] Link is %s\n", ms.link_up ? "up" : "down");
    }

    g_master_state = ms;
}

/**
 * @brief 检查从站配置状态
 */
static void check_slave_config_states(void)
{
    ec_slave_config_state_t s;

    /* 检查EL2008状态 */
    ecrt_slave_config_state(g_sc_el2008, &s);
    if (s.al_state != g_sc_el2008_state.al_state) {
        printf("[INFO] EL2008: State 0x%02X\n", s.al_state);
    }
    if (s.online != g_sc_el2008_state.online) {
        printf("[INFO] EL2008: %s\n", s.online ? "online" : "offline");
    }
    if (s.operational != g_sc_el2008_state.operational) {
        printf("[INFO] EL2008: %s\n", s.operational ? "operational" : "not operational");
    }
    g_sc_el2008_state = s;

    /* 检查EL1018状态 */
    ecrt_slave_config_state(g_sc_el1018, &s);
    if (s.al_state != g_sc_el1018_state.al_state) {
        printf("[INFO] EL1018: State 0x%02X\n", s.al_state);
    }
    if (s.online != g_sc_el1018_state.online) {
        printf("[INFO] EL1018: %s\n", s.online ? "online" : "offline");
    }
    if (s.operational != g_sc_el1018_state.operational) {
        printf("[INFO] EL1018: %s\n", s.operational ? "operational" : "not operational");
    }
    g_sc_el1018_state = s;
}

/**
 * @brief 检查域状态
 */
static void check_domain_state(void)
{
    ec_domain_state_t ds;
    ecrt_domain_state(g_domain, &ds);

    if (ds.working_counter != g_domain_state.working_counter) {
        printf("[INFO] Domain: WC %u\n", ds.working_counter);
    }

    if (ds.wc_state != g_domain_state.wc_state) {
        printf("[INFO] Domain: State %u\n", ds.wc_state);
    }

    g_domain_state = ds;
}

/**
 * @brief 初始化EtherCAT主站
 * @return 成功返回0，失败返回-1
 */
static int init_ethercat_master(void)
{
    printf("[INFO] Initializing EtherCAT master...\n");

    /* 请求主站实例 */
    g_master = ecrt_request_master(0);
    if (!g_master) {
        fprintf(stderr, "[ERROR] Failed to request master\n");
        return -1;
    }

    /* 创建过程数据域 */
    g_domain = ecrt_master_create_domain(g_master);
    if (!g_domain) {
        fprintf(stderr, "[ERROR] Failed to create domain\n");
        return -1;
    }

    /* 配置EL2008从站 */
    printf("[INFO] Configuring EL2008 (Digital Output)...\n");
    g_sc_el2008 = ecrt_master_slave_config(g_master,
        SLAVE_DO_POSITION, EL2008_VENDOR_ID, EL2008_PRODUCT_ID);
    if (!g_sc_el2008) {
        fprintf(stderr, "[ERROR] Failed to get EL2008 slave config\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(g_sc_el2008, EC_END, g_el2008_syncs)) {
        fprintf(stderr, "[ERROR] Failed to config EL2008 PDOs\n");
        return -1;
    }

    /* 配置EL1018从站 */
    printf("[INFO] Configuring EL1018 (Digital Input)...\n");
    g_sc_el1018 = ecrt_master_slave_config(g_master,
        SLAVE_DI_POSITION, EL1018_VENDOR_ID, EL1018_PRODUCT_ID);
    if (!g_sc_el1018) {
        fprintf(stderr, "[ERROR] Failed to get EL1018 slave config\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(g_sc_el1018, EC_END, g_el1018_syncs)) {
        fprintf(stderr, "[ERROR] Failed to config EL1018 PDOs\n");
        return -1;
    }

    /* 注册PDO条目 */
    if (ecrt_domain_reg_pdo_entry_list(g_domain, g_domain_regs)) {
        fprintf(stderr, "[ERROR] Failed to register PDO entries\n");
        return -1;
    }

    /* 激活主站配置 */
    printf("[INFO] Activating master configuration...\n");
    if (ecrt_master_activate(g_master)) {
        fprintf(stderr, "[ERROR] Failed to activate master\n");
        return -1;
    }

    /* 获取过程数据指针 */
    g_domain_pd = ecrt_domain_data(g_domain);
    if (!g_domain_pd) {
        fprintf(stderr, "[ERROR] Failed to get domain data pointer\n");
        return -1;
    }

    printf("[INFO] EtherCAT master initialized successfully\n");
    return 0;
}

/**
 * @brief 释放EtherCAT资源
 */
static void release_ethercat_master(void)
{
    printf("[INFO] Releasing EtherCAT master...\n");

    if (g_master) {
        ecrt_master_deactivate(g_master);
        ecrt_release_master(g_master);
        g_master = NULL;
    }
}

/**
 * @brief 运行主循环
 */
static void run_main_loop(void)
{
    struct timespec wakeup_time;
    long long start_time, end_time, cycle_time;

    /* 设置起始时间 */
    clock_gettime(CLOCK_MONOTONIC, &wakeup_time);

    printf("[INFO] Starting cyclic task (Frequency: %d Hz)...\n", TASK_FREQUENCY);
    printf("[INFO] Press Ctrl+C to stop\n\n");

    while (g_running) {
        start_time = get_time_ns();

        /* 接收EtherCAT数据 */
        ecrt_master_receive(g_master);
        ecrt_domain_process(g_domain);

        /* 检查状态 (每100周期) */
        if (g_cycle_count % 100 == 0) {
            check_domain_state();
            check_master_state();
            check_slave_config_states();
        }

        /* ==================== 应用逻辑 ==================== */

        /* 读取输入 (EL1018) */
        uint8_t input_data = EC_READ_U8(g_domain_pd + g_off_el1018_in);

        /* 处理逻辑: 跑马灯效果 */
        uint8_t output_data = 0;
        if (g_sc_el2008_state.operational) {
            /* 简单的跑马灯模式 */
            int led_index = (g_cycle_count / 100) % 8;
            output_data = (1 << led_index);
        }

        /* 写入输出 (EL2008) */
        EC_WRITE_U8(g_domain_pd + g_off_el2008_out, output_data);

        /* 周期性打印信息 */
        if (g_cycle_count % 1000 == 0) {
            printf("[DATA] Input: 0x%02X | Output: 0x%02X | Cycle: %u\n",
                   input_data, output_data, g_cycle_count);
        }

        /* ================================================= */

        /* 发送EtherCAT数据 */
        ecrt_domain_queue(g_domain);
        ecrt_master_send(g_master);

        g_cycle_count++;

        /* 统计周期时间 */
        end_time = get_time_ns();
        cycle_time = end_time - start_time;

        if (cycle_time < g_min_cycle_time) g_min_cycle_time = cycle_time;
        if (cycle_time > g_max_cycle_time) g_max_cycle_time = cycle_time;
        g_total_cycle_time += cycle_time;
        g_stat_count++;

        /* 等待下一个周期 */
        wakeup_time.tv_nsec += TASK_PERIOD_NS;
        while (wakeup_time.tv_nsec >= 1000000000L) {
            wakeup_time.tv_nsec -= 1000000000L;
            wakeup_time.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wakeup_time, NULL);
    }
}

/**
 * @brief 打印统计信息
 */
static void print_statistics(void)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║            EtherCAT Performance Statistics               ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ Total Cycles:    %u                                       ║\n", g_cycle_count);
    printf("║ Min Cycle Time:  %lld μs                                 ║\n", g_min_cycle_time / 1000);
    printf("║ Max Cycle Time:  %lld μs                                 ║\n", g_max_cycle_time / 1000);
    printf("║ Avg Cycle Time:  %lld μs                                 ║\n",
           g_stat_count > 0 ? (g_total_cycle_time / g_stat_count) / 1000 : 0);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       IgH EtherCAT Master Demo Application               ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
    }

    /* 初始化EtherCAT */
    if (init_ethercat_master() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize EtherCAT\n");
        return 1;
    }

    /* 运行主循环 */
    run_main_loop();

    /* 打印统计 */
    print_statistics();

    /* 释放资源 */
    release_ethercat_master();

    printf("\n[INFO] Program completed\n");

    return 0;
}
