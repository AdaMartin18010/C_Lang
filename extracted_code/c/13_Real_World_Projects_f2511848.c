/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 2238
 * Language: c
 * Block ID: f2511848
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Logging System - Thread-safe logging with multiple levels
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "logger.h"

static const char *level_strings[] = {
    "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *level_colors[] = {
    "\033[36m",  /* Cyan */
    "\033[32m",  /* Green */
    "\033[33m",  /* Yellow */
    "\033[31m",  /* Red */
    "\033[35m"   /* Magenta */
};

static const char *color_reset = "\033[0m";

logger_t* logger_create(const char *name, log_level_t level, const char *file_path) {
    logger_t *logger = calloc(1, sizeof(logger_t));
    if (!logger) return NULL;

    strncpy(logger->name, name, sizeof(logger->name) - 1);
    logger->level = level;
    logger->use_colors = (file_path == NULL);
    logger->include_timestamp = true;
    logger->include_level = true;
    pthread_mutex_init(&logger->lock, NULL);

    if (file_path) {
        logger->file = fopen(file_path, "a");
        if (!logger->file) {
            free(logger);
            return NULL;
        }
        logger->use_colors = false;
    } else {
        logger->file = stdout;
    }

    return logger;
}

void logger_destroy(logger_t *logger) {
    if (!logger) return;

    pthread_mutex_destroy(&logger->lock);

    if (logger->file && logger->file != stdout) {
        fclose(logger->file);
    }

    free(logger);
}

void logger_set_level(logger_t *logger, log_level_t level) {
    if (logger) {
        logger->level = level;
    }
}

void logger_log(logger_t *logger, log_level_t level, const char *file,
                int line, const char *fmt, ...) {
    if (!logger || level < logger->level) return;

    pthread_mutex_lock(&logger->lock);

    /* Timestamp */
    if (logger->include_timestamp) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[26];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(logger->file, "[%s] ", time_str);
    }

    /* Log level */
    if (logger->include_level) {
        if (logger->use_colors && level < sizeof(level_colors)/sizeof(level_colors[0])) {
            fprintf(logger->file, "%s%s%s ",
                   level_colors[level], level_strings[level], color_reset);
        } else {
            fprintf(logger->file, "[%s] ", level_strings[level]);
        }
    }

    /* Source location for debug level */
    if (level == LOG_LEVEL_DEBUG) {
        fprintf(logger->file, "(%s:%d) ", file, line);
    }

    /* Message */
    va_list args;
    va_start(args, fmt);
    vfprintf(logger->file, fmt, args);
    va_end(args);

    fprintf(logger->file, "\n");
    fflush(logger->file);

    pthread_mutex_unlock(&logger->lock);

    /* Fatal exit */
    if (level == LOG_LEVEL_FATAL) {
        exit(1);
    }
}

void logger_vlog(logger_t *logger, log_level_t level, const char *file,
                 int line, const char *fmt, va_list args) {
    if (!logger || level < logger->level) return;

    pthread_mutex_lock(&logger->lock);

    if (logger->include_timestamp) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char time_str[26];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(logger->file, "[%s] ", time_str);
    }

    if (logger->include_level) {
        fprintf(logger->file, "[%s] ", level_strings[level]);
    }

    vfprintf(logger->file, fmt, args);
    fprintf(logger->file, "\n");
    fflush(logger->file);

    pthread_mutex_unlock(&logger->lock);
}

void logger_access_log(logger_t *logger, const access_log_entry_t *entry) {
    if (!logger || !entry) return;

    pthread_mutex_lock(&logger->lock);

    char time_str[26];
    struct tm *tm_info = localtime(&entry->timestamp);
    strftime(time_str, sizeof(time_str), "%d/%b/%Y:%H:%M:%S %z", tm_info);

    fprintf(logger->file, "%s - - [%s] \"%s %s HTTP/1.1\" %d %zu \"%s\" \"%s\" %.3fms\n",
           entry->client_ip,
           time_str,
           entry->method,
           entry->uri,
           entry->status,
           entry->response_size,
           entry->referer[0] ? entry->referer : "-",
           entry->user_agent[0] ? entry->user_agent : "-",
           entry->response_time_ms);

    fflush(logger->file);

    pthread_mutex_unlock(&logger->lock);
}
