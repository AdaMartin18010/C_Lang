/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 1762
 * Language: c
 * Block ID: 61eff89d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// modbus_gateway.c - Modbus RTU/TCP网关
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus-rtu.h>
#include <gpiod.h>
#include <json-c/json.h>

#define VERSION         "2.0.0"
#define DEFAULT_PORT    502
#define DEFAULT_DEVICE  "/dev/ttyUSB0"
#define DEFAULT_BAUD    9600
#define MAX_CONNECTIONS 10
#define MAX_SLAVES      32
#define REG_COUNT       1000

/* 配置结构 */
struct gateway_config {
    char rtu_device[256];
    int rtu_baud;
    char rtu_parity;
    int rtu_data_bits;
    int rtu_stop_bits;
    int tcp_port;
    int tcp_timeout;
    int rs485_gpio;
    int debug_level;
    char mapping_file[256];
};

/* 网关状态 */
struct gateway_state {
    modbus_t *rtu_ctx;
    modbus_t *tcp_ctx;
    struct gpiod_chip *gpio_chip;
    struct gpiod_line *rs485_line;

    pthread_mutex_t rtu_lock;
    pthread_mutex_t data_lock;

    uint16_t holding_regs[REG_COUNT];
    uint16_t input_regs[REG_COUNT];
    uint8_t coils[REG_COUNT / 8];
    uint8_t discrete_inputs[REG_COUNT / 8];

    volatile int running;
    int client_count;
};

static struct gateway_state g_state = {0};
static struct gateway_config g_config = {
    .rtu_device = "/dev/ttyUSB0",
    .rtu_baud = 9600,
    .rtu_parity = 'N',
    .rtu_data_bits = 8,
    .rtu_stop_bits = 1,
    .tcp_port = 502,
    .tcp_timeout = 5,
    .rs485_gpio = -1,
    .debug_level = 1,
    .mapping_file = "/etc/modbus-gateway/mapping.json",
};

/* RS485方向控制 */
void rs485_set_tx(int tx_enable)
{
    if (g_state.rs485_line) {
        gpiod_line_set_value(g_state.rs485_line, tx_enable);
        usleep(tx_enable ? 1000 : 100);
    }
}

/* 信号处理 */
static void signal_handler(int sig)
{
    (void)sig;
    g_state.running = 0;
}

/* 读取配置文件 */
int load_config(const char *filename)
{
    FILE *fp;
    char buf[4096];
    struct json_object *root, *obj;

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Using default configuration\n");
        return 0;
    }

    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[n] = '\0';
    fclose(fp);

    root = json_tokener_parse(buf);
    if (!root) {
        fprintf(stderr, "Failed to parse config file\n");
        return -1;
    }

    if (json_object_object_get_ex(root, "rtu", &obj)) {
        struct json_object *val;
        if (json_object_object_get_ex(obj, "device", &val))
            strncpy(g_config.rtu_device, json_object_get_string(val), 255);
        if (json_object_object_get_ex(obj, "baud", &val))
            g_config.rtu_baud = json_object_get_int(val);
        if (json_object_object_get_ex(obj, "rs485_gpio", &val))
            g_config.rs485_gpio = json_object_get_int(val);
    }

    if (json_object_object_get_ex(root, "tcp", &obj)) {
        struct json_object *val;
        if (json_object_object_get_ex(obj, "port", &val))
            g_config.tcp_port = json_object_get_int(val);
        if (json_object_object_get_ex(obj, "timeout", &val))
            g_config.tcp_timeout = json_object_get_int(val);
    }

    json_object_put(root);
    printf("Configuration loaded from %s\n", filename);
    return 0;
}

/* RTU通信线程 */
void *rtu_thread(void *arg)
{
    (void)arg;
    uint16_t buffer[125];
    int ret;
    int slave_ids[] = {1, 2, 3, 4, 5};
    int num_slaves = sizeof(slave_ids) / sizeof(slave_ids[0]);

    printf("[RTU] Thread started\n");

    while (g_state.running) {
        for (int i = 0; i < num_slaves && g_state.running; i++) {
            int slave_id = slave_ids[i];

            pthread_mutex_lock(&g_state.rtu_lock);
            modbus_set_slave(g_state.rtu_ctx, slave_id);

            /* 读取保持寄存器 */
            rs485_set_tx(0);
            ret = modbus_read_registers(g_state.rtu_ctx, 0, 10, buffer);

            if (ret == 10) {
                pthread_mutex_lock(&g_state.data_lock);
                memcpy(&g_state.holding_regs[slave_id * 100], buffer, ret * 2);
                pthread_mutex_unlock(&g_state.data_lock);

                if (g_config.debug_level >= 2) {
                    printf("[RTU] Slave %d: Read %d registers\n", slave_id, ret);
                }
            } else {
                if (g_config.debug_level >= 1) {
                    fprintf(stderr, "[RTU] Slave %d: Read failed: %s\n",
                            slave_id, modbus_strerror(errno));
                }
            }

            pthread_mutex_unlock(&g_state.rtu_lock);

            usleep(100000);  /* 100ms between slaves */
        }
    }

    printf("[RTU] Thread stopped\n");
    return NULL;
}

/* 处理TCP客户端 */
void *tcp_client_thread(void *arg)
{
    int client_sock = *(int *)arg;
    modbus_t *ctx;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;

    free(arg);

    ctx = modbus_new_tcp("127.0.0.1", 0);
    if (!ctx) {
        close(client_sock);
        return NULL;
    }

    modbus_set_socket(ctx, client_sock);

    __sync_add_and_fetch(&g_state.client_count, 1);
    printf("[TCP] Client connected (total: %d)\n", g_state.client_count);

    while (g_state.running) {
        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            int slave_id = query[6];
            int function = query[7];
            int addr = (query[8] << 8) | query[9];
            int count = (query[10] << 8) | query[11];

            if (g_config.debug_level >= 2) {
                printf("[TCP] Request: slave=%d, func=%d, addr=%d, count=%d\n",
                       slave_id, function, addr, count);
            }

            /* 处理本地数据 */
            pthread_mutex_lock(&g_state.data_lock);

            uint8_t response[MODBUS_TCP_MAX_ADU_LENGTH];
            int resp_len;

            response[0] = query[0];  /* Transaction ID */
            response[1] = query[1];
            response[2] = 0;         /* Protocol ID */
            response[3] = 0;
            response[4] = 0;         /* Length (fill later) */
            response[5] = 0;
            response[6] = slave_id;
            response[7] = function;

            switch (function) {
            case 0x03:  /* Read Holding Registers */
                if (addr + count <= REG_COUNT) {
                    response[8] = count * 2;
                    for (int i = 0; i < count; i++) {
                        response[9 + i * 2] = g_state.holding_regs[addr + i] >> 8;
                        response[10 + i * 2] = g_state.holding_regs[addr + i] & 0xFF;
                    }
                    resp_len = 9 + count * 2;
                } else {
                    response[7] = function | 0x80;
                    response[8] = 0x02;  /* Illegal data address */
                    resp_len = 9;
                }
                break;

            case 0x04:  /* Read Input Registers */
                if (addr + count <= REG_COUNT) {
                    response[8] = count * 2;
                    for (int i = 0; i < count; i++) {
                        response[9 + i * 2] = g_state.input_regs[addr + i] >> 8;
                        response[10 + i * 2] = g_state.input_regs[addr + i] & 0xFF;
                    }
                    resp_len = 9 + count * 2;
                } else {
                    response[7] = function | 0x80;
                    response[8] = 0x02;
                    resp_len = 9;
                }
                break;

            default:
                /* 转发到RTU */
                pthread_mutex_unlock(&g_state.data_lock);

                pthread_mutex_lock(&g_state.rtu_lock);
                rs485_set_tx(0);
                rc = modbus_send_raw_request(g_state.rtu_ctx, query + 6, rc - 6);
                if (rc >= 0) {
                    uint8_t rtu_resp[MODBUS_RTU_MAX_ADU_LENGTH];
                    rc = modbus_receive_confirmation(g_state.rtu_ctx, rtu_resp);
                    if (rc >= 0) {
                        /* 转换RTU到TCP */
                        response[4] = (rc + 1) >> 8;
                        response[5] = (rc + 1) & 0xFF;
                        memcpy(response + 8, rtu_resp + 1, rc - 1);
                        resp_len = 8 + rc - 1;
                        send(client_sock, response, resp_len, 0);
                    }
                }
                pthread_mutex_unlock(&g_state.rtu_lock);

                pthread_mutex_lock(&g_state.data_lock);
                goto skip_response;
            }

            /* 设置长度 */
            int data_len = resp_len - 6;
            response[4] = data_len >> 8;
            response[5] = data_len & 0xFF;

            send(client_sock, response, resp_len, 0);

        skip_response:
            pthread_mutex_unlock(&g_state.data_lock);

        } else if (rc == -1) {
            break;
        }
    }

    modbus_close(ctx);
    modbus_free(ctx);
    close(client_sock);

    __sync_sub_and_fetch(&g_state.client_count, 1);
    printf("[TCP] Client disconnected (total: %d)\n", g_state.client_count);

    return NULL;
}

/* TCP服务器线程 */
void *tcp_server_thread(void *arg)
{
    (void)arg;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("[TCP] Server thread started\n");

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[TCP] Socket creation failed");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_config.tcp_port);

    if (bind(server_sock, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("[TCP] Bind failed");
        close(server_sock);
        return NULL;
    }

    if (listen(server_sock, MAX_CONNECTIONS) < 0) {
        perror("[TCP] Listen failed");
        close(server_sock);
        return NULL;
    }

    printf("[TCP] Listening on port %d\n", g_config.tcp_port);

    while (g_state.running) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr,
                             &client_len);
        if (client_sock < 0) {
            if (errno == EINTR) continue;
            perror("[TCP] Accept failed");
            continue;
        }

        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = client_sock;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, tcp_client_thread, sock_ptr);
        pthread_detach(client_thread);
    }

    close(server_sock);
    printf("[TCP] Server stopped\n");
    return NULL;
}

/* 初始化RTU */
int init_rtu(void)
{
    printf("[RTU] Initializing %s@%d\n", g_config.rtu_device, g_config.rtu_baud);

    g_state.rtu_ctx = modbus_new_rtu(g_config.rtu_device,
                                      g_config.rtu_baud,
                                      g_config.rtu_parity,
                                      g_config.rtu_data_bits,
                                      g_config.rtu_stop_bits);
    if (!g_state.rtu_ctx) {
        fprintf(stderr, "[RTU] Failed to create context\n");
        return -1;
    }

    modbus_set_error_recovery(g_state.rtu_ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);
    modbus_set_response_timeout(g_state.rtu_ctx, g_config.tcp_timeout, 0);

    /* 初始化RS485 GPIO */
    if (g_config.rs485_gpio >= 0) {
        g_state.gpio_chip = gpiod_chip_open_by_name("gpiochip0");
        if (g_state.gpio_chip) {
            g_state.rs485_line = gpiod_chip_get_line(g_state.gpio_chip,
                                                      g_config.rs485_gpio);
            if (g_state.rs485_line) {
                gpiod_line_request_output(g_state.rs485_line, "rs485_ctrl", 0);
                printf("[RTU] RS485 control on GPIO%d\n", g_config.rs485_gpio);
            }
        }
    }

    if (modbus_connect(g_state.rtu_ctx) == -1) {
        fprintf(stderr, "[RTU] Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(g_state.rtu_ctx);
        return -1;
    }

    printf("[RTU] Connected\n");
    return 0;
}

/* 清理资源 */
void cleanup(void)
{
    g_state.running = 0;

    if (g_state.rs485_line) {
        gpiod_line_release(g_state.rs485_line);
    }
    if (g_state.gpio_chip) {
        gpiod_chip_close(g_state.gpio_chip);
    }
    if (g_state.rtu_ctx) {
        modbus_close(g_state.rtu_ctx);
        modbus_free(g_state.rtu_ctx);
    }
}

/* 主函数 */
int main(int argc, char *argv[])
{
    pthread_t rtu_tid, tcp_tid;
    int opt;

    printf("=== Modbus Gateway v%s ===\n\n", VERSION);

    /* 解析命令行 */
    while ((opt = getopt(argc, argv, "d:b:p:c:hv")) != -1) {
        switch (opt) {
        case 'd':
            strncpy(g_config.rtu_device, optarg, 255);
            break;
        case 'b':
            g_config.rtu_baud = atoi(optarg);
            break;
        case 'p':
            g_config.tcp_port = atoi(optarg);
            break;
        case 'c':
            strncpy(g_config.mapping_file, optarg, 255);
            break;
        case 'v':
            g_config.debug_level++;
            break;
        case 'h':
        default:
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  -d device   RTU device (default: %s)\n", g_config.rtu_device);
            printf("  -b baud     RTU baud rate (default: %d)\n", g_config.rtu_baud);
            printf("  -p port     TCP port (default: %d)\n", g_config.tcp_port);
            printf("  -c config   Config file\n");
            printf("  -v          Verbose (multiple for more)\n");
            printf("  -h          Show this help\n");
            return 0;
        }
    }

    /* 加载配置 */
    load_config(g_config.mapping_file);

    /* 初始化 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    pthread_mutex_init(&g_state.rtu_lock, NULL);
    pthread_mutex_init(&g_state.data_lock, NULL);

    g_state.running = 1;

    /* 初始化RTU */
    if (init_rtu() < 0) {
        return 1;
    }

    /* 创建线程 */
    pthread_create(&rtu_tid, NULL, rtu_thread, NULL);
    pthread_create(&tcp_tid, NULL, tcp_server_thread, NULL);

    /* 主循环 */
    printf("\nGateway running. Press Ctrl+C to stop.\n\n");

    while (g_state.running) {
        sleep(1);
    }

    printf("\nShutting down...\n");

    /* 等待线程 */
    pthread_join(rtu_tid, NULL);
    pthread_join(tcp_tid, NULL);

    cleanup();

    printf("Gateway stopped.\n");
    return 0;
}
