/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 100
 * Language: c
 * Block ID: bf97db0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 内核版本号含义: VERSION.PATCHLEVEL.SUBLEVEL
 * 例如: 5.15.32 表示主版本5, 补丁级别15, 子级别32
 *
 * 开发环境准备:
 * 1. 安装内核头文件: sudo apt-get install linux-headers-$(uname -r)
 * 2. 安装编译工具: sudo apt-get install build-essential libncurses-dev
 * 3. 获取内核源码: git clone https://github.com/torvalds/linux.git
 */

/* 检查内核版本 */
#include <linux/version.h>

/* 版本宏定义 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
    /* 5.0+ 内核代码 */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0)
    /* 4.x 内核代码 */
#else
    #error "Kernel version too old"
#endif
