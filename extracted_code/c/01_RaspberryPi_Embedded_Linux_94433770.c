/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 318
 * Language: c
 * Block ID: 94433770
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// hello_module.c - 基础内核模块
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>

#define MODULE_NAME     "rpi_hello"
#define PROC_FILENAME   "rpi_hello"

static char *user_name = "Raspberry Pi";
module_param(user_name, charp, 0644);
MODULE_PARM_DESC(user_name, "User name to greet");

static int loop_count = 3;
module_param(loop_count, int, 0644);
MODULE_PARM_DESC(loop_count, "Number of times to print greeting");

/* 模块统计信息 */
struct hello_stats {
    u64 load_time;
    u64 unload_time;
    u64 read_count;
};

static struct hello_stats stats;

/* proc文件系统操作 */
static int hello_seq_show(struct seq_file *s, void *v)
{
    u64 now = ktime_get_ns();

    seq_printf(s, "=== %s Module Statistics ===\n\n", MODULE_NAME);
    seq_printf(s, "User Name:   %s\n", user_name);
    seq_printf(s, "Loop Count:  %d\n", loop_count);
    seq_printf(s, "Load Time:   %llu ns\n", stats.load_time);
    seq_printf(s, "Uptime:      %llu ms\n", (now - stats.load_time) / 1000000);
    seq_printf(s, "Read Count:  %llu\n", stats.read_count);

    stats.read_count++;
    return 0;
}

static int hello_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, hello_seq_show, NULL);
}

static const struct proc_ops hello_proc_ops = {
    .proc_open    = hello_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static struct proc_dir_entry *hello_proc_entry;

/* 模块初始化 */
static int __init hello_init(void)
{
    printk(KERN_INFO "[%s] Module loading...\n", MODULE_NAME);

    stats.load_time = ktime_get_ns();

    printk(KERN_INFO "[%s] Hello, %s!\n", MODULE_NAME, user_name);

    for (int i = 0; i < loop_count; i++) {
        printk(KERN_INFO "[%s] Greeting %d/%d\n", MODULE_NAME, i + 1, loop_count);
    }

    hello_proc_entry = proc_create(PROC_FILENAME, 0444, NULL, &hello_proc_ops);
    if (!hello_proc_entry) {
        printk(KERN_ERR "[%s] Failed to create proc entry\n", MODULE_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "[%s] Module loaded successfully\n", MODULE_NAME);
    return 0;
}

/* 模块清理 */
static void __exit hello_exit(void)
{
    stats.unload_time = ktime_get_ns();

    if (hello_proc_entry)
        proc_remove(hello_proc_entry);

    printk(KERN_INFO "[%s] Goodbye, %s!\n", MODULE_NAME, user_name);
    printk(KERN_INFO "[%s] Module uptime: %llu ms\n",
           MODULE_NAME,
           (stats.unload_time - stats.load_time) / 1000000);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Industrial IoT Team");
MODULE_DESCRIPTION("Raspberry Pi Hello World Kernel Module");
MODULE_VERSION("1.0.0");
