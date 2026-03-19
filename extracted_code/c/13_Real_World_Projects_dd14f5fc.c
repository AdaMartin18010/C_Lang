/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 577
 * Language: c
 * Block ID: dd14f5fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * HTTP Web Server - Main Entry Point
 *
 * A high-performance, lightweight HTTP/1.1 web server
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include "server.h"
#include "logger.h"

static volatile sig_atomic_t g_running = 1;
static server_t *g_server = NULL;
static logger_t *g_logger = NULL;

static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        g_running = 0;
        if (g_server) {
            server_stop(g_server);
        }
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
}

static void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  -c, --config FILE    Configuration file path\n");
    printf("  -p, --port PORT      Server port (default: 8080)\n");
    printf("  -r, --root PATH      Document root directory\n");
    printf("  -d, --daemon         Run as daemon\n");
    printf("  -l, --log FILE       Log file path\n");
    printf("  -w, --workers NUM    Number of worker threads\n");
    printf("  -v, --version        Show version\n");
    printf("  -h, --help           Show this help\n");
}

static void print_version(void) {
    printf("HTTP Server Version %s\n", SERVER_VERSION);
    printf("Built with epoll support for Linux\n");
}

int main(int argc, char *argv[]) {
    server_config_t config;
    server_default_config(&config);

    const char *config_file = NULL;

    static struct option long_options[] = {
        {"config",  required_argument, 0, 'c'},
        {"port",    required_argument, 0, 'p'},
        {"root",    required_argument, 0, 'r'},
        {"daemon",  no_argument,       0, 'd'},
        {"log",     required_argument, 0, 'l'},
        {"workers", required_argument, 0, 'w'},
        {"version", no_argument,       0, 'v'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:p:r:dl:w:vh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                break;
            case 'p':
                config.port = atoi(optarg);
                break;
            case 'r':
                strncpy(config.document_root, optarg, sizeof(config.document_root) - 1);
                break;
            case 'd':
                config.daemon_mode = true;
                break;
            case 'l':
                strncpy(config.log_file, optarg, sizeof(config.log_file) - 1);
                break;
            case 'w':
                config.worker_threads = atoi(optarg);
                break;
            case 'v':
                print_version();
                return 0;
            case 'h':
            default:
                print_usage(argv[0]);
                return (opt == 'h') ? 0 : 1;
        }
    }

    /* Load configuration file if specified */
    if (config_file) {
        if (server_load_config(&config, config_file) != 0) {
            fprintf(stderr, "Failed to load config file: %s\n", config_file);
            return 1;
        }
    }

    /* Setup logging */
    g_logger = logger_create("httpserver", LOG_LEVEL_INFO, config.log_file);
    if (!g_logger) {
        fprintf(stderr, "Failed to create logger\n");
        return 1;
    }

    LOG_INFO(g_logger, "HTTP Server v%s starting...", SERVER_VERSION);
    LOG_INFO(g_logger, "Document root: %s", config.document_root);
    LOG_INFO(g_logger, "Port: %d", config.port);

    /* Daemonize if requested */
    if (config.daemon_mode) {
        if (daemon(0, 0) < 0) {
            LOG_ERROR(g_logger, "Failed to daemonize: %s", strerror(errno));
            return 1;
        }
        LOG_INFO(g_logger, "Running as daemon");
    }

    /* Create and start server */
    g_server = server_create(&config);
    if (!g_server) {
        LOG_FATAL(g_logger, "Failed to create server");
        return 1;
    }

    setup_signals();

    LOG_INFO(g_logger, "Server started successfully");

    int ret = server_start(g_server);
    if (ret != 0) {
        LOG_ERROR(g_logger, "Server error: %d", ret);
    }

    LOG_INFO(g_logger, "Server shutting down...");

    /* Cleanup */
    server_destroy(g_server);
    logger_destroy(g_logger);

    return ret;
}
