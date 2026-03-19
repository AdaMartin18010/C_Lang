/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 572
 * Language: c
 * Block ID: 163127cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* sample_logger_plugin.c - 示例日志插件 */
#include "plugin_system.h"
#include <stdio.h>
#include <time.h>

static const char *log_file_path = "/tmp/plugin.log";
static FILE *log_fp = NULL;

static int logger_init(void *host_context) {
    log_fp = fopen(log_file_path, "a");
    if (!log_fp) return -1;

    fprintf(log_fp, "=== Logger plugin initialized ===\n");
    fflush(log_fp);
    return 0;
}

static int logger_deinit(void) {
    if (log_fp) {
        fprintf(log_fp, "=== Logger plugin deinitialized ===\n");
        fclose(log_fp);
        log_fp = NULL;
    }
    return 0;
}

static bool logger_has_capability(const char *cap) {
    return (strcmp(cap, "logging") == 0 ||
            strcmp(cap, "file_output") == 0);
}

static int logger_invoke(const char *method, void *args, void *result) {
    if (strcmp(method, "log") == 0 && log_fp) {
        const char *msg = (const char *)args;
        time_t now = time(NULL);
        fprintf(log_fp, "[%s] %s\n", ctime(&now), msg);
        fflush(log_fp);
        return 0;
    }
    return -1;
}

/* 插件接口定义 */
static const plugin_interface_t logger_plugin = {
    .info = {
        .magic = PLUGIN_MAGIC,
        .api_version = PLUGIN_API_VERSION,
        .type = PLUGIN_TYPE_LOGGER,
        .name = "file_logger",
        .version = "1.0.0",
        .author = "System Architect",
        .description = "File-based logging plugin"
    },
    .init = logger_init,
    .deinit = logger_deinit,
    .configure = NULL,
    .has_capability = logger_has_capability,
    .invoke = logger_invoke
};

/* 导出插件 */
DEFINE_PLUGIN(logger_plugin);
