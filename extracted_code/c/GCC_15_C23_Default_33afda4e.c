/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1570
 * Language: c
 * Block ID: 33afda4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 完整的时间处理工具库 */
#include <stdio.h>
#include <time.h>
#include <string.h>

#define TIME_STR_MAX 64

/* 时间格式化工具 */
typedef enum {
    TIME_FMT_ISO8601,      /* 2025-03-19T10:30:00Z */
    TIME_FMT_RFC2822,      /* Wed, 19 Mar 2025 10:30:00 +0000 */
    TIME_FMT_SHORT,        /* 2025-03-19 10:30:00 */
    TIME_FMT_DATE_ONLY,    /* 2025-03-19 */
    TIME_FMT_TIME_ONLY     /* 10:30:00 */
} time_format_t;

/* 线程安全的时间格式化 */
bool format_time_r(time_t t, time_format_t fmt,
                   char* buf, size_t buf_size,
                   bool use_local) {
    struct tm tm_info;
    struct tm* result = use_local ?
        localtime_r(&t, &tm_info) :
        gmtime_r(&t, &tm_info);

    if (!result) return false;

    const char* format_str;
    switch (fmt) {
        case TIME_FMT_ISO8601:
            format_str = "%Y-%m-%dT%H:%M:%SZ";
            break;
        case TIME_FMT_RFC2822:
            format_str = "%a, %d %b %Y %H:%M:%S %z";
            break;
        case TIME_FMT_SHORT:
            format_str = "%Y-%m-%d %H:%M:%S";
            break;
        case TIME_FMT_DATE_ONLY:
            format_str = "%Y-%m-%d";
            break;
        case TIME_FMT_TIME_ONLY:
            format_str = "%H:%M:%S";
            break;
        default:
            return false;
    }

    return strftime(buf, buf_size, format_str, result) > 0;
}

/* 解析 ISO8601 格式时间 */
bool parse_iso8601_r(const char* str, struct tm* result) {
    /* 格式: 2025-03-19T10:30:00Z 或 2025-03-19T10:30:00+08:00 */
    memset(result, 0, sizeof(struct tm));

    int year, mon, day, hour, min, sec;
    if (sscanf(str, "%d-%d-%dT%d:%d:%d",
               &year, &mon, &day, &hour, &min, &sec) == 6) {
        result->tm_year = year - 1900;
        result->tm_mon = mon - 1;
        result->tm_mday = day;
        result->tm_hour = hour;
        result->tm_min = min;
        result->tm_sec = sec;
        return true;
    }

    return false;
}

/* 计算时间差（秒） */
double time_diff_seconds(time_t t1, time_t t2) {
    return difftime(t1, t2);
}

/* 计算日期差（天） */
int date_diff_days(time_t t1, time_t t2) {
    struct tm tm1, tm2;

    gmtime_r(&t1, &tm1);
    gmtime_r(&t2, &tm2);

    /* 归一化到午夜 */
    tm1.tm_hour = tm1.tm_min = tm1.tm_sec = 0;
    tm2.tm_hour = tm2.tm_min = tm2.tm_sec = 0;

    time_t midnight1 = timegm(&tm1);
    time_t midnight2 = timegm(&tm2);

    return (int)(difftime(midnight1, midnight2) / 86400);
}

/* 使用示例 */
int main(void) {
    time_t now = time(NULL);
    char buf[TIME_STR_MAX];

    printf("=== C23 线程安全时间处理 ===\n\n");

    /* ISO8601 UTC */
    if (format_time_r(now, TIME_FMT_ISO8601, buf, sizeof(buf), false)) {
        printf("ISO8601 UTC: %s\n", buf);
    }

    /* ISO8601 Local */
    if (format_time_r(now, TIME_FMT_ISO8601, buf, sizeof(buf), true)) {
        /* 需要移除 Z 后缀 */
        char* z = strchr(buf, 'Z');
        if (z) *z = '\0';
        printf("ISO8601 Local: %s\n", buf);
    }

    /* RFC2822 */
    if (format_time_r(now, TIME_FMT_RFC2822, buf, sizeof(buf), true)) {
        printf("RFC2822: %s\n", buf);
    }

    /* 短格式 */
    if (format_time_r(now, TIME_FMT_SHORT, buf, sizeof(buf), true)) {
        printf("Short: %s\n", buf);
    }

    /* 解析示例 */
    struct tm parsed;
    if (parse_iso8601_r("2025-03-19T10:30:00Z", &parsed)) {
        time_t t = timegm(&parsed);
        printf("\nParsed: %s", asctime(&parsed));
        printf("Time diff: %.0f seconds\n", difftime(now, t));
    }

    return 0;
}
