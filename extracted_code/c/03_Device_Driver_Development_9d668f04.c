/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 439
 * Language: c
 * Block ID: 9d668f04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: export_module.c
 * Description: 导出版本符号供其他模块使用
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Driver Developer");
MODULE_DESCRIPTION("Module that exports symbols");

/*
 * 导出的整数变量
 * EXPORT_SYMBOL: 导出给所有模块使用
 * EXPORT_SYMBOL_GPL: 仅导出给GPL许可证模块使用
 */
int my_exported_variable = 100;
EXPORT_SYMBOL_GPL(my_exported_variable);

/* 导出的函数 */
int my_exported_function(int a, int b)
{
    printk(KERN_INFO "my_exported_function called with %d, %d\n", a, b);
    return a + b;
}
EXPORT_SYMBOL_GPL(my_exported_function);

/* 导出全局数据结构 */
struct my_data_struct {
    int id;
    char name[32];
    void *private_data;
};

struct my_data_struct my_shared_data = {
    .id = 1,
    .name = "shared_data",
};
EXPORT_SYMBOL_GPL(my_shared_data);

/* 导出接口操作表 */
struct my_operations {
    int (*open)(void *ctx);
    int (*close)(void *ctx);
    ssize_t (*read)(void *ctx, char *buf, size_t count);
    ssize_t (*write)(void *ctx, const char *buf, size_t count);
};

static int my_open(void *ctx)
{
    printk(KERN_INFO "my_open called\n");
    return 0;
}

static int my_close(void *ctx)
{
    printk(KERN_INFO "my_close called\n");
    return 0;
}

static ssize_t my_read(void *ctx, char *buf, size_t count)
{
    printk(KERN_INFO "my_read called, count=%zu\n", count);
    return count;
}

static ssize_t my_write(void *ctx, const char *buf, size_t count)
{
    printk(KERN_INFO "my_write called, count=%zu\n", count);
    return count;
}

struct my_operations default_ops = {
    .open = my_open,
    .close = my_close,
    .read = my_read,
    .write = my_write,
};
EXPORT_SYMBOL_GPL(default_ops);

static int __init export_module_init(void)
{
    printk(KERN_INFO "Export module loaded\n");
    printk(KERN_INFO "Exported symbols available for other modules\n");
    return 0;
}

static void __exit export_module_exit(void)
{
    printk(KERN_INFO "Export module unloaded\n");
}

module_init(export_module_init);
module_exit(export_module_exit);
