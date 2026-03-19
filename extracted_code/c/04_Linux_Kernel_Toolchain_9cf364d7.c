/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 324
 * Language: c
 * Block ID: 9cf364d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内核模块基本结构 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

/* 模块信息 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Author Name <email@example.com>");
MODULE_DESCRIPTION("Module description");
MODULE_VERSION("1.0");

/* 模块参数 */
static int my_param = 0;
module_param(my_param, int, 0644);
MODULE_PARM_DESC(my_param, "Description of my_param");

static char *my_string = "default";
module_param(my_string, charp, 0000);
MODULE_PARM_DESC(my_string, "A character string");

/* 初始化函数 */
static int __init my_module_init(void)
{
    int ret = 0;

    printk(KERN_INFO "My module: initializing\n");

    /* 分配资源、注册设备、初始化数据结构 */
    ret = register_chrdev(0, "mydev", &my_fops);
    if (ret < 0) {
        printk(KERN_ERR "My module: failed to register device\n");
        return ret;
    }

    printk(KERN_INFO "My module: loaded with param=%d\n", my_param);
    return 0;
}

/* 清理函数 */
static void __exit my_module_exit(void)
{
    printk(KERN_INFO "My module: exiting\n");

    /* 释放资源、注销设备 */
    unregister_chrdev(0, "mydev");
}

/* 注册入口 */
module_init(my_module_init);
module_exit(my_module_exit);
