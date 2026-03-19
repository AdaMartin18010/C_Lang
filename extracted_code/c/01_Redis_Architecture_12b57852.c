/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 1069
 * Language: c
 * Block ID: 12b57852
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// fmacros.h - 功能宏定义和系统兼容性处理

#ifndef __FMACROS_H
#define __FMACROS_H

// 大文件支持
#define _FILE_OFFSET_BITS 64

// POSIX 版本
#define _POSIX_C_SOURCE 200809L

// BSD 兼容性
#define _BSD_SOURCE

// GNU 扩展
#define _GNU_SOURCE

// 平台检测
#ifdef __linux__
#define HAVE_EPOLL 1
#endif

#ifdef __APPLE__
#define HAVE_KQUEUE 1
#endif

#ifdef __sun
#define HAVE_EVPORT 1
#endif

// 编译器特性检测
#ifdef __GNUC__
#define redis_unreachable __builtin_unreachable
#else
#define redis_unreachable()
#endif

#endif
