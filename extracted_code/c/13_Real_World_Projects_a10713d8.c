/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 513
 * Language: c
 * Block ID: a10713d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} log_level_t;

typedef struct logger {
    char name[64];
    log_level_t level;
    FILE *file;
    bool use_colors;
    bool include_timestamp;
    bool include_level;
    pthread_mutex_t lock;
} logger_t;

/* Logger lifecycle */
logger_t* logger_create(const char *name, log_level_t level, const char *file_path);
void logger_destroy(logger_t *logger);
void logger_set_level(logger_t *logger, log_level_t level);

/* Logging functions */
void logger_log(logger_t *logger, log_level_t level, const char *file,
                int line, const char *fmt, ...);
void logger_vlog(logger_t *logger, log_level_t level, const char *file,
                 int line, const char *fmt, va_list args);

/* Convenience macros */
#define LOG_DEBUG(logger, ...) logger_log(logger, LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(logger, ...)  logger_log(logger, LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(logger, ...)  logger_log(logger, LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(logger, ...) logger_log(logger, LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(logger, ...) logger_log(logger, LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Access logging */
typedef struct access_log_entry {
    time_t timestamp;
    char client_ip[64];
    char method[16];
    char uri[2048];
    int status;
    size_t response_size;
    double response_time_ms;
    char user_agent[512];
    char referer[1024];
} access_log_entry_t;

void logger_access_log(logger_t *logger, const access_log_entry_t *entry);

#endif /* LOGGER_H */
