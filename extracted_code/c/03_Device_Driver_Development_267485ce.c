/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 185
 * Language: c
 * Block ID: 267485ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: hello_world.c
 * Description: 最基本的内核模块示例
 * Compile: make (使用下面的Makefile)
 * Load: sudo insmod hello_world.ko
 * Remove: sudo rmmod hello_world
 * View log: dmesg | tail
 */

#include <linux/init.h>      /* 模块初始化和清理宏 */
#include <linux/module.h>    /* 模块核心头文件 */
#include <linux/kernel.h>    /* printk等内核函数 */

/*
 * 模块许可证声明
 * 必须是GPL兼容的许可证，否则某些内核功能受限
 * 可选值: "GPL", "GPL v2", "GPL and additional rights",
 *        "Dual BSD/GPL", "Dual MIT/GPL", "Proprietary"
 */
MODULE_LICENSE("GPL");

/* 模块作者信息 */
MODULE_AUTHOR("Your Name <your.email@example.com>");

/* 模块描述 */
MODULE_DESCRIPTION("A simple Hello World kernel module");

/* 模块版本 */
MODULE_VERSION("1.0");

/*
 * 模块初始化函数
 * 在insmod加载模块时执行
 * @return: 0表示成功，负数表示错误码
 */
static int __init hello_init(void)
{
    /*
     * printk - 内核打印函数
     * KERN_INFO - 日志级别，表示信息性消息
     * 其他级别: KERN_EMERG, KERN_ALERT, KERN_CRIT, KERN_ERR,
     *          KERN_WARNING, KERN_NOTICE, KERN_DEBUG
     */
    printk(KERN_INFO "Hello, World!\n");
    printk(KERN_INFO "Kernel module loaded successfully.\n");

    return 0;  /* 必须返回0表示成功 */
}

/*
 * 模块清理函数
 * 在rmmod卸载模块时执行
 */
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, World!\n");
    printk(KERN_INFO "Kernel module unloaded successfully.\n");
}

/*
 * 注册初始化函数
 * 告诉内核在加载时调用hello_init
 */
module_init(hello_init);

/*
 * 注册清理函数
 * 告诉内核在卸载时调用hello_exit
 */
module_exit(hello_exit);
