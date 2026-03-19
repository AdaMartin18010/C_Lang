/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 690
 * Language: c
 * Block ID: 2c3515d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 日志级别 */
#define KERN_EMERG   KERN_SOH "0"   /* 系统不可用 */
#define KERN_ALERT   KERN_SOH "1"   /* 立即采取行动 */
#define KERN_CRIT    KERN_SOH "2"   /* 临界条件 */
#define KERN_ERR     KERN_SOH "3"   /* 错误 */
#define KERN_WARNING KERN_SOH "4"   /* 警告 */
#define KERN_NOTICE  KERN_SOH "5"   /* 正常但重要 */
#define KERN_INFO    KERN_SOH "6"   /* 信息 */
#define KERN_DEBUG   KERN_SOH "7"   /* 调试信息 */

/* 使用 */
printk(KERN_INFO "Driver loaded\n");
printk(KERN_ERR "Failed to allocate memory: %d\n", ret);

/* 动态调试 */
pr_info("Info message\n");
pr_err("Error: %pe\n", ERR_PTR(ret));
pr_debug("Debug: value=%d\n", value);  /* 需要 DEBUG 定义 */

/* 条件打印 */
#define DEBUG
#ifdef DEBUG
#define dbg_printk(fmt, ...) printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
#define dbg_printk(fmt, ...) no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif
