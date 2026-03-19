/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 6286
 * Language: c
 * Block ID: 7634a8d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: debug_techniques.c
 * Description: Linux内核调试技术示例
 * 包含printk、动态调试、tracepoints、ftrace集成等
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/stacktrace.h>

/* 动态调试支持 */
#define DEBUG
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Debug Techniques Developer");
MODULE_DESCRIPTION("Linux Kernel Debugging Techniques");

#define DRIVER_NAME     "debugdemo"
#define CLASS_NAME      "debugdemo"

/*
 * 1. printk 日志级别
 * KERN_EMERG   - 系统崩溃
 * KERN_ALERT   - 立即采取行动
 * KERN_CRIT    - 临界条件
 * KERN_ERR     - 错误条件
 * KERN_WARNING - 警告
 * KERN_NOTICE  - 正常但重要
 * KERN_INFO    - 信息
 * KERN_DEBUG   - 调试信息
 */

/* 动态printk示例 - 使用pr_fmt */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/* 设备私有数据 */
struct debugdemo_dev {
    /* 统计数据 */
    u64 printk_count;
    u64 debug_count;
    u64 error_count;

    /* 调试信息 */
    char last_message[256];
    ktime_t last_message_time;

    /* debugfs条目 */
    struct dentry *debugfs_dir;
    struct dentry *stats_file;
    struct dentry *control_file;

    /* procfs模拟 */
    struct proc_dir_entry *proc_entry;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/* 全局设备实例 */
static struct debugdemo_dev *g_dev;

/*
 * 2. 条件打印和速率限制打印
 */
static void demo_printk(void)
{
    /* 基本printk */
    pr_info("This is an info message\n");

    /* 带变量打印 */
    int value = 42;
    pr_info("The value is: %d (0x%x)\n", value, value);

    /* 速率限制打印 - 防止日志风暴 */
    printk_ratelimited(KERN_WARNING "This warning is rate limited\n");

    /* 条件打印 */
    if (value > 0)
        pr_debug("Debug: value is positive\n");

    /* 一次性打印 */
    printk_once(KERN_INFO "This message prints only once\n");

    /* 带函数名和行号 */
    pr_info("Called from %s:%d\n", __func__, __LINE__);

    /* 内存转储 */
    u8 data[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    print_hex_dump(KERN_DEBUG, "data: ", DUMP_PREFIX_OFFSET, 16, 1,
                   data, sizeof(data), true);
}

/*
 * 3. 动态调试 (dyndbg)
 * 使用方式:
 * echo 'file debug_techniques.c +p' > /sys/kernel/debug/dynamic_debug/control
 * echo 'file debug_techniques.c -p' > /sys/kernel/debug/dynamic_debug/control
 */
static void demo_dynamic_debug(void)
{
    /* 这些消息可以通过dyndbg动态开启/关闭 */
    pr_debug("Dynamic debug message 1\n");
    pr_debug("Dynamic debug message 2: data=%d\n", 100);

    /* 带条件的动态调试 */
    if (g_dev)
        dev_dbg(g_dev->device, "Device debug message\n");
}

/*
 * 4. WARN_ON / BUG_ON 使用
 */
static int demo_assertions(int value)
{
    /* 警告但不停止 */
    WARN_ON(value < 0);

    /* 警告并打印信息 */
    WARN(value > 100, "Value %d is unusually large\n", value);

    /* 仅一次警告 */
    WARN_ON_ONCE(value == 0);

    /* 严重错误，打印栈并继续 */
    /* dump_stack(); */

    /* BUG_ON - 内核oops (仅用于不可恢复错误) */
    /* BUG_ON(value < -100); */

    return 0;
}

/*
 * 5. debugfs 接口
 */
static void *debugdemo_seq_start(struct seq_file *s, loff_t *pos)
{
    if (*pos >= 1)
        return NULL;
    return g_dev;
}

static void *debugdemo_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
    (*pos)++;
    return NULL;
}

static void debugdemo_seq_stop(struct seq_file *s, void *v)
{
}

static int debugdemo_seq_show(struct seq_file *s, void *v)
{
    struct debugdemo_dev *dev = v;

    seq_printf(s, "=== Debug Statistics ===\n");
    seq_printf(s, "printk_count: %llu\n", dev->printk_count);
    seq_printf(s, "debug_count: %llu\n", dev->debug_count);
    seq_printf(s, "error_count: %llu\n", dev->error_count);
    seq_printf(s, "\nLast message: %s\n", dev->last_message);

    return 0;
}

static const struct seq_operations debugdemo_seq_ops = {
    .start = debugdemo_seq_start,
    .next  = debugdemo_seq_next,
    .stop  = debugdemo_seq_stop,
    .show  = debugdemo_seq_show,
};

static int debugdemo_stats_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &debugdemo_seq_ops);
}

static const struct file_operations debugdemo_stats_fops = {
    .owner   = THIS_MODULE,
    .open    = debugdemo_stats_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = seq_release,
};

/* debugfs 写入接口 */
static ssize_t debugdemo_control_write(struct file *file, const char __user *user_buf,
                                        size_t count, loff_t *ppos)
{
    char buf[32];
    int cmd;

    if (count >= sizeof(buf))
        count = sizeof(buf) - 1;

    if (copy_from_user(buf, user_buf, count))
        return -EFAULT;

    buf[count] = '\0';

    if (kstrtoint(buf, 0, &cmd) == 0) {
        switch (cmd) {
        case 1:
            pr_info("Debug command 1: Trigger debug dump\n");
            demo_printk();
            break;
        case 2:
            pr_info("Debug command 2: Clear counters\n");
            if (g_dev) {
                g_dev->printk_count = 0;
                g_dev->debug_count = 0;
                g_dev->error_count = 0;
            }
            break;
        default:
            pr_info("Unknown debug command: %d\n", cmd);
        }
    }

    return count;
}

static const struct file_operations debugdemo_control_fops = {
    .owner   = THIS_MODULE,
    .write   = debugdemo_control_write,
};

/*
 * 6. 创建debugfs条目
 */
static int create_debugfs_entries(struct debugdemo_dev *dev)
{
    /* 创建目录 */
    dev->debugfs_dir = debugfs_create_dir(DRIVER_NAME, NULL);
    if (!dev->debugfs_dir)
        return -ENOMEM;

    /* 创建统计文件 */
    dev->stats_file = debugfs_create_file("stats", 0444, dev->debugfs_dir,
                                          dev, &debugdemo_stats_fops);

    /* 创建控制文件 */
    dev->control_file = debugfs_create_file("control", 0222, dev->debugfs_dir,
                                            dev, &debugdemo_control_fops);

    /* 创建原子变量文件 */
    debugfs_create_atomic_t("printk_count", 0444, dev->debugfs_dir,
                            (atomic_t *)&dev->printk_count);

    /* 创建64位变量文件 */
    debugfs_create_u64("debug_count", 0444, dev->debugfs_dir, &dev->debug_count);

    /* 创建布尔值文件 */
    debugfs_create_bool("debug_enabled", 0644, dev->debugfs_dir,
                        (u32 *)&dev->debug_count);

    pr_info("Debugfs entries created at /sys/kernel/debug/%s/\n", DRIVER_NAME);

    return 0;
}

/*
 * 7. 追踪点 (Tracepoints)
 * 需要在头文件中定义：
 * #include <linux/tracepoint.h>
 * DECLARE_TRACE(my_tracepoint, TP_PROTO(int value), TP_ARGS(value));
 */

/*
 * 8. ftrace 钩子示例
 */
static void demo_ftrace(void)
{
    /* 启用函数追踪 */
    /* echo function > /sys/kernel/debug/tracing/current_tracer */

    /* 添加过滤器 */
    /* echo debugdemo_* > /sys/kernel/debug/tracing/set_ftrace_filter */

    /* 查看追踪结果 */
    /* cat /sys/kernel/debug/tracing/trace */

    pr_info("Ftrace: Check /sys/kernel/debug/tracing/ for output\n");
}

/* 字符设备接口 */
static int debugdemo_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int debugdemo_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t debugdemo_write(struct file *filp, const char __user *user_buf,
                                size_t count, loff_t *off)
{
    struct debugdemo_dev *dev = g_dev;

    if (count > sizeof(dev->last_message) - 1)
        count = sizeof(dev->last_message) - 1;

    if (copy_from_user(dev->last_message, user_buf, count))
        return -EFAULT;

    dev->last_message[count] = '\0';
    dev->last_message_time = ktime_get();
    dev->printk_count++;

    pr_info("Received message: %s\n", dev->last_message);

    return count;
}

/* ioctl 命令 */
#define DEBUG_IOC_MAGIC     'X'
#define DEBUG_TRIGGER_DUMP  _IO(DEBUG_IOC_MAGIC, 0)
#define DEBUG_CLEAR_STATS   _IO(DEBUG_IOC_MAGIC, 1)
#define DEBUG_TEST_WARN     _IOW(DEBUG_IOC_MAGIC, 2, __u32)
#define DEBUG_TEST_STACK    _IO(DEBUG_IOC_MAGIC, 3)

static long debugdemo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct debugdemo_dev *dev = g_dev;
    __u32 value;

    switch (cmd) {
    case DEBUG_TRIGGER_DUMP:
        pr_info("=== Debug Dump Triggered ===\n");
        demo_printk();
        demo_dynamic_debug();
        demo_ftrace();
        break;

    case DEBUG_CLEAR_STATS:
        dev->printk_count = 0;
        dev->debug_count = 0;
        dev->error_count = 0;
        pr_info("Statistics cleared\n");
        break;

    case DEBUG_TEST_WARN:
        if (copy_from_user(&value, (void __user *)arg, sizeof(value)))
            return -EFAULT;
        demo_assertions(value);
        break;

    case DEBUG_TEST_STACK:
        pr_info("Dumping stack trace:\n");
        dump_stack();
        break;

    default:
        return -ENOTTY;
    }

    return 0;
}

static const struct file_operations debugdemo_fops = {
    .owner          = THIS_MODULE,
    .open           = debugdemo_open,
    .release        = debugdemo_release,
    .write          = debugdemo_write,
    .unlocked_ioctl = debugdemo_ioctl,
};

static int __init debugdemo_init(void)
{
    int ret;

    pr_info("Debug Demo initializing\n");

    g_dev = kzalloc(sizeof(*g_dev), GFP_KERNEL);
    if (!g_dev)
        return -ENOMEM;

    /* 分配设备号 */
    ret = alloc_chrdev_region(&g_dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret)
        goto free_dev;

    /* 创建设备类 */
    g_dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(g_dev->class)) {
        ret = PTR_ERR(g_dev->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    g_dev->device = device_create(g_dev->class, NULL, g_dev->dev_num, g_dev, DRIVER_NAME);
    if (IS_ERR(g_dev->device)) {
        ret = PTR_ERR(g_dev->device);
        goto destroy_class;
    }

    /* 初始化字符设备 */
    cdev_init(&g_dev->cdev, &debugdemo_fops);
    g_dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&g_dev->cdev, g_dev->dev_num, 1);
    if (ret)
        goto destroy_device;

    /* 创建debugfs条目 */
    create_debugfs_entries(g_dev);

    pr_info("Debug Demo loaded: /dev/%s\n", DRIVER_NAME);
    pr_info("Debugfs: /sys/kernel/debug/%s/\n", DRIVER_NAME);

    return 0;

destroy_device:
    device_destroy(g_dev->class, g_dev->dev_num);
destroy_class:
    class_destroy(g_dev->class);
unregister_chrdev:
    unregister_chrdev_region(g_dev->dev_num, 1);
free_dev:
    kfree(g_dev);
    return ret;
}

static void __exit debugdemo_exit(void)
{
    /* 清理debugfs */
    debugfs_remove_recursive(g_dev->debugfs_dir);

    cdev_del(&g_dev->cdev);
    device_destroy(g_dev->class, g_dev->dev_num);
    class_destroy(g_dev->class);
    unregister_chrdev_region(g_dev->dev_num, 1);
    kfree(g_dev);

    pr_info("Debug Demo unloaded\n");
}

module_init(debugdemo_init);
module_exit(debugdemo_exit);
