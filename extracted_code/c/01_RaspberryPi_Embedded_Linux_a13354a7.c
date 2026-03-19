/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 460
 * Language: c
 * Block ID: a13354a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// char_device.c - 完整字符设备驱动
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/kfifo.h>

#define DRIVER_NAME     "rpi_cdev"
#define CLASS_NAME      "rpi_cdev_class"
#define DEVICE_PREFIX   "rpi_cdev"
#define MAX_DEVICES     8
#define BUFFER_SIZE     (64 * 1024)
#define DMA_BUFFER_SIZE (256 * 1024)

/* IOCTL命令 */
#define CDEV_IOC_MAGIC          'c'
#define CDEV_RESET              _IO(CDEV_IOC_MAGIC, 0)
#define CDEV_GET_STATS          _IOR(CDEV_IOC_MAGIC, 1, struct cdev_stats)
#define CDEV_SET_MODE           _IOW(CDEV_IOC_MAGIC, 2, int)
#define CDEV_IOC_MAXNR          2

/* 设备模式 */
enum cdev_mode {
    MODE_STREAM,
    MODE_PACKET,
    MODE_DMA,
};

/* 统计信息 */
struct cdev_stats {
    u64 bytes_read;
    u64 bytes_written;
    u64 irq_count;
    u32 buffer_used;
    u32 buffer_size;
};

/* 设备私有数据 */
struct rpi_cdev_data {
    struct cdev cdev;
    struct device *device;
    int dev_id;

    unsigned char *buffer;
    size_t buf_size;
    size_t write_pos;
    size_t read_pos;
    size_t data_len;

    dma_addr_t dma_handle;
    void *dma_buffer;
    size_t dma_size;

    struct mutex lock;
    spinlock_t spinlock;
    wait_queue_head_t read_wait;
    wait_queue_head_t write_wait;

    enum cdev_mode mode;
    bool open;

    struct cdev_stats stats;

    DECLARE_KFIFO(event_fifo, u32, 256);
};

static struct class *cdev_class;
static dev_t dev_base;
static struct rpi_cdev_data *cdev_devices[MAX_DEVICES];
static int device_count = MAX_DEVICES;

module_param(device_count, int, 0444);
MODULE_PARM_DESC(device_count, "Number of devices (1-8)");

/* 缓冲区操作 */
static ssize_t buffer_write(struct rpi_cdev_data *dev,
                            const unsigned char *buf, size_t count)
{
    size_t space = dev->buf_size - dev->data_len;
    size_t to_write = min(count, space);
    size_t first_chunk, second_chunk;
    unsigned long flags;

    if (to_write == 0) return 0;

    spin_lock_irqsave(&dev->spinlock, flags);

    first_chunk = min(to_write, dev->buf_size - dev->write_pos);
    second_chunk = to_write - first_chunk;

    memcpy(dev->buffer + dev->write_pos, buf, first_chunk);

    if (second_chunk > 0) {
        memcpy(dev->buffer, buf + first_chunk, second_chunk);
        dev->write_pos = second_chunk;
    } else {
        dev->write_pos += first_chunk;
        if (dev->write_pos >= dev->buf_size)
            dev->write_pos = 0;
    }

    dev->data_len += to_write;
    dev->stats.bytes_written += to_write;

    spin_unlock_irqrestore(&dev->spinlock, flags);
    wake_up_interruptible(&dev->read_wait);

    return to_write;
}

static ssize_t buffer_read(struct rpi_cdev_data *dev,
                           unsigned char *buf, size_t count)
{
    size_t available = dev->data_len;
    size_t to_read = min(count, available);
    size_t first_chunk, second_chunk;
    unsigned long flags;

    if (to_read == 0) return 0;

    spin_lock_irqsave(&dev->spinlock, flags);

    first_chunk = min(to_read, dev->buf_size - dev->read_pos);
    second_chunk = to_read - first_chunk;

    memcpy(buf, dev->buffer + dev->read_pos, first_chunk);

    if (second_chunk > 0) {
        memcpy(buf + first_chunk, dev->buffer, second_chunk);
        dev->read_pos = second_chunk;
    } else {
        dev->read_pos += first_chunk;
        if (dev->read_pos >= dev->buf_size)
            dev->read_pos = 0;
    }

    dev->data_len -= to_read;
    dev->stats.bytes_read += to_read;

    spin_unlock_irqrestore(&dev->spinlock, flags);
    wake_up_interruptible(&dev->write_wait);

    return to_read;
}

/* 文件操作 */
static int cdev_open(struct inode *inode, struct file *filp)
{
    struct rpi_cdev_data *dev;
    int dev_id = MINOR(inode->i_rdev);

    if (dev_id >= device_count || !cdev_devices[dev_id])
        return -ENODEV;

    dev = cdev_devices[dev_id];

    mutex_lock(&dev->lock);
    if (dev->open) {
        mutex_unlock(&dev->lock);
        return -EBUSY;
    }
    dev->open = true;
    filp->private_data = dev;
    mutex_unlock(&dev->lock);

    printk(KERN_INFO "[%s%d] Device opened\n", DRIVER_NAME, dev_id);
    return 0;
}

static int cdev_release(struct inode *inode, struct file *filp)
{
    struct rpi_cdev_data *dev = filp->private_data;

    mutex_lock(&dev->lock);
    dev->write_pos = 0;
    dev->read_pos = 0;
    dev->data_len = 0;
    dev->open = false;
    mutex_unlock(&dev->lock);

    printk(KERN_INFO "[%s%d] Device closed\n", DRIVER_NAME, dev->dev_id);
    return 0;
}

static ssize_t cdev_read(struct file *filp, char __user *buf,
                         size_t count, loff_t *f_pos)
{
    struct rpi_cdev_data *dev = filp->private_data;
    unsigned char *kbuf;
    ssize_t ret;

    if (count == 0) return 0;
    if (count > BUFFER_SIZE) count = BUFFER_SIZE;

    kbuf = kmalloc(count, GFP_KERNEL);
    if (!kbuf) return -ENOMEM;

    if (dev->data_len == 0) {
        if (filp->f_flags & O_NONBLOCK) {
            kfree(kbuf);
            return -EAGAIN;
        }
        ret = wait_event_interruptible(dev->read_wait, dev->data_len > 0);
        if (ret) {
            kfree(kbuf);
            return ret;
        }
    }

    mutex_lock(&dev->lock);
    ret = buffer_read(dev, kbuf, count);
    mutex_unlock(&dev->lock);

    if (ret > 0) {
        if (copy_to_user(buf, kbuf, ret))
            ret = -EFAULT;
    }

    kfree(kbuf);
    return ret;
}

static ssize_t cdev_write(struct file *filp, const char __user *buf,
                          size_t count, loff_t *f_pos)
{
    struct rpi_cdev_data *dev = filp->private_data;
    unsigned char *kbuf;
    ssize_t ret;

    if (count == 0) return 0;
    kbuf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!kbuf) return -ENOMEM;

    if (copy_from_user(kbuf, buf, min(count, (size_t)BUFFER_SIZE))) {
        kfree(kbuf);
        return -EFAULT;
    }

    if ((dev->buf_size - dev->data_len) < min(count, (size_t)BUFFER_SIZE)) {
        if (filp->f_flags & O_NONBLOCK) {
            kfree(kbuf);
            return -EAGAIN;
        }
        ret = wait_event_interruptible(dev->write_wait,
            (dev->buf_size - dev->data_len) >= min(count, (size_t)BUFFER_SIZE));
        if (ret) {
            kfree(kbuf);
            return ret;
        }
    }

    mutex_lock(&dev->lock);
    ret = buffer_write(dev, kbuf, min(count, (size_t)BUFFER_SIZE));
    mutex_unlock(&dev->lock);

    kfree(kbuf);
    return ret;
}

static unsigned int cdev_poll(struct file *filp, poll_table *wait)
{
    struct rpi_cdev_data *dev = filp->private_data;
    unsigned int mask = 0;

    poll_wait(filp, &dev->read_wait, wait);
    poll_wait(filp, &dev->write_wait, wait);

    if (dev->data_len > 0)
        mask |= POLLIN | POLLRDNORM;
    if (dev->data_len < dev->buf_size)
        mask |= POLLOUT | POLLWRNORM;

    return mask;
}

static int cdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
    struct rpi_cdev_data *dev = filp->private_data;
    size_t size = vma->vm_end - vma->vm_start;
    int ret;

    if (size > DMA_BUFFER_SIZE)
        return -EINVAL;

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    ret = dma_mmap_coherent(dev->device, vma, dev->dma_buffer,
                            dev->dma_handle, size);
    if (ret) {
        printk(KERN_ERR "[%s%d] DMA mmap failed: %d\n",
               DRIVER_NAME, dev->dev_id, ret);
        return ret;
    }

    return 0;
}

static long cdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct rpi_cdev_data *dev = filp->private_data;
    int ret = 0;

    if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC)
        return -ENOTTY;
    if (_IOC_NR(cmd) > CDEV_IOC_MAXNR)
        return -ENOTTY;

    mutex_lock(&dev->lock);

    switch (cmd) {
    case CDEV_RESET:
        dev->write_pos = 0;
        dev->read_pos = 0;
        dev->data_len = 0;
        memset(&dev->stats, 0, sizeof(dev->stats));
        printk(KERN_INFO "[%s%d] Device reset\n", DRIVER_NAME, dev->dev_id);
        break;

    case CDEV_GET_STATS:
        dev->stats.buffer_used = dev->data_len;
        dev->stats.buffer_size = dev->buf_size;
        if (copy_to_user((void __user *)arg, &dev->stats, sizeof(dev->stats)))
            ret = -EFAULT;
        break;

    case CDEV_SET_MODE:
        {
            int mode;
            if (copy_from_user(&mode, (void __user *)arg, sizeof(int))) {
                ret = -EFAULT;
                break;
            }
            if (mode < MODE_STREAM || mode > MODE_DMA) {
                ret = -EINVAL;
                break;
            }
            dev->mode = mode;
        }
        break;

    default:
        ret = -ENOTTY;
    }

    mutex_unlock(&dev->lock);
    return ret;
}

static const struct file_operations cdev_fops = {
    .owner          = THIS_MODULE,
    .open           = cdev_open,
    .release        = cdev_release,
    .read           = cdev_read,
    .write          = cdev_write,
    .poll           = cdev_poll,
    .mmap           = cdev_mmap,
    .unlocked_ioctl = cdev_ioctl,
    .llseek         = no_llseek,
};

/* 创建设备 */
static int create_cdev_device(int id)
{
    struct rpi_cdev_data *dev;
    int ret;
    dev_t dev_num = MKDEV(MAJOR(dev_base), id);

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) return -ENOMEM;

    dev->dev_id = id;
    dev->buf_size = BUFFER_SIZE;
    dev->dma_size = DMA_BUFFER_SIZE;

    dev->buffer = kzalloc(dev->buf_size, GFP_KERNEL);
    if (!dev->buffer) { ret = -ENOMEM; goto fail_buffer; }

    dev->dma_buffer = dma_alloc_coherent(NULL, dev->dma_size,
                                          &dev->dma_handle, GFP_KERNEL);

    mutex_init(&dev->lock);
    spin_lock_init(&dev->spinlock);
    init_waitqueue_head(&dev->read_wait);
    init_waitqueue_head(&dev->write_wait);
    INIT_KFIFO(dev->event_fifo);
    dev->mode = MODE_STREAM;

    cdev_init(&dev->cdev, &cdev_fops);
    dev->cdev.owner = THIS_MODULE;

    ret = cdev_add(&dev->cdev, dev_num, 1);
    if (ret) goto fail_cdev;

    dev->device = device_create(cdev_class, NULL, dev_num, NULL,
                                "%s%d", DEVICE_PREFIX, id);
    if (IS_ERR(dev->device)) { ret = PTR_ERR(dev->device); goto fail_device; }

    cdev_devices[id] = dev;
    printk(KERN_INFO "[%s%d] Device created\n", DRIVER_NAME, id);
    return 0;

fail_device:
    cdev_del(&dev->cdev);
fail_cdev:
    if (dev->dma_buffer)
        dma_free_coherent(NULL, dev->dma_size, dev->dma_buffer, dev->dma_handle);
    kfree(dev->buffer);
fail_buffer:
    kfree(dev);
    return ret;
}

static void destroy_cdev_device(int id)
{
    struct rpi_cdev_data *dev = cdev_devices[id];
    dev_t dev_num = MKDEV(MAJOR(dev_base), id);

    if (!dev) return;

    device_destroy(cdev_class, dev_num);
    cdev_del(&dev->cdev);

    if (dev->dma_buffer)
        dma_free_coherent(NULL, dev->dma_size, dev->dma_buffer, dev->dma_handle);

    kfree(dev->buffer);
    kfree(dev);
    cdev_devices[id] = NULL;

    printk(KERN_INFO "[%s%d] Device destroyed\n", DRIVER_NAME, id);
}

static int __init cdev_driver_init(void)
{
    int ret, i;

    if (device_count < 1 || device_count > MAX_DEVICES)
        device_count = MAX_DEVICES;

    printk(KERN_INFO "[%s] Initializing %d devices...\n", DRIVER_NAME, device_count);

    ret = alloc_chrdev_region(&dev_base, 0, device_count, DRIVER_NAME);
    if (ret < 0) return ret;

    cdev_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(cdev_class)) {
        ret = PTR_ERR(cdev_class);
        goto fail_class;
    }

    for (i = 0; i < device_count; i++) {
        ret = create_cdev_device(i);
        if (ret) goto fail_devices;
    }

    printk(KERN_INFO "[%s] Driver loaded\n", DRIVER_NAME);
    return 0;

fail_devices:
    while (--i >= 0) destroy_cdev_device(i);
    class_destroy(cdev_class);
fail_class:
    unregister_chrdev_region(dev_base, device_count);
    return ret;
}

static void __exit cdev_driver_exit(void)
{
    int i;

    for (i = 0; i < device_count; i++)
        destroy_cdev_device(i);

    class_destroy(cdev_class);
    unregister_chrdev_region(dev_base, device_count);

    printk(KERN_INFO "[%s] Driver unloaded\n", DRIVER_NAME);
}

module_init(cdev_driver_init);
module_exit(cdev_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Industrial IoT Team");
MODULE_DESCRIPTION("Raspberry Pi Character Device Driver");
MODULE_VERSION("2.0.0");
