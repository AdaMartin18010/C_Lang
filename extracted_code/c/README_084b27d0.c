/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\README.md
 * Line: 200
 * Language: c
 * Block ID: 084b27d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 高级模块示例：字符设备 + 并发控制
 * 文件: advanced_module.c
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define DEVICE_NAME "mydevice"
#define CLASS_NAME "myclass"
#define BUFFER_SIZE 1024
#define MAX_DEVICES 4

/* 设备私有数据结构 */
struct mydevice_data {
    struct cdev cdev;
    dev_t dev_num;

    /* 数据缓冲区 */
    char *buffer;
    size_t buf_size;
    size_t data_len;

    /* 位置指针 */
    loff_t read_pos;
    loff_t write_pos;

    /* 并发控制 */
    struct mutex mutex;         /* 互斥锁 - 保护缓冲区 */
    spinlock_t spinlock;        /* 自旋锁 - 保护快速操作 */

    /* 同步机制 */
    wait_queue_head_t read_queue;   /* 读等待队列 */
    wait_queue_head_t write_queue;  /* 写等待队列 */

    /* 统计信息 */
    atomic_t open_count;
    atomic_t read_count;
    atomic_t write_count;

    /* 工作队列 */
    struct work_struct deferred_work;

    /* 设备状态 */
    bool device_open;
    bool data_available;
};

static struct mydevice_data *devices[MAX_DEVICES];
static int major;
static struct class *my_class;

/* 文件操作: open */
static int mydevice_open(struct inode *inode, struct file *filp)
{
    struct mydevice_data *dev;
    int minor = iminor(inode);

    if (minor >= MAX_DEVICES)
        return -ENODEV;

    dev = devices[minor];
    if (!dev)
        return -ENODEV;

    /* 非阻塞检查 */
    if (filp->f_flags & O_EXCL) {
        if (dev->device_open)
            return -EBUSY;
    }

    /* 获取互斥锁 */
    if (mutex_lock_interruptible(&dev->mutex))
        return -ERESTARTSYS;

    dev->device_open = true;
    atomic_inc(&dev->open_count);

    /* 保存设备指针供其他操作使用 */
    filp->private_data = dev;

    mutex_unlock(&dev->mutex);

    pr_info("mydevice: opened device %d\n", minor);
    return 0;
}

/* 文件操作: release */
static int mydevice_release(struct inode *inode, struct file *filp)
{
    struct mydevice_data *dev = filp->private_data;

    mutex_lock(&dev->mutex);
    dev->device_open = false;
    mutex_unlock(&dev->mutex);

    pr_info("mydevice: closed device\n");
    return 0;
}

/* 文件操作: read */
static ssize_t mydevice_read(struct file *filp, char __user *user_buf,
                             size_t count, loff_t *f_pos)
{
    struct mydevice_data *dev = filp->private_data;
    ssize_t bytes_read = 0;

    /* 获取互斥锁 */
    if (mutex_lock_interruptible(&dev->mutex))
        return -ERESTARTSYS;

    /* 等待数据可用（阻塞模式） */
    while (dev->data_len == 0) {
        mutex_unlock(&dev->mutex);

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        /* 等待唤醒 */
        if (wait_event_interruptible(dev->read_queue,
                                     dev->data_len > 0))
            return -ERESTARTSYS;

        if (mutex_lock_interruptible(&dev->mutex))
            return -ERESTARTSYS;
    }

    /* 计算可读字节数 */
    size_t available = min(count, dev->data_len - dev->read_pos);

    /* 复制到用户空间 */
    if (copy_to_user(user_buf, dev->buffer + dev->read_pos, available)) {
        mutex_unlock(&dev->mutex);
        return -EFAULT;
    }

    dev->read_pos += available;
    bytes_read = available;

    /* 如果读完所有数据，重置位置 */
    if (dev->read_pos >= dev->data_len) {
        dev->read_pos = 0;
        dev->data_len = 0;
        dev->data_available = false;
    }

    atomic_inc(&dev->read_count);
    mutex_unlock(&dev->mutex);

    /* 唤醒等待的写者 */
    wake_up_interruptible(&dev->write_queue);

    return bytes_read;
}

/* 文件操作: write */
static ssize_t mydevice_write(struct file *filp, const char __user *user_buf,
                              size_t count, loff_t *f_pos)
{
    struct mydevice_data *dev = filp->private_data;
    ssize_t bytes_written = 0;

    if (mutex_lock_interruptible(&dev->mutex))
        return -ERESTARTSYS;

    /* 等待缓冲区空间（阻塞模式） */
    while (dev->data_len >= dev->buf_size) {
        mutex_unlock(&dev->mutex);

        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        if (wait_event_interruptible(dev->write_queue,
                                     dev->data_len < dev->buf_size))
            return -ERESTARTSYS;

        if (mutex_lock_interruptible(&dev->mutex))
            return -ERESTARTSYS;
    }

    /* 计算可写字节数 */
    size_t space = dev->buf_size - dev->data_len;
    size_t to_write = min(count, space);

    /* 从用户空间复制 */
    if (copy_from_user(dev->buffer + dev->data_len, user_buf, to_write)) {
        mutex_unlock(&dev->mutex);
        return -EFAULT;
    }

    dev->data_len += to_write;
    dev->data_available = true;
    bytes_written = to_write;

    atomic_inc(&dev->write_count);
    mutex_unlock(&dev->mutex);

    /* 唤醒等待的读者 */
    wake_up_interruptible(&dev->read_queue);

    return bytes_written;
}

/* 文件操作: ioctl */
static long mydevice_ioctl(struct file *filp, unsigned int cmd,
                           unsigned long arg)
{
    struct mydevice_data *dev = filp->private_data;
    int ret = 0;

    /* 检查幻数和命令号 */
    if (_IOC_TYPE(cmd) != MYDEVICE_MAGIC)
        return -ENOTTY;

    switch (cmd) {
    case MYDEVICE_CLEAR:
        mutex_lock(&dev->mutex);
        dev->data_len = 0;
        dev->read_pos = 0;
        dev->write_pos = 0;
        dev->data_available = false;
        mutex_unlock(&dev->mutex);
        break;

    case MYDEVICE_GET_STATS: {
        struct mydevice_stats stats;
        stats.open_count = atomic_read(&dev->open_count);
        stats.read_count = atomic_read(&dev->read_count);
        stats.write_count = atomic_read(&dev->write_count);
        stats.buffer_size = dev->buf_size;
        stats.data_length = dev->data_len;

        if (copy_to_user((void __user *)arg, &stats, sizeof(stats)))
            ret = -EFAULT;
        break;
    }

    case MYDEVICE_RESIZE: {
        size_t new_size;
        if (copy_from_user(&new_size, (void __user *)arg, sizeof(new_size)))
            return -EFAULT;

        mutex_lock(&dev->mutex);
        char *new_buf = krealloc(dev->buffer, new_size, GFP_KERNEL);
        if (!new_buf) {
            ret = -ENOMEM;
        } else {
            dev->buffer = new_buf;
            dev->buf_size = new_size;
        }
        mutex_unlock(&dev->mutex);
        break;
    }

    default:
        ret = -ENOTTY;
    }

    return ret;
}

/* 文件操作: poll/select */
static unsigned int mydevice_poll(struct file *filp, poll_table *wait)
{
    struct mydevice_data *dev = filp->private_data;
    unsigned int mask = 0;

    poll_wait(filp, &dev->read_queue, wait);
    poll_wait(filp, &dev->write_queue, wait);

    mutex_lock(&dev->mutex);

    if (dev->data_len > 0)
        mask |= POLLIN | POLLRDNORM;  /* 可读 */

    if (dev->data_len < dev->buf_size)
        mask |= POLLOUT | POLLWRNORM;  /* 可写 */

    mutex_unlock(&dev->mutex);

    return mask;
}

/* 文件操作表 */
static const struct file_operations mydevice_fops = {
    .owner          = THIS_MODULE,
    .open           = mydevice_open,
    .release        = mydevice_release,
    .read           = mydevice_read,
    .write          = mydevice_write,
    .unlocked_ioctl = mydevice_ioctl,
    .poll           = mydevice_poll,
    .llseek         = default_llseek,
};

/* 模块初始化 */
static int __init mydevice_init(void)
{
    int ret, i;
    dev_t dev_num;

    /* 动态分配设备号 */
    ret = alloc_chrdev_region(&dev_num, 0, MAX_DEVICES, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }
    major = MAJOR(dev_num);

    /* 创建设备类 */
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) {
        ret = PTR_ERR(my_class);
        goto err_unregister;
    }

    /* 初始化每个设备 */
    for (i = 0; i < MAX_DEVICES; i++) {
        devices[i] = kzalloc(sizeof(struct mydevice_data), GFP_KERNEL);
        if (!devices[i]) {
            ret = -ENOMEM;
            goto err_cleanup;
        }

        /* 分配缓冲区 */
        devices[i]->buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
        if (!devices[i]->buffer) {
            ret = -ENOMEM;
            goto err_cleanup;
        }
        devices[i]->buf_size = BUFFER_SIZE;

        /* 初始化同步原语 */
        mutex_init(&devices[i]->mutex);
        spin_lock_init(&devices[i]->spinlock);
        init_waitqueue_head(&devices[i]->read_queue);
        init_waitqueue_head(&devices[i]->write_queue);

        /* 初始化原子变量 */
        atomic_set(&devices[i]->open_count, 0);
        atomic_set(&devices[i]->read_count, 0);
        atomic_set(&devices[i]->write_count, 0);

        /* 注册字符设备 */
        devices[i]->dev_num = MKDEV(major, i);
        cdev_init(&devices[i]->cdev, &mydevice_fops);
        devices[i]->cdev.owner = THIS_MODULE;

        ret = cdev_add(&devices[i]->cdev, devices[i]->dev_num, 1);
        if (ret) {
            pr_err("Failed to add cdev %d\n", i);
            goto err_cleanup;
        }

        /* 创建设备节点 */
        device_create(my_class, NULL, devices[i]->dev_num,
                      NULL, "%s%d", DEVICE_NAME, i);
    }

    pr_info("mydevice: Loaded with %d devices (major=%d)\n",
            MAX_DEVICES, major);
    return 0;

err_cleanup:
    for (i = i - 1; i >= 0; i--) {
        if (devices[i]) {
            device_destroy(my_class, devices[i]->dev_num);
            cdev_del(&devices[i]->cdev);
            kfree(devices[i]->buffer);
            kfree(devices[i]);
        }
    }
    class_destroy(my_class);
err_unregister:
    unregister_chrdev_region(MKDEV(major, 0), MAX_DEVICES);
    return ret;
}

/* 模块卸载 */
static void __exit mydevice_exit(void)
{
    int i;

    for (i = 0; i < MAX_DEVICES; i++) {
        if (devices[i]) {
            device_destroy(my_class, devices[i]->dev_num);
            cdev_del(&devices[i]->cdev);
            kfree(devices[i]->buffer);
            kfree(devices[i]);
        }
    }

    class_destroy(my_class);
    unregister_chrdev_region(MKDEV(major, 0), MAX_DEVICES);

    pr_info("mydevice: Unloaded\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);
