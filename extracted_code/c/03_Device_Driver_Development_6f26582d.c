/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 654
 * Language: c
 * Block ID: 6f26582d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: char_driver_basic.c
 * Description: 完整的字符设备驱动基础实现
 * Major/Minor设备号、file_operations、设备注册与注销
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>           /* 文件系统相关 */
#include <linux/cdev.h>         /* 字符设备 */
#include <linux/device.h>       /* 设备类 */
#include <linux/slab.h>         /* 内存分配 */
#include <linux/uaccess.h>      /* 用户空间内存访问 */
#include <linux/mutex.h>        /* 互斥锁 */
#include <linux/semaphore.h>    /* 信号量 */
#include <linux/wait.h>         /* 等待队列 */
#include <linux/poll.h>         /* 轮询 */
#include <linux/ioctl.h>        /* ioctl */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Driver Developer");
MODULE_DESCRIPTION("Complete Character Device Driver");

/* 设备配置 */
#define DEVICE_NAME     "mychar"
#define CLASS_NAME      "mychar_class"
#define BUFFER_SIZE     1024
#define MAX_DEVICES     4       /* 支持的最大设备数 */

/*
 * ioctl命令定义
 * _IO(type, nr): 无参数命令
 * _IOR(type, nr, datatype): 从驱动读取数据
 * _IOW(type, nr, datatype): 向驱动写入数据
 * _IOWR(type, nr, datatype): 双向数据传输
 */
#define MYCHAR_IOC_MAGIC    'M'
#define MYCHAR_IOC_RESET    _IO(MYCHAR_IOC_MAGIC, 0)
#define MYCHAR_IOC_GET_LEN  _IOR(MYCHAR_IOC_MAGIC, 1, int)
#define MYCHAR_IOC_SET_LEN  _IOW(MYCHAR_IOC_MAGIC, 2, int)
#define MYCHAR_IOC_GET_DATA _IOR(MYCHAR_IOC_MAGIC, 3, char[256])
#define MYCHAR_IOC_MAXNR    3

/* 设备私有数据结构 */
struct mychar_device {
    struct cdev cdev;               /* 字符设备 */
    struct mutex lock;              /* 互斥锁 */
    wait_queue_head_t read_wq;      /* 读等待队列 */
    wait_queue_head_t write_wq;     /* 写等待队列 */

    char *buffer;                   /* 数据缓冲区 */
    size_t buffer_size;             /* 缓冲区大小 */
    size_t data_len;                /* 当前数据长度 */
    loff_t read_pos;                /* 读位置 */
    loff_t write_pos;               /* 写位置 */

    bool readable;                  /* 可读标志 */
    bool writable;                  /* 可写标志 */

    int device_index;               /* 设备索引 */
    unsigned int open_count;        /* 打开计数 */
};

/* 驱动全局数据 */
static struct mychar_driver {
    dev_t dev_num;                          /* 设备号 */
    struct class *class;                    /* 设备类 */
    struct mychar_device *devices[MAX_DEVICES];  /* 设备数组 */
    int num_devices;                        /* 实际设备数 */
} mychar_drv;

/* 模块参数：设备数量 */
static int num_devices = 2;
module_param(num_devices, int, 0444);
MODULE_PARM_DESC(num_devices, "Number of devices to create (1-4, default: 2)");

/*
 * 打开设备
 * 对应用户空间: open("/dev/mychar0", O_RDWR)
 */
static int mychar_open(struct inode *inode, struct file *filp)
{
    struct mychar_device *dev;

    /* 通过inode获取设备结构体指针 */
    dev = container_of(inode->i_cdev, struct mychar_device, cdev);
    filp->private_data = dev;  /* 保存供其他操作使用 */

    mutex_lock(&dev->lock);
    dev->open_count++;
    dev->readable = true;
    dev->writable = true;
    mutex_unlock(&dev->lock);

    printk(KERN_INFO "mychar%d: opened (count=%u)\n",
           dev->device_index, dev->open_count);

    return 0;
}

/*
 * 关闭设备
 * 对应用户空间: close(fd)
 */
static int mychar_release(struct inode *inode, struct file *filp)
{
    struct mychar_device *dev = filp->private_data;

    mutex_lock(&dev->lock);
    dev->open_count--;
    mutex_unlock(&dev->lock);

    printk(KERN_INFO "mychar%d: released (count=%u)\n",
           dev->device_index, dev->open_count);

    return 0;
}

/*
 * 读取数据
 * 对应用户空间: read(fd, buf, count)
 */
static ssize_t mychar_read(struct file *filp, char __user *user_buf,
                           size_t count, loff_t *f_pos)
{
    struct mychar_device *dev = filp->private_data;
    ssize_t bytes_read = 0;
    size_t available;

    /* 检查非阻塞模式 */
    if (filp->f_flags & O_NONBLOCK) {
        mutex_lock(&dev->lock);
        if (dev->data_len == 0) {
            mutex_unlock(&dev->lock);
            return -EAGAIN;  /* 资源暂时不可用 */
        }
        mutex_unlock(&dev->lock);
    }

    /* 等待数据可用 (可中断睡眠) */
    if (wait_event_interruptible(dev->read_wq, dev->data_len > 0))
        return -ERESTARTSYS;  /* 被信号中断 */

    mutex_lock(&dev->lock);

    /* 计算可读取的数据量 */
    available = min(count, dev->data_len - (size_t)*f_pos);

    if (available == 0) {
        mutex_unlock(&dev->lock);
        return 0;  /* EOF */
    }

    /* 复制数据到用户空间 */
    if (copy_to_user(user_buf, dev->buffer + *f_pos, available)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;  /* 内存访问错误 */
    }

    *f_pos += available;
    bytes_read = available;

    /* 如果读取到末尾，重置缓冲区 */
    if (*f_pos >= dev->data_len) {
        dev->data_len = 0;
        *f_pos = 0;
        dev->read_pos = 0;
        dev->write_pos = 0;
    }

    mutex_unlock(&dev->lock);

    /* 唤醒等待写入的进程 */
    wake_up_interruptible(&dev->write_wq);

    printk(KERN_DEBUG "mychar%d: read %zu bytes\n",
           dev->device_index, bytes_read);

    return bytes_read;
}

/*
 * 写入数据
 * 对应用户空间: write(fd, buf, count)
 */
static ssize_t mychar_write(struct file *filp, const char __user *user_buf,
                            size_t count, loff_t *f_pos)
{
    struct mychar_device *dev = filp->private_data;
    ssize_t bytes_written = 0;
    size_t space_available;

    /* 检查非阻塞模式 */
    if (filp->f_flags & O_NONBLOCK) {
        mutex_lock(&dev->lock);
        if (dev->data_len >= dev->buffer_size) {
            mutex_unlock(&dev->lock);
            return -EAGAIN;
        }
        mutex_unlock(&dev->lock);
    }

    /* 等待空间可用 */
    if (wait_event_interruptible(dev->write_wq,
                                  dev->data_len < dev->buffer_size))
        return -ERESTARTSYS;

    mutex_lock(&dev->lock);

    /* 计算可用空间 */
    space_available = min(count, dev->buffer_size - dev->data_len);

    if (space_available == 0) {
        mutex_unlock(&dev->lock);
        return -ENOSPC;  /* 没有空间 */
    }

    /* 从用户空间复制数据 */
    if (copy_from_user(dev->buffer + dev->data_len, user_buf, space_available)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;
    }

    dev->data_len += space_available;
    *f_pos = dev->data_len;
    bytes_written = space_available;

    mutex_unlock(&dev->lock);

    /* 唤醒等待读取的进程 */
    wake_up_interruptible(&dev->read_wq);

    printk(KERN_DEBUG "mychar%d: wrote %zu bytes\n",
           dev->device_index, bytes_written);

    return bytes_written;
}

/*
 * 定位文件位置
 * 对应用户空间: lseek(fd, offset, whence)
 */
static loff_t mychar_llseek(struct file *filp, loff_t offset, int whence)
{
    struct mychar_device *dev = filp->private_data;
    loff_t new_pos;

    mutex_lock(&dev->lock);

    switch (whence) {
    case SEEK_SET:  /* 从文件开头 */
        new_pos = offset;
        break;
    case SEEK_CUR:  /* 从当前位置 */
        new_pos = filp->f_pos + offset;
        break;
    case SEEK_END:  /* 从文件末尾 */
        new_pos = dev->data_len + offset;
        break;
    default:
        mutex_unlock(&dev->lock);
        return -EINVAL;
    }

    /* 检查边界 */
    if (new_pos < 0 || new_pos > dev->buffer_size) {
        mutex_unlock(&dev->lock);
        return -EINVAL;
    }

    filp->f_pos = new_pos;

    mutex_unlock(&dev->lock);

    return new_pos;
}

/*
 * ioctl - 设备控制
 * 对应用户空间: ioctl(fd, cmd, arg)
 * 新内核推荐使用unlocked_ioctl
 */
static long mychar_unlocked_ioctl(struct file *filp, unsigned int cmd,
                                   unsigned long arg)
{
    struct mychar_device *dev = filp->private_data;
    int err = 0;
    int tmp;
    char data[256];

    /* 检查幻数 */
    if (_IOC_TYPE(cmd) != MYCHAR_IOC_MAGIC)
        return -ENOTTY;

    /* 检查命令号范围 */
    if (_IOC_NR(cmd) > MYCHAR_IOC_MAXNR)
        return -ENOTTY;

    /* 检查访问权限 */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    if (err)
        return -EFAULT;

    switch (cmd) {
    case MYCHAR_IOC_RESET:
        /* 重置设备 */
        mutex_lock(&dev->lock);
        dev->data_len = 0;
        dev->read_pos = 0;
        dev->write_pos = 0;
        memset(dev->buffer, 0, dev->buffer_size);
        mutex_unlock(&dev->lock);
        printk(KERN_INFO "mychar%d: device reset\n", dev->device_index);
        break;

    case MYCHAR_IOC_GET_LEN:
        /* 获取当前数据长度 */
        mutex_lock(&dev->lock);
        tmp = dev->data_len;
        mutex_unlock(&dev->lock);
        if (copy_to_user((int __user *)arg, &tmp, sizeof(int)))
            return -EFAULT;
        break;

    case MYCHAR_IOC_SET_LEN:
        /* 设置数据长度 (仅用于测试) */
        if (copy_from_user(&tmp, (int __user *)arg, sizeof(int)))
            return -EFAULT;
        mutex_lock(&dev->lock);
        if (tmp >= 0 && tmp <= dev->buffer_size)
            dev->data_len = tmp;
        mutex_unlock(&dev->lock);
        break;

    case MYCHAR_IOC_GET_DATA:
        /* 获取数据副本 */
        mutex_lock(&dev->lock);
        tmp = min((size_t)256, dev->data_len);
        memcpy(data, dev->buffer, tmp);
        mutex_unlock(&dev->lock);
        if (copy_to_user((char __user *)arg, data, tmp))
            return -EFAULT;
        break;

    default:
        return -ENOTTY;  /* 不支持的命令 */
    }

    return 0;
}

/*
 * poll/select 支持
 * 对应用户空间: poll(), select(), epoll()
 */
static unsigned int mychar_poll(struct file *filp, poll_table *wait)
{
    struct mychar_device *dev = filp->private_data;
    unsigned int mask = 0;

    /* 将等待队列添加到poll_table */
    poll_wait(filp, &dev->read_wq, wait);
    poll_wait(filp, &dev->write_wq, wait);

    mutex_lock(&dev->lock);

    /* 可读检查 */
    if (dev->data_len > 0)
        mask |= POLLIN | POLLRDNORM;  /* 可读 */

    /* 可写检查 */
    if (dev->data_len < dev->buffer_size)
        mask |= POLLOUT | POLLWRNORM;  /* 可写 */

    mutex_unlock(&dev->lock);

    return mask;
}

/*
 * mmap 支持 (可选)
 * 对应用户空间: mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)
 */
#ifdef CONFIG_MYCHAR_MMAP
#include <linux/mm.h>

static int mychar_mmap(struct file *filp, struct vm_area_struct *vma)
{
    /* mmap实现较复杂，需要分配物理连续内存或使用vm_insert_page */
    /* 这里仅作占位，实际实现需根据需求设计 */
    return -ENOSYS;  /* 功能未实现 */
}
#endif

/* 文件操作表 */
static struct file_operations mychar_fops = {
    .owner          = THIS_MODULE,
    .open           = mychar_open,
    .release        = mychar_release,
    .read           = mychar_read,
    .write          = mychar_write,
    .llseek         = mychar_llseek,
    .unlocked_ioctl = mychar_unlocked_ioctl,
    .poll           = mychar_poll,
#ifdef CONFIG_MYCHAR_MMAP
    .mmap           = mychar_mmap,
#endif
};

/*
 * 创建设备
 */
static int create_mychar_device(int index)
{
    struct mychar_device *dev;
    int ret;
    dev_t dev_num;

    /* 分配设备结构体 */
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    /* 初始化设备 */
    dev->buffer_size = BUFFER_SIZE;
    dev->buffer = kzalloc(dev->buffer_size, GFP_KERNEL);
    if (!dev->buffer) {
        kfree(dev);
        return -ENOMEM;
    }

    dev->device_index = index;
    dev->data_len = 0;
    dev->read_pos = 0;
    dev->write_pos = 0;
    dev->open_count = 0;
    dev->readable = false;
    dev->writable = false;

    mutex_init(&dev->lock);
    init_waitqueue_head(&dev->read_wq);
    init_waitqueue_head(&dev->write_wq);

    /* 初始化cdev */
    cdev_init(&dev->cdev, &mychar_fops);
    dev->cdev.owner = THIS_MODULE;

    /* 计算设备号 */
    dev_num = MKDEV(MAJOR(mychar_drv.dev_num), index);

    /* 添加字符设备 */
    ret = cdev_add(&dev->cdev, dev_num, 1);
    if (ret) {
        printk(KERN_ERR "Failed to add cdev for device %d\n", index);
        kfree(dev->buffer);
        kfree(dev);
        return ret;
    }

    /* 创建设备节点 */
    device_create(mychar_drv.class, NULL, dev_num, NULL,
                  "%s%d", DEVICE_NAME, index);

    mychar_drv.devices[index] = dev;

    printk(KERN_INFO "Created device %s%d (major=%d, minor=%d)\n",
           DEVICE_NAME, index, MAJOR(dev_num), MINOR(dev_num));

    return 0;
}

/*
 * 销毁设备
 */
static void destroy_mychar_device(int index)
{
    struct mychar_device *dev = mychar_drv.devices[index];
    dev_t dev_num;

    if (!dev)
        return;

    dev_num = MKDEV(MAJOR(mychar_drv.dev_num), index);

    /* 销毁设备节点 */
    device_destroy(mychar_drv.class, dev_num);

    /* 删除字符设备 */
    cdev_del(&dev->cdev);

    /* 释放资源 */
    kfree(dev->buffer);
    kfree(dev);

    mychar_drv.devices[index] = NULL;

    printk(KERN_INFO "Destroyed device %s%d\n", DEVICE_NAME, index);
}

/*
 * 模块初始化
 */
static int __init mychar_init(void)
{
    int ret;
    int i;

    /* 限制设备数量 */
    if (num_devices < 1 || num_devices > MAX_DEVICES) {
        printk(KERN_ERR "Invalid num_devices: %d (must be 1-%d)\n",
               num_devices, MAX_DEVICES);
        return -EINVAL;
    }
    mychar_drv.num_devices = num_devices;

    /* 分配设备号范围 */
    ret = alloc_chrdev_region(&mychar_drv.dev_num, 0, num_devices, DEVICE_NAME);
    if (ret) {
        printk(KERN_ERR "Failed to allocate device numbers\n");
        return ret;
    }

    printk(KERN_INFO "Allocated device major: %d\n", MAJOR(mychar_drv.dev_num));

    /* 创建设备类 */
    mychar_drv.class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mychar_drv.class)) {
        ret = PTR_ERR(mychar_drv.class);
        printk(KERN_ERR "Failed to create class\n");
        unregister_chrdev_region(mychar_drv.dev_num, num_devices);
        return ret;
    }

    /* 创建所有设备 */
    for (i = 0; i < num_devices; i++) {
        ret = create_mychar_device(i);
        if (ret) {
            /* 清理已创建的设备 */
            while (--i >= 0)
                destroy_mychar_device(i);
            class_destroy(mychar_drv.class);
            unregister_chrdev_region(mychar_drv.dev_num, num_devices);
            return ret;
        }
    }

    printk(KERN_INFO "MyChar driver loaded with %d devices\n", num_devices);
    printk(KERN_INFO "Create device nodes with: mknod /dev/%s0 c %d 0\n",
           DEVICE_NAME, MAJOR(mychar_drv.dev_num));

    return 0;
}

/*
 * 模块清理
 */
static void __exit mychar_exit(void)
{
    int i;

    /* 销毁所有设备 */
    for (i = 0; i < mychar_drv.num_devices; i++)
        destroy_mychar_device(i);

    /* 销毁类 */
    class_destroy(mychar_drv.class);

    /* 释放设备号 */
    unregister_chrdev_region(mychar_drv.dev_num, mychar_drv.num_devices);

    printk(KERN_INFO "MyChar driver unloaded\n");
}

module_init(mychar_init);
module_exit(mychar_exit);
