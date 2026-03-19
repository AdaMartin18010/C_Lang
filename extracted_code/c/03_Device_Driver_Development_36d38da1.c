/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 2797
 * Language: c
 * Block ID: 36d38da1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: interrupt_handling.c
 * Description: Linux中断处理完整示例
 * 包含上半部/下半部、tasklet、workqueue、软中断
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Interrupt Driver Developer");
MODULE_DESCRIPTION("Complete Interrupt Handling Example");

#define DRIVER_NAME     "irqhandler"
#define CLASS_NAME      "irqhandler"
#define MAX_EVENTS      1000

/* 事件类型 */
enum event_type {
    EVENT_TOP_HALF,     /* 上半部 */
    EVENT_TASKLET,      /* Tasklet下半部 */
    EVENT_WORK,         /* 工作队列 */
    EVENT_THREAD,       /* 线程化中断 */
};

/* 事件记录 */
struct irq_event {
    enum event_type type;
    ktime_t timestamp;
    u32 irq_num;
    u32 data;
};

/* 设备私有数据 */
struct irqhandler_dev {
    struct platform_device *pdev;

    /* GPIO和中断 */
    struct gpio_desc *irq_gpio;
    int irq_num;

    /* 上半部统计 */
    spinlock_t top_half_lock;
    u64 top_half_count;
    ktime_t top_half_time;

    /* Tasklet */
    struct tasklet_struct my_tasklet;
    spinlock_t tasklet_lock;
    u64 tasklet_count;
    ktime_t tasklet_time;
    u32 tasklet_data;

    /* 工作队列 */
    struct work_struct my_work;
    struct workqueue_struct *workqueue;
    spinlock_t work_lock;
    u64 work_count;
    ktime_t work_time;

    /* 线程化工作 */
    struct work_struct threaded_work;
    u64 threaded_count;

    /* 事件环形缓冲区 */
    struct irq_event events[MAX_EVENTS];
    atomic_t event_head;
    atomic_t event_tail;

    /* 同步 */
    wait_queue_head_t wait_queue;
    atomic_t data_ready;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
};

/*
 * 上半部 - 快速硬件处理
 * 特点：
 * - 在中断上下文执行，禁止睡眠
 * - 尽快完成，只处理最紧急的硬件操作
 * - 注册共享中断时需要检查设备是否产生中断
 */
static irqreturn_t irqhandler_top_half(int irq, void *dev_id)
{
    struct irqhandler_dev *dev = dev_id;
    ktime_t start, end;
    int head, tail;

    start = ktime_get();

    /* 记录事件 */
    head = atomic_read(&dev->event_head);
    tail = atomic_read(&dev->event_tail);

    if ((head + 1) % MAX_EVENTS != tail) {
        dev->events[head].type = EVENT_TOP_HALF;
        dev->events[head].timestamp = start;
        dev->events[head].irq_num = irq;
        dev->events[head].data = gpiod_get_value(dev->irq_gpio);
        atomic_set(&dev->event_head, (head + 1) % MAX_EVENTS);
    }

    /* 更新统计 */
    spin_lock(&dev->top_half_lock);
    dev->top_half_count++;
    end = ktime_get();
    dev->top_half_time = ktime_sub(end, start);
    spin_unlock(&dev->top_half_lock);

    /*
     * 调度tasklet
     * tasklet_sched不会立即执行，而是在中断返回后、
     * 开启中断前执行
     */
    dev->tasklet_data = dev->top_half_count;
    tasklet_schedule(&dev->tasklet);

    /*
     * 调度工作队列
     * 工作队列在进程上下文执行，可以睡眠
     */
    queue_work(dev->workqueue, &dev->my_work);

    /* 唤醒等待的进程 */
    atomic_set(&dev->data_ready, 1);
    wake_up_interruptible(&dev->wait_queue);

    /*
     * 返回值：
     * IRQ_HANDLED - 处理了这个中断
     * IRQ_NONE - 不是我们的中断
     * IRQ_WAKE_THREAD - 唤醒线程化中断处理程序
     */
    return IRQ_HANDLED;
}

/*
 * Tasklet - 软中断上下文下半部
 * 特点：
 * - 在软中断上下文执行，不能睡眠
 * - 同一个tasklet不会并行执行（但不同tasklet可以）
 * - 适用于较短的延迟敏感操作
 * - 注意：高网络负载时可能延迟较大
 */
static void irqhandler_tasklet_handler(unsigned long data)
{
    struct irqhandler_dev *dev = (struct irqhandler_dev *)data;
    ktime_t start, end;
    int head, tail;

    start = ktime_get();

    /* 记录事件 */
    head = atomic_read(&dev->event_head);
    tail = atomic_read(&dev->event_tail);

    if ((head + 1) % MAX_EVENTS != tail) {
        dev->events[head].type = EVENT_TASKLET;
        dev->events[head].timestamp = start;
        dev->events[head].data = dev->tasklet_data;
        atomic_set(&dev->event_head, (head + 1) % MAX_EVENTS);
    }

    /*
     * 模拟一些处理工作
     * 注意：不能调用可能睡眠的函数！
     * 错误示例：kmalloc(GFP_KERNEL), mutex_lock, msleep等
     */
    udelay(10);  /* 忙等待，仅用于演示 */

    /* 更新统计 */
    spin_lock(&dev->tasklet_lock);
    dev->tasklet_count++;
    end = ktime_get();
    dev->tasklet_time = ktime_sub(end, start);
    spin_unlock(&dev->tasklet_lock);
}

/*
 * 工作队列处理函数 - 进程上下文下半部
 * 特点：
 * - 在进程上下文执行，可以睡眠
 * - 可以执行耗时操作、访问用户空间、获取信号量
 * - 延迟比tasklet大，但更灵活
 */
static void irqhandler_work_handler(struct work_struct *work)
{
    struct irqhandler_dev *dev = container_of(work, struct irqhandler_dev, my_work);
    ktime_t start, end;
    int head, tail;

    start = ktime_get();

    /* 记录事件 */
    head = atomic_read(&dev->event_head);
    tail = atomic_read(&dev->event_tail);

    if ((head + 1) % MAX_EVENTS != tail) {
        dev->events[head].type = EVENT_WORK;
        dev->events[head].timestamp = start;
        dev->events[head].data = 0;
        atomic_set(&dev->event_head, (head + 1) % MAX_EVENTS);
    }

    /*
     * 可以睡眠的操作示例：
     * - kmalloc(GFP_KERNEL)
     * - mutex_lock
     * - msleep
     * - 访问用户空间
     */

    /* 模拟耗时操作 */
    msleep(5);  /* 可以睡眠！ */

    /* 更新统计 */
    spin_lock(&dev->work_lock);
    dev->work_count++;
    end = ktime_get();
    dev->work_time = ktime_sub(end, start);
    spin_unlock(&dev->work_lock);
}

/*
 * 线程化工作处理
 * 使用专用工作队列，可以设置优先级
 */
static void irqhandler_threaded_work_handler(struct work_struct *work)
{
    struct irqhandler_dev *dev = container_of(work, struct irqhandler_dev, threaded_work);
    int head, tail;

    /* 记录事件 */
    head = atomic_read(&dev->event_head);
    tail = atomic_read(&dev->event_tail);

    if ((head + 1) % MAX_EVENTS != tail) {
        dev->events[head].type = EVENT_THREAD;
        dev->events[head].timestamp = ktime_get();
        atomic_set(&dev->event_head, (head + 1) % MAX_EVENTS);
    }

    /* 高优先级的后台处理 */
    dev->threaded_count++;
}

/*
 * 线程化中断处理 (替代tasklet/workqueue的方案)
 * 特点：
 * - 使用内核线程处理中断
 * - 可以睡眠，优先级可调
 * - 简化中断处理流程
 */
static irqreturn_t irqhandler_thread_fn(int irq, void *dev_id)
{
    struct irqhandler_dev *dev = dev_id;

    /* 在线程上下文中处理中断 */
    /* 这里可以做任何可以睡眠的操作 */

    dev_dbg(&dev->pdev->dev, "Threaded interrupt handler\n");

    return IRQ_HANDLED;
}

/*
 * 获取统计信息 (供用户空间读取)
 */
static ssize_t irqhandler_read_stats(struct irqhandler_dev *dev, char *buf, size_t size)
{
    ssize_t count = 0;
    u64 top_count, tasklet_count, work_count;
    s64 top_time_us, tasklet_time_us, work_time_us;

    /* 读取统计 */
    spin_lock_irq(&dev->top_half_lock);
    top_count = dev->top_half_count;
    top_time_us = ktime_to_us(dev->top_half_time);
    spin_unlock_irq(&dev->top_half_lock);

    spin_lock_irq(&dev->tasklet_lock);
    tasklet_count = dev->tasklet_count;
    tasklet_time_us = ktime_to_us(dev->tasklet_time);
    spin_unlock_irq(&dev->tasklet_lock);

    spin_lock_irq(&dev->work_lock);
    work_count = dev->work_count;
    work_time_us = ktime_to_us(dev->work_time);
    spin_unlock_irq(&dev->work_lock);

    count += snprintf(buf + count, size - count,
                      "=== Interrupt Statistics ===\n");
    count += snprintf(buf + count, size - count,
                      "Top Half:  count=%llu, last_time=%lld us\n",
                      top_count, top_time_us);
    count += snprintf(buf + count, size - count,
                      "Tasklet:   count=%llu, last_time=%lld us\n",
                      tasklet_count, tasklet_time_us);
    count += snprintf(buf + count, size - count,
                      "Workqueue: count=%llu, last_time=%lld us\n",
                      work_count, work_time_us);
    count += snprintf(buf + count, size - count,
                      "Threaded:  count=%llu\n", dev->threaded_count);
    count += snprintf(buf + count, size - count,
                      "Pending Events: %d\n",
                      atomic_read(&dev->event_head) - atomic_read(&dev->event_tail));

    return count;
}

/*
 * 读取事件缓冲区
 */
static ssize_t irqhandler_read_events(struct irqhandler_dev *dev, char *buf, size_t size)
{
    ssize_t count = 0;
    int head, tail;
    int i;
    int num_events;

    head = atomic_read(&dev->event_head);
    tail = atomic_read(&dev->event_tail);

    if (head >= tail)
        num_events = head - tail;
    else
        num_events = MAX_EVENTS - tail + head;

    count += snprintf(buf + count, size - count,
                      "=== Recent Events (total: %d) ===\n", num_events);

    /* 最多显示最近10个事件 */
    for (i = 0; i < min(10, num_events); i++) {
        int idx = (head - 1 - i + MAX_EVENTS) % MAX_EVENTS;
        struct irq_event *evt = &dev->events[idx];
        const char *type_str;
        s64 timestamp_ns;

        switch (evt->type) {
        case EVENT_TOP_HALF:  type_str = "TOP   "; break;
        case EVENT_TASKLET:   type_str = "TASKLT"; break;
        case EVENT_WORK:      type_str = "WORK  "; break;
        case EVENT_THREAD:    type_str = "THREAD"; break;
        default:              type_str = "UNKNOWN"; break;
        }

        timestamp_ns = ktime_to_ns(evt->timestamp);
        count += snprintf(buf + count, size - count,
                          "%s: ts=%lld, irq=%u, data=%u\n",
                          type_str, timestamp_ns, evt->irq_num, evt->data);
    }

    /* 清空已读 */
    atomic_set(&dev->event_tail, head);

    return count;
}

/*
 * 字符设备接口
 */
static int irqhandler_open(struct inode *inode, struct file *filp)
{
    struct irqhandler_dev *dev = container_of(inode->i_cdev, struct irqhandler_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static int irqhandler_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t irqhandler_read(struct file *filp, char __user *user_buf,
                                size_t count, loff_t *f_pos)
{
    struct irqhandler_dev *dev = filp->private_data;
    char *buf;
    ssize_t len;

    buf = kzalloc(PAGE_SIZE, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    /* 等待数据就绪 */
    if (!(filp->f_flags & O_NONBLOCK)) {
        wait_event_interruptible(dev->wait_queue, atomic_read(&dev->data_ready));
    }

    /* 生成报告 */
    len = irqhandler_read_stats(dev, buf, PAGE_SIZE);
    len += irqhandler_read_events(dev, buf + len, PAGE_SIZE - len);

    atomic_set(&dev->data_ready, 0);

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

static unsigned int irqhandler_poll(struct file *filp, poll_table *wait)
{
    struct irqhandler_dev *dev = filp->private_data;
    unsigned int mask = 0;

    poll_wait(filp, &dev->wait_queue, wait);

    if (atomic_read(&dev->data_ready))
        mask |= POLLIN | POLLRDNORM;

    return mask;
}

static const struct file_operations irqhandler_fops = {
    .owner  = THIS_MODULE,
    .open   = irqhandler_open,
    .release = irqhandler_release,
    .read   = irqhandler_read,
    .poll   = irqhandler_poll,
};

/*
 * 平台探测函数
 */
static int irqhandler_probe(struct platform_device *pdev)
{
    struct irqhandler_dev *dev;
    int ret;

    dev_info(&pdev->dev, "Probing Interrupt Handler driver\n");

    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->pdev = pdev;
    platform_set_drvdata(pdev, dev);

    /* 初始化自旋锁 */
    spin_lock_init(&dev->top_half_lock);
    spin_lock_init(&dev->tasklet_lock);
    spin_lock_init(&dev->work_lock);

    /* 初始化原子变量 */
    atomic_set(&dev->event_head, 0);
    atomic_set(&dev->event_tail, 0);
    atomic_set(&dev->data_ready, 0);

    /* 初始化等待队列 */
    init_waitqueue_head(&dev->wait_queue);

    /* 获取GPIO */
    dev->irq_gpio = devm_gpiod_get(&pdev->dev, "irq", GPIOD_IN);
    if (IS_ERR(dev->irq_gpio)) {
        ret = PTR_ERR(dev->irq_gpio);
        dev_err(&pdev->dev, "Failed to get IRQ GPIO: %d\n", ret);
        return ret;
    }

    /* 获取中断号 */
    dev->irq_num = gpiod_to_irq(dev->irq_gpio);
    if (dev->irq_num < 0) {
        dev_err(&pdev->dev, "Failed to get IRQ number\n");
        return dev->irq_num;
    }

    dev_info(&pdev->dev, "Using IRQ %d\n", dev->irq_num);

    /* 初始化tasklet */
    tasklet_init(&dev->my_tasklet, irqhandler_tasklet_handler, (unsigned long)dev);

    /* 创建工作队列 */
    dev->workqueue = alloc_workqueue("irqhandler_wq", WQ_HIGHPRI | WQ_UNBOUND, 1);
    if (!dev->workqueue) {
        dev_err(&pdev->dev, "Failed to create workqueue\n");
        return -ENOMEM;
    }

    INIT_WORK(&dev->my_work, irqhandler_work_handler);
    INIT_WORK(&dev->threaded_work, irqhandler_threaded_work_handler);

    /*
     * 注册中断
     * 方式1: 标准中断处理
     */
    ret = devm_request_irq(&pdev->dev, dev->irq_num, irqhandler_top_half,
                           IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                           "irqhandler", dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ: %d\n", ret);
        destroy_workqueue(dev->workqueue);
        return ret;
    }

    /*
     * 方式2: 线程化中断 (可选，取消注释使用)
     * ret = devm_request_threaded_irq(&pdev->dev, dev->irq_num,
     *                                  irqhandler_top_half,
     *                                  irqhandler_thread_fn,
     *                                  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
     *                                  "irqhandler_threaded", dev);
     */

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret) {
        destroy_workqueue(dev->workqueue);
        return ret;
    }

    /* 创建设备类 */
    dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        unregister_chrdev_region(dev->dev_num, 1);
        destroy_workqueue(dev->workqueue);
        return ret;
    }

    /* 创建设备 */
    device_create(dev->class, &pdev->dev, dev->dev_num, dev, DRIVER_NAME);

    /* 初始化字符设备 */
    cdev_init(&dev->cdev, &irqhandler_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->dev_num, 1);
    if (ret) {
        device_destroy(dev->class, dev->dev_num);
        class_destroy(dev->class);
        unregister_chrdev_region(dev->dev_num, 1);
        destroy_workqueue(dev->workqueue);
        return ret;
    }

    dev_info(&pdev->dev, "Interrupt Handler driver loaded\n");
    dev_info(&pdev->dev, "Read stats from: /dev/%s\n", DRIVER_NAME);

    return 0;
}

/*
 * 平台移除函数
 */
static int irqhandler_remove(struct platform_device *pdev)
{
    struct irqhandler_dev *dev = platform_get_drvdata(pdev);

    /* 删除字符设备 */
    cdev_del(&dev->cdev);
    device_destroy(dev->class, dev->dev_num);
    class_destroy(dev->class);
    unregister_chrdev_region(dev->dev_num, 1);

    /* 销毁workqueue */
    destroy_workqueue(dev->workqueue);

    /* 杀死tasklet */
    tasklet_kill(&dev->my_tasklet);

    /* 中断会自动释放 (devm_) */

    dev_info(&pdev->dev, "Interrupt Handler driver removed\n");

    return 0;
}

static const struct of_device_id irqhandler_of_match[] = {
    { .compatible = "myvendor,irqhandler", },
    { }
};
MODULE_DEVICE_TABLE(of, irqhandler_of_match);

static struct platform_driver irqhandler_driver = {
    .probe  = irqhandler_probe,
    .remove = irqhandler_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(irqhandler_of_match),
    },
};

module_platform_driver(irqhandler_driver);
