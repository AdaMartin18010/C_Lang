/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 4577
 * Language: c
 * Block ID: e284cac8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: spi_flash_driver.c
 * Description: SPI Flash驱动完整实现 (W25Q系列)
 * 支持读取、写入、擦除、扇区保护等功能
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SPI Driver Developer");
MODULE_DESCRIPTION("SPI Flash Driver for W25Q Series");

#define DRIVER_NAME     "w25q"
#define CLASS_NAME      "w25q"

/* W25Q命令定义 */
#define W25X_WRITE_ENABLE       0x06
#define W25X_WRITE_DISABLE      0x04
#define W25X_READ_STATUS_REG1   0x05
#define W25X_READ_STATUS_REG2   0x35
#define W25X_READ_DATA          0x03
#define W25X_FAST_READ          0x0B
#define W25X_PAGE_PROGRAM       0x02
#define W25X_SECTOR_ERASE       0x20    /* 4KB */
#define W25X_BLOCK_ERASE_32K    0x52
#define W25X_BLOCK_ERASE_64K    0xD8
#define W25X_CHIP_ERASE         0xC7
#define W25X_POWER_DOWN         0xB9
#define W25X_RELEASE_PD         0xAB
#define W25X_JEDEC_ID           0x9F

/* Flash参数 */
#define W25Q_PAGE_SIZE          256
#define W25Q_SECTOR_SIZE        4096
#define W25Q_BLOCK_SIZE_32K     (32 * 1024)
#define W25Q_BLOCK_SIZE_64K     (64 * 1024)

/* 状态寄存器位 */
#define SR1_BUSY                0x01
#define SR1_WEL                 0x02

/* Flash芯片信息 */
struct flash_info {
    char name[16];
    u32 jedec_id;
    u32 capacity;       /* 字节数 */
    u32 page_size;
    u32 sector_size;
    u32 block_size;
    u16 flags;
};

static const struct flash_info flash_info_table[] = {
    { "w25q16", 0xEF4015, 2 * 1024 * 1024,   256, 4096, 65536, 0 },
    { "w25q32", 0xEF4016, 4 * 1024 * 1024,   256, 4096, 65536, 0 },
    { "w25q64", 0xEF4017, 8 * 1024 * 1024,   256, 4096, 65536, 0 },
    { "w25q128", 0xEF4018, 16 * 1024 * 1024, 256, 4096, 65536, 0 },
    { "w25q256", 0xEF4019, 32 * 1024 * 1024, 256, 4096, 65536, 0 },
};

/* 设备私有数据 */
struct w25q_data {
    struct spi_device *spi;
    struct flash_info info;

    struct mutex lock;
    u8 *buffer;                 /* 临时缓冲区 */

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/*
 * 简单SPI传输
 */
static inline int w25q_spi_txrx(struct w25q_data *flash, const u8 *tx, u8 *rx, size_t len)
{
    struct spi_transfer t = {
        .tx_buf = tx,
        .rx_buf = rx,
        .len = len,
    };
    struct spi_message m;

    spi_message_init(&m);
    spi_message_add_tail(&t, &m);

    return spi_sync(flash->spi, &m);
}

/*
 * 发送命令 (无数据阶段)
 */
static inline int w25q_cmd(struct w25q_data *flash, u8 cmd)
{
    return spi_write(flash->spi, &cmd, 1);
}

/*
 * 读取状态寄存器
 */
static u8 w25q_read_sr(struct w25q_data *flash)
{
    u8 cmd = W25X_READ_STATUS_REG1;
    u8 sr;

    w25q_spi_txrx(flash, &cmd, &sr, 2);

    return sr;
}

/*
 * 等待就绪
 */
static int w25q_wait_ready(struct w25q_data *flash, unsigned long timeout)
{
    unsigned long deadline;
    u8 sr;

    deadline = jiffies + timeout;

    do {
        sr = w25q_read_sr(flash);
        if (!(sr & SR1_BUSY))
            return 0;

        cond_resched();
    } while (time_before(jiffies, deadline));

    return -ETIMEDOUT;
}

/*
 * 写使能
 */
static int w25q_write_enable(struct w25q_data *flash)
{
    int ret;

    ret = w25q_cmd(flash, W25X_WRITE_ENABLE);
    if (ret)
        return ret;

    /* 验证WEL位 */
    if (!(w25q_read_sr(flash) & SR1_WEL))
        return -EIO;

    return 0;
}

/*
 * 读JEDEC ID
 */
static int w25q_read_jedec(struct w25q_data *flash, u32 *jedec)
{
    u8 cmd = W25X_JEDEC_ID;
    u8 buf[3];
    int ret;

    ret = w25q_spi_txrx(flash, &cmd, buf, 4);
    if (ret)
        return ret;

    *jedec = (buf[0] << 16) | (buf[1] << 8) | buf[2];

    return 0;
}

/*
 * 读取数据
 */
static int w25q_read(struct w25q_data *flash, loff_t from, size_t len, u8 *buf)
{
    u8 cmd[4];
    struct spi_transfer t[2];
    struct spi_message m;
    int ret;

    if (from + len > flash->info.capacity)
        return -EINVAL;

    cmd[0] = W25X_READ_DATA;
    cmd[1] = (from >> 16) & 0xFF;
    cmd[2] = (from >> 8) & 0xFF;
    cmd[3] = from & 0xFF;

    spi_message_init(&m);

    memset(t, 0, sizeof(t));

    t[0].tx_buf = cmd;
    t[0].len = 4;
    spi_message_add_tail(&t[0], &m);

    t[1].rx_buf = buf;
    t[1].len = len;
    spi_message_add_tail(&t[1], &m);

    mutex_lock(&flash->lock);
    ret = spi_sync(flash->spi, &m);
    mutex_unlock(&flash->lock);

    return ret ? ret : len;
}

/*
 * 页编程 (256字节)
 */
static int w25q_page_program(struct w25q_data *flash, loff_t to, const u8 *buf, size_t len)
{
    u8 cmd[4];
    struct spi_transfer t[2];
    struct spi_message m;
    int ret;

    if (len > W25Q_PAGE_SIZE)
        len = W25Q_PAGE_SIZE;

    /* 写使能 */
    ret = w25q_write_enable(flash);
    if (ret)
        return ret;

    cmd[0] = W25X_PAGE_PROGRAM;
    cmd[1] = (to >> 16) & 0xFF;
    cmd[2] = (to >> 8) & 0xFF;
    cmd[3] = to & 0xFF;

    spi_message_init(&m);

    memset(t, 0, sizeof(t));

    t[0].tx_buf = cmd;
    t[0].len = 4;
    spi_message_add_tail(&t[0], &m);

    t[1].tx_buf = buf;
    t[1].len = len;
    spi_message_add_tail(&t[1], &m);

    ret = spi_sync(flash->spi, &m);
    if (ret)
        return ret;

    /* 等待写入完成 */
    ret = w25q_wait_ready(flash, HZ);

    return ret ? ret : len;
}

/*
 * 写入数据 (处理跨页)
 */
static int w25q_write(struct w25q_data *flash, loff_t to, size_t len, const u8 *buf)
{
    size_t page_offset, page_remain;
    size_t chunk;
    int ret;

    if (to + len > flash->info.capacity)
        return -EINVAL;

    mutex_lock(&flash->lock);

    while (len) {
        page_offset = to & (W25Q_PAGE_SIZE - 1);
        page_remain = W25Q_PAGE_SIZE - page_offset;
        chunk = min(len, page_remain);

        ret = w25q_page_program(flash, to, buf, chunk);
        if (ret < 0)
            goto out;

        buf += chunk;
        to += chunk;
        len -= chunk;
    }

    ret = 0;

out:
    mutex_unlock(&flash->lock);
    return ret;
}

/*
 * 擦除扇区 (4KB)
 */
static int w25q_sector_erase(struct w25q_data *flash, loff_t offset)
{
    u8 cmd[4];
    int ret;

    if (offset & (W25Q_SECTOR_SIZE - 1))
        return -EINVAL;

    /* 写使能 */
    ret = w25q_write_enable(flash);
    if (ret)
        return ret;

    cmd[0] = W25X_SECTOR_ERASE;
    cmd[1] = (offset >> 16) & 0xFF;
    cmd[2] = (offset >> 8) & 0xFF;
    cmd[3] = offset & 0xFF;

    mutex_lock(&flash->lock);
    ret = spi_write(flash->spi, cmd, 4);
    if (ret == 0)
        ret = w25q_wait_ready(flash, 5 * HZ);  /* 擦除需要更长时间 */
    mutex_unlock(&flash->lock);

    return ret;
}

/*
 * 擦除块 (64KB)
 */
static int w25q_block_erase(struct w25q_data *flash, loff_t offset)
{
    u8 cmd[4];
    int ret;

    if (offset & (W25Q_BLOCK_SIZE_64K - 1))
        return -EINVAL;

    ret = w25q_write_enable(flash);
    if (ret)
        return ret;

    cmd[0] = W25X_BLOCK_ERASE_64K;
    cmd[1] = (offset >> 16) & 0xFF;
    cmd[2] = (offset >> 8) & 0xFF;
    cmd[3] = offset & 0xFF;

    mutex_lock(&flash->lock);
    ret = spi_write(flash->spi, cmd, 4);
    if (ret == 0)
        ret = w25q_wait_ready(flash, 10 * HZ);
    mutex_unlock(&flash->lock);

    return ret;
}

/*
 * 整片擦除
 */
static int w25q_chip_erase(struct w25q_data *flash)
{
    int ret;

    ret = w25q_write_enable(flash);
    if (ret)
        return ret;

    mutex_lock(&flash->lock);
    ret = w25q_cmd(flash, W25X_CHIP_ERASE);
    if (ret == 0)
        ret = w25q_wait_ready(flash, 100 * HZ);  /* 整片擦除需要很长时间 */
    mutex_unlock(&flash->lock);

    return ret;
}

/* 字符设备接口 */
static int w25q_open(struct inode *inode, struct file *filp)
{
    struct w25q_data *flash = container_of(inode->i_cdev, struct w25q_data, cdev);
    filp->private_data = flash;
    return 0;
}

static int w25q_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t w25q_read(struct file *filp, char __user *user_buf,
                          size_t count, loff_t *off)
{
    struct w25q_data *flash = filp->private_data;
    u8 *buf;
    int ret;

    if (*off >= flash->info.capacity)
        return 0;

    if (count > flash->info.capacity - *off)
        count = flash->info.capacity - *off;

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    ret = w25q_read(flash, *off, count, buf);
    if (ret < 0) {
        kfree(buf);
        return ret;
    }

    if (copy_to_user(user_buf, buf, ret)) {
        kfree(buf);
        return -EFAULT;
    }

    *off += ret;
    kfree(buf);

    return ret;
}

static ssize_t w25q_write(struct file *filp, const char __user *user_buf,
                           size_t count, loff_t *off)
{
    struct w25q_data *flash = filp->private_data;
    u8 *buf;
    int ret;

    if (*off >= flash->info.capacity)
        return -ENOSPC;

    if (count > flash->info.capacity - *off)
        count = flash->info.capacity - *off;

    buf = memdup_user(user_buf, count);
    if (IS_ERR(buf))
        return PTR_ERR(buf);

    ret = w25q_write(flash, *off, count, buf);

    kfree(buf);

    if (ret < 0)
        return ret;

    *off += count;
    return count;
}

static loff_t w25q_llseek(struct file *filp, loff_t off, int whence)
{
    struct w25q_data *flash = filp->private_data;

    return fixed_size_llseek(filp, off, whence, flash->info.capacity);
}

/* ioctl */
#define W25Q_IOC_MAGIC      'W'
#define W25Q_GET_INFO       _IOR(W25Q_IOC_MAGIC, 0, struct w25q_info)
#define W25Q_SECTOR_ERASE   _IOW(W25Q_IOC_MAGIC, 1, __u32)
#define W25Q_BLOCK_ERASE    _IOW(W25Q_IOC_MAGIC, 2, __u32)
#define W25Q_CHIP_ERASE     _IO(W25Q_IOC_MAGIC, 3)

struct w25q_info {
    char name[16];
    __u32 capacity;
    __u32 page_size;
    __u32 sector_size;
    __u32 block_size;
};

static long w25q_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct w25q_data *flash = filp->private_data;
    struct w25q_info info;
    __u32 offset;
    int ret = 0;

    switch (cmd) {
    case W25Q_GET_INFO:
        strncpy(info.name, flash->info.name, sizeof(info.name));
        info.capacity = flash->info.capacity;
        info.page_size = flash->info.page_size;
        info.sector_size = flash->info.sector_size;
        info.block_size = flash->info.block_size;
        if (copy_to_user((void __user *)arg, &info, sizeof(info)))
            return -EFAULT;
        break;

    case W25Q_SECTOR_ERASE:
        if (copy_from_user(&offset, (void __user *)arg, sizeof(offset)))
            return -EFAULT;
        ret = w25q_sector_erase(flash, offset);
        break;

    case W25Q_BLOCK_ERASE:
        if (copy_from_user(&offset, (void __user *)arg, sizeof(offset)))
            return -EFAULT;
        ret = w25q_block_erase(flash, offset);
        break;

    case W25Q_CHIP_ERASE:
        ret = w25q_chip_erase(flash);
        break;

    default:
        ret = -ENOTTY;
    }

    return ret;
}

static const struct file_operations w25q_fops = {
    .owner          = THIS_MODULE,
    .open           = w25q_open,
    .release        = w25q_release,
    .read           = w25q_read,
    .write          = w25q_write,
    .llseek         = w25q_llseek,
    .unlocked_ioctl = w25q_ioctl,
};

/*
 * SPI探测函数
 */
static int w25q_probe(struct spi_device *spi)
{
    struct w25q_data *flash;
    struct device *dev = &spi->dev;
    u32 jedec_id;
    int i;
    int ret;

    dev_info(dev, "Probing W25Q SPI Flash\n");

    flash = devm_kzalloc(dev, sizeof(*flash), GFP_KERNEL);
    if (!flash)
        return -ENOMEM;

    mutex_init(&flash->lock);
    flash->spi = spi;
    spi_set_drvdata(spi, flash);

    /* 设置SPI模式 */
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret) {
        dev_err(dev, "SPI setup failed: %d\n", ret);
        return ret;
    }

    /* 读取JEDEC ID */
    ret = w25q_read_jedec(flash, &jedec_id);
    if (ret) {
        dev_err(dev, "Failed to read JEDEC ID: %d\n", ret);
        return ret;
    }

    dev_info(dev, "JEDEC ID: 0x%06X\n", jedec_id);

    /* 查找芯片信息 */
    for (i = 0; i < ARRAY_SIZE(flash_info_table); i++) {
        if (flash_info_table[i].jedec_id == jedec_id) {
            flash->info = flash_info_table[i];
            break;
        }
    }

    if (i >= ARRAY_SIZE(flash_info_table)) {
        dev_warn(dev, "Unknown JEDEC ID, assuming 8MB\n");
        strncpy(flash->info.name, "unknown", sizeof(flash->info.name));
        flash->info.capacity = 8 * 1024 * 1024;
        flash->info.page_size = 256;
        flash->info.sector_size = 4096;
        flash->info.block_size = 65536;
    }

    /* 分配缓冲区 */
    flash->buffer = devm_kzalloc(dev, W25Q_PAGE_SIZE, GFP_KERNEL);
    if (!flash->buffer)
        return -ENOMEM;

    /* 分配设备号 */
    ret = alloc_chrdev_region(&flash->dev_num, 0, 1, DRIVER_NAME);
    if (ret)
        return ret;

    /* 创建设备类 */
    flash->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(flash->class)) {
        ret = PTR_ERR(flash->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    flash->device = device_create(flash->class, dev, flash->dev_num, flash,
                                   "w25q_%s", flash->info.name);
    if (IS_ERR(flash->device)) {
        ret = PTR_ERR(flash->device);
        goto destroy_class;
    }

    /* 初始化字符设备 */
    cdev_init(&flash->cdev, &w25q_fops);
    flash->cdev.owner = THIS_MODULE;
    ret = cdev_add(&flash->cdev, flash->dev_num, 1);
    if (ret) {
        dev_err(dev, "Failed to add cdev\n");
        goto destroy_device;
    }

    dev_info(dev, "W25Q %s probed: %u MB\n",
             flash->info.name, flash->info.capacity / (1024 * 1024));
    dev_info(dev, "Device node: /dev/w25q_%s\n", flash->info.name);

    return 0;

destroy_device:
    device_destroy(flash->class, flash->dev_num);
destroy_class:
    class_destroy(flash->class);
unregister_chrdev:
    unregister_chrdev_region(flash->dev_num, 1);
    return ret;
}

static int w25q_remove(struct spi_device *spi)
{
    struct w25q_data *flash = spi_get_drvdata(spi);

    cdev_del(&flash->cdev);
    device_destroy(flash->class, flash->dev_num);
    class_destroy(flash->class);
    unregister_chrdev_region(flash->dev_num, 1);

    dev_info(&spi->dev, "W25Q removed\n");

    return 0;
}

static const struct of_device_id w25q_of_match[] = {
    { .compatible = "winbond,w25q16" },
    { .compatible = "winbond,w25q32" },
    { .compatible = "winbond,w25q64" },
    { .compatible = "winbond,w25q128" },
    { .compatible = "winbond,w25q256" },
    { }
};
MODULE_DEVICE_TABLE(of, w25q_of_match);

static struct spi_driver w25q_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(w25q_of_match),
    },
    .probe = w25q_probe,
    .remove = w25q_remove,
};

module_spi_driver(w25q_driver);
