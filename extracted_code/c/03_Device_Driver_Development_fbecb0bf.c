/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 538
 * Language: c
 * Block ID: fbecb0bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: import_module.c
 * Description: 使用其他模块导出的符号
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");  /* 必须是GPL才能使用EXPORT_SYMBOL_GPL导出的符号 */
MODULE_AUTHOR("Driver Developer");
MODULE_DESCRIPTION("Module that imports symbols");

/*
 * 声明外部符号
 * 这些符号在export_module.c中定义并导出
 */
extern int my_exported_variable;
extern int my_exported_function(int a, int b);

struct my_data_struct {
    int id;
    char name[32];
    void *private_data;
};
extern struct my_data_struct my_shared_data;

struct my_operations {
    int (*open)(void *ctx);
    int (*close)(void *ctx);
    ssize_t (*read)(void *ctx, char *buf, size_t count);
    ssize_t (*write)(void *ctx, const char *buf, size_t count);
};
extern struct my_operations default_ops;

/* 模块依赖声明 */
MODULE_SOFTDEP("pre: export_module");  /* 提示需要先加载export_module */

static int __init import_module_init(void)
{
    int result;

    printk(KERN_INFO "=== Import Module ===\n");

    /* 使用导出的变量 */
    printk(KERN_INFO "my_exported_variable = %d\n", my_exported_variable);
    my_exported_variable = 200;
    printk(KERN_INFO "my_exported_variable changed to %d\n", my_exported_variable);

    /* 使用导出的函数 */
    result = my_exported_function(10, 20);
    printk(KERN_INFO "my_exported_function(10, 20) = %d\n", result);

    /* 使用导出的数据结构 */
    printk(KERN_INFO "my_shared_data.id = %d\n", my_shared_data.id);
    printk(KERN_INFO "my_shared_data.name = %s\n", my_shared_data.name);

    /* 使用导出的操作表 */
    if (default_ops.open) {
        default_ops.open(NULL);
    }
    if (default_ops.read) {
        default_ops.read(NULL, NULL, 100);
    }
    if (default_ops.close) {
        default_ops.close(NULL);
    }

    return 0;
}

static void __exit import_module_exit(void)
{
    printk(KERN_INFO "Import module unloaded\n");
}

module_init(import_module_init);
module_exit(import_module_exit);
