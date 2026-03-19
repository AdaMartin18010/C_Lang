/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 315
 * Language: c
 * Block ID: 468ae473
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: module_params.c
 * Description: 演示模块参数传递
 * Usage: sudo insmod module_params.ko my_int=100 my_string="hello" my_array=1,2,3,4,5
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>  /* 参数处理头文件 */
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Driver Developer");
MODULE_DESCRIPTION("Module with parameters example");

/*
 * 基本类型参数定义
 * module_param(name, type, perm)
 * name: 变量名
 * type: 类型 (byte, short, ushort, int, uint, long, ulong, charp, bool)
 * perm: sysfs权限 (0表示不导出到sysfs, 0644表示所有者可读写，其他只读)
 */
static int my_int = 42;
module_param(my_int, int, 0644);
MODULE_PARM_DESC(my_int, "An integer parameter (default: 42)");

static char *my_string = "default";
module_param(my_string, charp, 0644);
MODULE_PARM_DESC(my_string, "A string parameter");

static bool my_bool = true;
module_param(my_bool, bool, 0644);
MODULE_PARM_DESC(my_bool, "A boolean parameter (true/false, y/n, 1/0)");

static ulong my_ulong = 0x12345678;
module_param(my_ulong, ulong, 0644);
MODULE_PARM_DESC(my_ulong, "An unsigned long parameter");

/*
 * 数组参数定义
 * module_param_array(name, type, nump, perm)
 * nump: 指向存储实际元素数量的变量指针
 */
static int my_array[10];
static int my_array_count;
module_param_array(my_array, int, &my_array_count, 0644);
MODULE_PARM_DESC(my_array, "An array of integers (max 10 elements)");

/*
 * 字符串数组参数
 */
static char *my_strings[5];
static int my_strings_count;
module_param_array(my_strings, charp, &my_strings_count, 0644);
MODULE_PARM_DESC(my_strings, "An array of strings (max 5 elements)");

/*
 * 带回调的参数
 * 当参数值改变时执行特定操作
 */
static int my_callback_int = 10;

static int my_callback_int_set(const char *val, const struct kernel_param *kp)
{
    int ret;
    int old_val = my_callback_int;

    /* 使用标准参数处理函数 */
    ret = param_set_int(val, kp);
    if (ret == 0) {
        printk(KERN_INFO "Parameter changed: %d -> %d\n", old_val, my_callback_int);

        /* 这里可以添加参数改变后的处理逻辑 */
        if (my_callback_int < 0 || my_callback_int > 100) {
            printk(KERN_WARNING "Warning: parameter out of recommended range (0-100)\n");
        }
    }
    return ret;
}

static const struct kernel_param_ops my_callback_int_ops = {
    .set = my_callback_int_set,
    .get = param_get_int,
};

module_param_cb(my_callback_int, &my_callback_int_ops, &my_callback_int, 0644);
MODULE_PARM_DESC(my_callback_int, "Integer with callback validation");

static int __init param_module_init(void)
{
    int i;

    printk(KERN_INFO "=== Module Parameters Example ===\n");
    printk(KERN_INFO "my_int = %d\n", my_int);
    printk(KERN_INFO "my_string = %s\n", my_string);
    printk(KERN_INFO "my_bool = %s\n", my_bool ? "true" : "false");
    printk(KERN_INFO "my_ulong = 0x%lx\n", my_ulong);
    printk(KERN_INFO "my_callback_int = %d\n", my_callback_int);

    printk(KERN_INFO "my_array has %d elements:\n", my_array_count);
    for (i = 0; i < my_array_count; i++) {
        printk(KERN_INFO "  my_array[%d] = %d\n", i, my_array[i]);
    }

    printk(KERN_INFO "my_strings has %d elements:\n", my_strings_count);
    for (i = 0; i < my_strings_count; i++) {
        printk(KERN_INFO "  my_strings[%d] = %s\n", i, my_strings[i]);
    }

    return 0;
}

static void __exit param_module_exit(void)
{
    printk(KERN_INFO "Module parameters example unloaded\n");
}

module_init(param_module_init);
module_exit(param_module_exit);
