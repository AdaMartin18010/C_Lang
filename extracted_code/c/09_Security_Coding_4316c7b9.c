/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 364
 * Language: c
 * Block ID: 4316c7b9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// security_config.h
#ifndef SECURITY_CONFIG_H
#define SECURITY_CONFIG_H

// 禁用危险函数
#define strcpy(...)  _error_use_strlcpy_instead_of_strcpy
#define strcat(...)  _error_use_strlcat_instead_of_strcat
#define sprintf(...) _error_use_snprintf_instead_of_sprintf
#define gets(...)    _error_never_use_gets

// 安全检查宏
#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "Null pointer at %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

#define CHECK_BOUNDS(index, size) do { \
    if ((index) >= (size)) { \
        fprintf(stderr, "Bounds check failed at %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

#endif
