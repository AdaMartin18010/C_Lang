/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\README.md
 * Line: 103
 * Language: c
 * Block ID: f55f80db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基本内核模块模板
 * 文件: hello_module.c
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kern_levels.h>

/* 模块元数据 */
MODULE_AUTHOR("Your Name <email@example.com>");
MODULE_DESCRIPTION("A simple Linux kernel module example");
MODULE_LICENSE("GPL");  /* 必须使用GPL兼容许可证 */
MODULE_VERSION("1.0");

/* 模块参数 */
static char *name = "World";
module_param(name, charp, S_IRUGO);  /* 权限: 所有者可读 */
MODULE_PARM_DESC(name, "Name to greet");

static int count = 1;
module_param(count, int, S_IRUGO | S_IWUSR);  /* 读写权限 */
MODULE_PARM_DESC(count, "Number of times to greet");

/* 模块加载函数 */
static int __init hello_init(void)
{
    int i;

    printk(KERN_INFO "Hello Module: Initializing\n");

    /* 使用不同日志级别 */
    printk(KERN_DEBUG "Debug: Loading module with name=%s, count=%d\n",
           name, count);

    for (i = 0; i < count; i++) {
        printk(KERN_INFO "Hello, %s! (%d/%d)\n", name, i + 1, count);
    }

    /* 返回0表示成功，负值表示错误码 */
    return 0;
}

/* 模块卸载函数 */
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Hello Module: Exiting\n");
    printk(KERN_INFO "Goodbye, %s!\n", name);
}

/* 注册入口/出口函数 */
module_init(hello_init);
module_exit(hello_exit);
