/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 3428
 * Language: c
 * Block ID: 40966cf1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: timer_driver.c
 * Description: Linux内核定时器驱动完整实现
 * 包含低分辨率定时器、高分辨率定时器、动态定时器
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>         /* 传统定时器 */
#include <linux/hrtimer.h>       /* 高分辨率定时器 */
#include <linux/ktime.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/timekeeping.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Timer Driver Developer");
MODULE_DESCRIPTION("Complete Linux Timer Driver Example");

#define DRIVER_NAME     "timermodule"
#define CLASS_NAME      "timermodule"
#define MAX_TIMERS      16

/* 定时器类型 */
enum timer_type {
    TIMER_LEGACY,       /* 传统定时器 */
    TIMER_HRTIMER,      /* 高分辨率定时器 */
    TIMER_HRTIMER_REL,  /* 高分辨率相对定时器 */
};

/* 定时器状态 */
struct timer_entry {
    enum timer_type type;
    bool active;
    u64 period_ns;          /* 周期 (纳秒) */
    u64 count;              /* 触发计数 */
    ktime_t start_time;
    ktime_t last_fire;

    union {
        struct timer_list legacy;
        struct hrtimer hr;
    } timer;
};

/* 设备私有数据 */
struct timermodule_dev {
    struct timer_entry timers[MAX_TIMERS];
    spinlock_t lock;

    /* 工作队列用于处理定时器回调中的耗时操作 */
    struct work_struct timeout_work;
    struct workqueue_struct *workqueue;
    u32 pending_timer_id;

    /* 统计 */
    u64 total_timeouts;
    ktime_t last_stats_reset;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/* 全局设备指针 (用于workqueue访问) */
static struct timermodule_dev *g_dev;

/*
 * 传统定时器回调
 * 在软中断上下文执行，不能睡眠
 */
static void legacy_timer_callback(struct timer_list *t)
{
    struct timer_entry *entry = container_of(t, struct timer_entry, timer.legacy);
    unsigned long flags;

    spin_lock_irqsave(&g_dev->lock, flags);

    entry->count++;
    entry->last_fire = ktime_get();
    g_dev->total_timeouts++;

    /* 如果是周期性定时器，重新设置 */
    if (entry->active && entry->period_ns > 0) {
        /* jiffies精度有限，转换为ticks */
        unsigned long expires = jiffies + nsecs_to_jiffies(entry->period_ns);
        mod_timer(&entry->timer.legacy, expires);
    } else {
        entry->active = false;
    }

    spin_unlock_irqrestore(&g_dev->lock, flags);

    /*
     * 如果需要在定时器回调中执行耗时操作，
     * 应该使用workqueue
     */
    g_dev->pending_timer_id = entry - g_dev->timers;
    queue_work(g_dev->workqueue, &g_dev->timeout_work);
}

/*
 * 高分辨率定时器回调
 * 返回类型决定定时器行为:
 * - HRTIMER_NORESTART: 不重启
 * - HRTIMER_RESTART: 自动重启 (需要设置next_time)
 */
static enum hrtimer_restart hrtimer_callback(struct hrtimer *timer)
{
    struct timer_entry *entry = container_of(timer, struct timer_entry, timer.hr);
    unsigned long flags;

    spin_lock_irqsave(&g_dev->lock, flags);

    entry->count++;
    entry->last_fire = ktime_get();
    g_dev->total_timeouts++;

    if (entry->active && entry->period_ns > 0) {
        /* 绝对时间模式: 设置下一个触发时间点 */
        hrtimer_forward_now(timer, ns_to_ktime(entry->period_ns));
        spin_unlock_irqrestore(&g_dev->lock, flags);
        return HRTIMER_RESTART;
    }

    entry->active = false;
    spin_unlock_irqrestore(&g_dev->lock, flags);

    /* 调度工作队列处理 */
    g_dev->pending_timer_id = entry - g_dev->timers;
    queue_work(g_dev->workqueue, &g_dev->timeout_work);

    return HRTIMER_NORESTART;
}

/*
 * 工作队列处理函数
 * 在进程上下文执行，可以睡眠
 */
static void timeout_work_handler(struct work_struct *work)
{
    struct timermodule_dev *dev = container_of(work, struct timermodule_dev, timeout_work);
    u32 id = dev->pending_timer_id;
    struct timer_entry *entry;

    if (id >= MAX_TIMERS)
        return;

    entry = &dev->timers[id];

    /* 在这里执行需要睡眠的操作 */
    pr_debug("Timer %u fired (type=%u, count=%llu)\n", id, entry->type, entry->count);
}

/*
 * 启动定时器
 */
static int timer_start(struct timermodule_dev *dev, u32 id,
                        enum timer_type type, u64 period_ns, bool periodic)
{
    struct timer_entry *entry;
    unsigned long flags;

    if (id >= MAX_TIMERS)
        return -EINVAL;

    entry = &dev->timers[id];

    spin_lock_irqsave(&dev->lock, flags);

    /* 停止已存在的定时器 */
    if (entry->active) {
        switch (entry->type) {
        case TIMER_LEGACY:
            del_timer_sync(&entry->timer.legacy);
            break;
        case TIMER_HRTIMER:
        case TIMER_HRTIMER_REL:
            hrtimer_cancel(&entry->timer.hr);
            break;
        }
        entry->active = false;
    }

    /* 初始化新定时器 */
    entry->type = type;
    entry->period_ns = periodic ? period_ns : 0;
    entry->count = 0;
    entry->start_time = ktime_get();
    entry->last_fire = ktime_set(0, 0);
    entry->active = true;

    switch (type) {
    case TIMER_LEGACY:
        timer_setup(&entry->timer.legacy, legacy_timer_callback, 0);
        entry->timer.legacy.expires = jiffies + nsecs_to_jiffies(period_ns);
        add_timer(&entry->timer.legacy);
        break;

    case TIMER_HRTIMER:
        hrtimer_init(&entry->timer.hr, CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
        entry->timer.hr.function = hrtimer_callback;
        hrtimer_start(&entry->timer.hr, ktime_add_ns(ktime_get(), period_ns),
                      HRTIMER_MODE_ABS);
        break;

    case TIMER_HRTIMER_REL:
        hrtimer_init(&entry->timer.hr, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        entry->timer.hr.function = hrtimer_callback;
        hrtimer_start(&entry->timer.hr, ns_to_ktime(period_ns),
                      HRTIMER_MODE_REL);
        break;
    }

    spin_unlock_irqrestore(&dev->lock, flags);

    pr_info("Timer %u started: type=%u, period=%llu ns, periodic=%d\n",
            id, type, period_ns, periodic);

    return 0;
}

/*
 * 停止定时器
 */
static int timer_stop(struct timermodule_dev *dev, u32 id)
{
    struct timer_entry *entry;
    unsigned long flags;

    if (id >= MAX_TIMERS)
        return -EINVAL;

    entry = &dev->timers[id];

    spin_lock_irqsave(&dev->lock, flags);

    if (!entry->active) {
        spin_unlock_irqrestore(&dev->lock, flags);
        return -EINVAL;
    }

    entry->active = false;

    switch (entry->type) {
    case TIMER_LEGACY:
        del_timer_sync(&entry->timer.legacy);
        break;
    case TIMER_HRTIMER:
    case TIMER_HRTIMER_REL:
        hrtimer_cancel(&entry->timer.hr);
        break;
    }

    spin_unlock_irqrestore(&dev->lock, flags);

    pr_info("Timer %u stopped (total fires: %llu)\n", id, entry->count);

    return 0;
}

/*
 * 获取定时器状态
 */
static void timer_get_status(struct timermodule_dev *dev, u32 id, char *buf, size_t size)
{
    struct timer_entry *entry;
    unsigned long flags;
    ssize_t count = 0;
    s64 elapsed_ns;

    if (id >= MAX_TIMERS) {
        snprintf(buf, size, "Invalid timer ID\n");
        return;
    }

    entry = &dev->timers[id];

    spin_lock_irqsave(&dev->lock, flags);

    elapsed_ns = ktime_to_ns(ktime_sub(ktime_get(), entry->start_time));

    count += snprintf(buf + count, size - count, "Timer %u:\n", id);
    count += snprintf(buf + count, size - count, "  Type: %s\n",
                      entry->type == TIMER_LEGACY ? "Legacy" :
                      entry->type == TIMER_HRTIMER ? "HRTimer (Abs)" :
                      entry->type == TIMER_HRTIMER_REL ? "HRTimer (Rel)" : "Unknown");
    count += snprintf(buf + count, size - count, "  Active: %s\n",
                      entry->active ? "Yes" : "No");
    count += snprintf(buf + count, size - count, "  Period: %llu ns\n",
                      entry->period_ns);
    count += snprintf(buf + count, size - count, "  Count: %llu\n",
                      entry->count);
    count += snprintf(buf + count, size - count, "  Elapsed: %lld ms\n",
                      elapsed_ns / NSEC_PER_MSEC);

    if (entry->count > 0) {
        count += snprintf(buf + count, size - count, "  Frequency: %lld Hz\n",
                          entry->count * NSEC_PER_SEC / elapsed_ns);
    }

    spin_unlock_irqrestore(&dev->lock, flags);
}

/* ioctl 命令 */
#define TIMER_IOC_MAGIC     'T'
#define TIMER_START         _IOW(TIMER_IOC_MAGIC, 0, struct timer_cmd)
#define TIMER_STOP          _IOW(TIMER_IOC_MAGIC, 1, __u32)
#define TIMER_GET_STATUS    _IOWR(TIMER_IOC_MAGIC, 2, struct timer_status)
#define TIMER_RESET_STATS   _IO(TIMER_IOC_MAGIC, 3)

struct timer_cmd {
    __u32 id;
    __u32 type;
    __u64 period_ns;
    __u32 periodic;
};

struct timer_status {
    __u32 id;
    __u32 active;
    __u64 period_ns;
    __u64 count;
    __u64 elapsed_ns;
};

static int timer_open(struct inode *inode, struct file *filp)
{
    struct timermodule_dev *dev = container_of(inode->i_cdev, struct timermodule_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static int timer_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static long timer_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct timermodule_dev *dev = filp->private_data;
    struct timer_cmd tcmd;
    struct timer_status tstat;
    int ret = 0;

    switch (cmd) {
    case TIMER_START:
        if (copy_from_user(&tcmd, (void __user *)arg, sizeof(tcmd)))
            return -EFAULT;
        ret = timer_start(dev, tcmd.id, tcmd.type, tcmd.period_ns, tcmd.periodic);
        break;

    case TIMER_STOP:
        ret = timer_stop(dev, (__u32)arg);
        break;

    case TIMER_GET_STATUS:
        if (copy_from_user(&tstat, (void __user *)arg, sizeof(tstat)))
            return -EFAULT;
        if (tstat.id >= MAX_TIMERS)
            return -EINVAL;
        {
            struct timer_entry *entry = &dev->timers[tstat.id];
            unsigned long flags;
            spin_lock_irqsave(&dev->lock, flags);
            tstat.active = entry->active;
            tstat.period_ns = entry->period_ns;
            tstat.count = entry->count;
            tstat.elapsed_ns = ktime_to_ns(ktime_sub(ktime_get(), entry->start_time));
            spin_unlock_irqrestore(&dev->lock, flags);
        }
        if (copy_to_user((void __user *)arg, &tstat, sizeof(tstat)))
            return -EFAULT;
        break;

    case TIMER_RESET_STATS:
        dev->total_timeouts = 0;
        dev->last_stats_reset = ktime_get();
        break;

    default:
        ret = -ENOTTY;
    }

    return ret;
}

static ssize_t timer_read(struct file *filp, char __user *user_buf,
                           size_t count, loff_t *f_pos)
{
    struct timermodule_dev *dev = filp->private_data;
    char *buf;
    ssize_t len = 0;
    int i;

    buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    len += snprintf(buf + len, PAGE_SIZE - len,
                    "=== Timer Module Statistics ===\n");
    len += snprintf(buf + len, PAGE_SIZE - len,
                    "Total timeouts: %llu\n", dev->total_timeouts);
    len += snprintf(buf + len, PAGE_SIZE - len,
                    "\n=== Active Timers ===\n");

    for (i = 0; i < MAX_TIMERS; i++) {
        if (dev->timers[i].active || dev->timers[i].count > 0) {
            char status[256];
            timer_get_status(dev, i, status, sizeof(status));
            len += snprintf(buf + len, PAGE_SIZE - len, "%s\n", status);
        }
    }

    if (*f_pos >= len) {
        kfree(buf);
        return 0;
    }

    count = min(count, (size_t)(len - *f_pos));

    if (copy_to_user(user_buf, buf + *f_pos, count)) {
        kfree(buf);
        return -EFAULT;
    }

    *f_pos += count;
    kfree(buf);

    return count;
}

static const struct file_operations timer_fops = {
    .owner          = THIS_MODULE,
    .open           = timer_open,
    .release        = timer_release,
    .unlocked_ioctl = timer_ioctl,
    .read           = timer_read,
};

static int __init timermodule_init(void)
{
    int ret;
    int i;

    pr_info("Timer Module initializing\n");

    g_dev = kzalloc(sizeof(*g_dev), GFP_KERNEL);
    if (!g_dev)
        return -ENOMEM;

    spin_lock_init(&g_dev->lock);
    g_dev->last_stats_reset = ktime_get();

    /* 初始化定时器表 */
    for (i = 0; i < MAX_TIMERS; i++) {
        g_dev->timers[i].active = false;
    }

    /* 创建工作队列 */
    INIT_WORK(&g_dev->timeout_work, timeout_work_handler);
    g_dev->workqueue = alloc_workqueue("timermodule_wq", WQ_UNBOUND, 1);
    if (!g_dev->workqueue) {
        ret = -ENOMEM;
        goto free_dev;
    }

    /* 分配设备号 */
    ret = alloc_chrdev_region(&g_dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret)
        goto destroy_wq;

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
    cdev_init(&g_dev->cdev, &timer_fops);
    g_dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&g_dev->cdev, g_dev->dev_num, 1);
    if (ret)
        goto destroy_device;

    pr_info("Timer Module loaded. Device: /dev/%s\n", DRIVER_NAME);

    return 0;

destroy_device:
    device_destroy(g_dev->class, g_dev->dev_num);
destroy_class:
    class_destroy(g_dev->class);
unregister_chrdev:
    unregister_chrdev_region(g_dev->dev_num, 1);
destroy_wq:
    destroy_workqueue(g_dev->workqueue);
free_dev:
    kfree(g_dev);
    return ret;
}

static void __exit timermodule_exit(void)
{
    int i;

    /* 停止所有定时器 */
    for (i = 0; i < MAX_TIMERS; i++) {
        if (g_dev->timers[i].active)
            timer_stop(g_dev, i);
    }

    /* 刷新并销毁workqueue */
    flush_workqueue(g_dev->workqueue);
    destroy_workqueue(g_dev->workqueue);

    /* 删除字符设备 */
    cdev_del(&g_dev->cdev);
    device_destroy(g_dev->class, g_dev->dev_num);
    class_destroy(g_dev->class);
    unregister_chrdev_region(g_dev->dev_num, 1);

    kfree(g_dev);

    pr_info("Timer Module unloaded\n");
}

module_init(timermodule_init);
module_exit(timermodule_exit);
