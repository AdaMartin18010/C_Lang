/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 1450
 * Language: c
 * Block ID: 7ae03664
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* my_pmu_driver.c - 简单PMU驱动示例 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/msr.h>

#define DEVICE_NAME "mypmu"
#define CLASS_NAME "mypmu_class"

/* PMU设备结构体 */
struct my_pmu_device {
    struct perf_event_attr attr;
    struct perf_event *event;
    int active;
};

static struct class *mypmu_class = NULL;
static struct device *mypmu_device = NULL;
static dev_t dev_num;
static struct cdev mypmu_cdev;
static struct my_pmu_device pmu_dev;

/* 配置PMU事件 */
static int mypmu_config_event(struct my_pmu_device *dev, u64 config) {
    memset(&dev->attr, 0, sizeof(struct perf_event_attr));
    dev->attr.type = PERF_TYPE_HARDWARE;
    dev->attr.size = sizeof(struct perf_event_attr);
    dev->attr.config = config;
    dev->attr.disabled = 1;
    dev->attr.exclude_kernel = 0;
    dev->attr.exclude_hv = 1;

    return 0;
}

/* 打开设备 */
static int mypmu_open(struct inode *inode, struct file *file) {
    pr_info("mypmu: device opened\n");
    return 0;
}

/* 释放设备 */
static int mypmu_release(struct inode *inode, struct file *file) {
    if (pmu_dev.event) {
        perf_event_release_kernel(pmu_dev.event);
        pmu_dev.event = NULL;
        pmu_dev.active = 0;
    }
    pr_info("mypmu: device released\n");
    return 0;
}

/* IOCTL处理 */
static long mypmu_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    u64 value;
    u64 config;

    switch (cmd) {
    case MYPMU_IOC_CONFIG:
        /* 配置事件 */
        if (copy_from_user(&config, (void __user *)arg, sizeof(u64)))
            return -EFAULT;

        if (pmu_dev.event) {
            perf_event_release_kernel(pmu_dev.event);
            pmu_dev.event = NULL;
        }

        mypmu_config_event(&pmu_dev, config);
        pmu_dev.event = perf_event_create_kernel_counter(&pmu_dev.attr, 0, NULL, NULL, NULL);
        if (IS_ERR(pmu_dev.event)) {
            pr_err("mypmu: failed to create perf event\n");
            return PTR_ERR(pmu_dev.event);
        }

        pmu_dev.active = 0;
        pr_info("mypmu: configured event 0x%llx\n", config);
        break;

    case MYPMU_IOC_START:
        /* 启动计数 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_enable(pmu_dev.event);
        pmu_dev.active = 1;
        pr_info("mypmu: started counting\n");
        break;

    case MYPMU_IOC_STOP:
        /* 停止计数 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_disable(pmu_dev.event);
        pmu_dev.active = 0;
        pr_info("mypmu: stopped counting\n");
        break;

    case MYPMU_IOC_READ:
        /* 读取计数值 */
        if (!pmu_dev.event)
            return -EINVAL;

        value = perf_event_read_value(pmu_dev.event, &value, &value);

        if (copy_to_user((void __user *)arg, &value, sizeof(u64)))
            return -EFAULT;
        break;

    case MYPMU_IOC_RESET:
        /* 重置计数器 */
        if (!pmu_dev.event)
            return -EINVAL;

        perf_event_reset(pmu_dev.event);
        pr_info("mypmu: counter reset\n");
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations mypmu_fops = {
    .owner = THIS_MODULE,
    .open = mypmu_open,
    .release = mypmu_release,
    .unlocked_ioctl = mypmu_ioctl,
};

/* IOCTL命令定义 */
#define MYPMU_IOC_MAGIC  'P'
#define MYPMU_IOC_CONFIG  _IOW(MYPMU_IOC_MAGIC, 0, u64)
#define MYPMU_IOC_START   _IO(MYPMU_IOC_MAGIC, 1)
#define MYPMU_IOC_STOP    _IO(MYPMU_IOC_MAGIC, 2)
#define MYPMU_IOC_READ    _IOR(MYPMU_IOC_MAGIC, 3, u64)
#define MYPMU_IOC_RESET   _IO(MYPMU_IOC_MAGIC, 4)

static int __init mypmu_init(void) {
    int ret;

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("mypmu: failed to allocate device number\n");
        return ret;
    }

    /* 初始化cdev */
    cdev_init(&mypmu_cdev, &mypmu_fops);
    mypmu_cdev.owner = THIS_MODULE;

    ret = cdev_add(&mypmu_cdev, dev_num, 1);
    if (ret) {
        pr_err("mypmu: failed to add cdev\n");
        unregister_chrdev_region(dev_num, 1);
        return ret;
    }

    /* 创建类 */
    mypmu_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mypmu_class)) {
        pr_err("mypmu: failed to create class\n");
        cdev_del(&mypmu_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mypmu_class);
    }

    /* 创建设备 */
    mypmu_device = device_create(mypmu_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(mypmu_device)) {
        pr_err("mypmu: failed to create device\n");
        class_destroy(mypmu_class);
        cdev_del(&mypmu_cdev);
        unregister_chrdev_region(dev_num, 1);
        return PTR_ERR(mypmu_device);
    }

    memset(&pmu_dev, 0, sizeof(pmu_dev));

    pr_info("mypmu: driver loaded (major=%d)\n", MAJOR(dev_num));
    return 0;
}

static void __exit mypmu_exit(void) {
    if (pmu_dev.event) {
        perf_event_release_kernel(pmu_dev.event);
    }

    device_destroy(mypmu_class, dev_num);
    class_destroy(mypmu_class);
    cdev_del(&mypmu_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("mypmu: driver unloaded\n");
}

module_init(mypmu_init);
module_exit(mypmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Performance Analysis Team");
MODULE_DESCRIPTION("Custom PMU Driver Example");
