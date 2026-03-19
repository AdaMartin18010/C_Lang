/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 3985
 * Language: c
 * Block ID: 87e35626
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: i2c_eeprom_driver.c
 * Description: I2C EEPROM驱动完整实现 (AT24C系列)
 * 支持24C02/04/08/16/32/64/128/256
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/bsearch.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("I2C Driver Developer");
MODULE_DESCRIPTION("I2C EEPROM Driver for AT24C Series");

#define DRIVER_NAME     "at24c"
#define CLASS_NAME      "at24c"

/* AT24C系列参数 */
struct at24c_chip_data {
    u32 byte_len;           /* 总容量 (字节) */
    u8  flags;
#define AT24C_FLAG_ADDR16   0x01    /* 使用16位地址 */
#define AT24C_FLAG_READONLY 0x02    /* 只读 */
#define AT24C_FLAG_IRUGO    0x04    /* 所有人可读 */
#define AT24C_FLAG_TAKE8ADDR 0x08   /* 占用8个I2C地址 */
};

static const struct at24c_chip_data at24c_chip_data_table[] = {
    /* 型号        容量      标志 */
    [0] = {         128,    0 },                    /* 24C01 */
    [1] = {         256,    0 },                    /* 24C02 */
    [2] = {         512,    AT24C_FLAG_TAKE8ADDR }, /* 24C04 */
    [3] = {        1024,    AT24C_FLAG_TAKE8ADDR }, /* 24C08 */
    [4] = {        2048,    AT24C_FLAG_TAKE8ADDR }, /* 24C16 */
    [5] = {        4096,    AT24C_FLAG_ADDR16 },    /* 24C32 */
    [6] = {        8192,    AT24C_FLAG_ADDR16 },    /* 24C64 */
    [7] = {       16384,    AT24C_FLAG_ADDR16 },    /* 24C128 */
    [8] = {       32768,    AT24C_FLAG_ADDR16 },    /* 24C256 */
    [9] = {       65536,    AT24C_FLAG_ADDR16 },    /* 24C512 */
    [10] = {     131072,    AT24C_FLAG_ADDR16 | AT24C_FLAG_TAKE8ADDR }, /* 24CM01 */
    [11] = {     262144,    AT24C_FLAG_ADDR16 | AT24C_FLAG_TAKE8ADDR }, /* 24CM02 */
};

enum at24c_chip_enum {
    AT24C01 = 0,
    AT24C02,
    AT24C04,
    AT24C08,
    AT24C16,
    AT24C32,
    AT24C64,
    AT24C128,
    AT24C256,
    AT24C512,
    AT24CM01,
    AT24CM02,
};

/* 设备私有数据 */
struct at24c_data {
    struct i2c_client *client;
    struct at24c_chip_data chip;

    /* 地址参数 */
    u16 write_max;          /* 最大写入长度 (页大小) */
    u16 page_size;          /* 页大小 */
    u8  addr_width;         /* 地址宽度 (1或2字节) */

    /* 保护 */
    struct mutex lock;

    /* 写入等待时间 (us) */
    unsigned int write_timeout;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;

    /* 缓冲区 */
    u8 *write_buf;
};

/*
 * 计算实际I2C地址
 * 对于24C04/08/16，使用地址线A0-A2作为页地址
 */
static struct i2c_client *at24c_translate_offset(struct at24c_data *at24,
                                                   loff_t *offset)
{
    unsigned i;

    if (at24->chip.flags & AT24C_FLAG_ADDR16) {
        /* 16位地址: 设备地址不变，offset包含完整地址 */
        i = 0;
    } else if (at24->chip.flags & AT24C_FLAG_TAKE8ADDR) {
        /* 8设备地址: 根据offset高几位选择设备地址 */
        i = *offset >> (at24->chip.byte_len > 8192 ? 13 :
                        at24->chip.byte_len > 4096 ? 12 :
                        at24->chip.byte_len > 2048 ? 11 : 10);
        *offset &= (1 << (at24->chip.byte_len > 8192 ? 13 :
                          at24->chip.byte_len > 4096 ? 12 :
                          at24->chip.byte_len > 2048 ? 11 : 10)) - 1;
    } else {
        i = 0;
    }

    /* 获取对应的I2C client */
    if (i2c_check_functionality(at24->client->adapter, I2C_FUNC_I2C))
        return at24->client;

    /* 对于SMBus适配器，需要选择正确的地址 */
    return at24->client;
}

/*
 * 从EEPROM读取数据
 */
static ssize_t at24c_read(struct at24c_data *at24, char *buf,
                           loff_t off, size_t count)
{
    struct i2c_msg msg[2];
    struct i2c_client *client;
    u8 msgbuf[2];
    int status;
    int i;

    client = at24c_translate_offset(at24, &off);

    if (count > I2C_SMBUS_BLOCK_MAX)
        count = I2C_SMBUS_BLOCK_MAX;

    /* 构建消息 */
    if (at24->addr_width == 2) {
        msgbuf[0] = off >> 8;
        msgbuf[1] = off;
        msg[0].len = 2;
    } else {
        msgbuf[0] = off;
        msg[0].len = 1;
    }

    msg[0].addr = client->addr;
    msg[0].flags = 0;
    msg[0].buf = msgbuf;

    msg[1].addr = client->addr;
    msg[1].flags = I2C_M_RD;
    msg[1].len = count;
    msg[1].buf = buf;

    /* 传输 */
    status = i2c_transfer(client->adapter, msg, 2);
    if (status < 0)
        return status;

    return count;
}

/*
 * 向EEPROM写入数据
 */
static ssize_t at24c_write(struct at24c_data *at24, const char *buf,
                            loff_t off, size_t count)
{
    struct i2c_client *client;
    ssize_t written = 0;
    size_t write_len;
    unsigned next_page;
    int status;

    client = at24c_translate_offset(at24, &off);

    /* EEPROM写入需要考虑页边界 */
    while (count) {
        /* 计算本次可写入的长度 (不能超过页边界) */
        next_page = ((off / at24->page_size) + 1) * at24->page_size;
        write_len = min(count, (size_t)(next_page - off));
        write_len = min(write_len, (size_t)at24->write_max);

        /* 准备写入缓冲区 */
        if (at24->addr_width == 2) {
            at24->write_buf[0] = off >> 8;
            at24->write_buf[1] = off;
            memcpy(&at24->write_buf[2], buf, write_len);
            status = i2c_master_send(client, at24->write_buf, write_len + 2);
        } else {
            at24->write_buf[0] = off;
            memcpy(&at24->write_buf[1], buf, write_len);
            status = i2c_master_send(client, at24->write_buf, write_len + 1);
        }

        if (status < 0)
            return status;

        /* 等待写入完成 */
        msleep(at24->write_timeout / 1000);

        buf += write_len;
        off += write_len;
        count -= write_len;
        written += write_len;
    }

    return written;
}

/*
 * 字符设备 - 打开
 */
static int at24c_open(struct inode *inode, struct file *filp)
{
    struct at24c_data *at24 = container_of(inode->i_cdev, struct at24c_data, cdev);
    filp->private_data = at24;
    return 0;
}

/*
 * 字符设备 - 读取
 */
static ssize_t at24c_file_read(struct file *filp, char __user *user_buf,
                                size_t count, loff_t *off)
{
    struct at24c_data *at24 = filp->private_data;
    char *buf;
    ssize_t ret;

    if (*off >= at24->chip.byte_len)
        return 0;

    if (count > at24->chip.byte_len - *off)
        count = at24->chip.byte_len - *off;

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    mutex_lock(&at24->lock);
    ret = at24c_read(at24, buf, *off, count);
    mutex_unlock(&at24->lock);

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

/*
 * 字符设备 - 写入
 */
static ssize_t at24c_file_write(struct file *filp, const char __user *user_buf,
                                 size_t count, loff_t *off)
{
    struct at24c_data *at24 = filp->private_data;
    char *buf;
    ssize_t ret;

    if (at24->chip.flags & AT24C_FLAG_READONLY)
        return -EPERM;

    if (*off >= at24->chip.byte_len)
        return -ENOSPC;

    if (count > at24->chip.byte_len - *off)
        count = at24->chip.byte_len - *off;

    buf = memdup_user(user_buf, count);
    if (IS_ERR(buf))
        return PTR_ERR(buf);

    mutex_lock(&at24->lock);
    ret = at24c_write(at24, buf, *off, count);
    mutex_unlock(&at24->lock);

    kfree(buf);

    if (ret < 0)
        return ret;

    *off += ret;
    return ret;
}

/*
 * 字符设备 - 定位
 */
static loff_t at24c_llseek(struct file *filp, loff_t off, int whence)
{
    struct at24c_data *at24 = filp->private_data;
    loff_t newpos;

    switch (whence) {
    case SEEK_SET:
        newpos = off;
        break;
    case SEEK_CUR:
        newpos = filp->f_pos + off;
        break;
    case SEEK_END:
        newpos = at24->chip.byte_len + off;
        break;
    default:
        return -EINVAL;
    }

    if (newpos < 0 || newpos > at24->chip.byte_len)
        return -EINVAL;

    filp->f_pos = newpos;
    return newpos;
}

/* ioctl 命令 */
#define AT24C_IOC_MAGIC     'E'
#define AT24C_GET_INFO      _IOR(AT24C_IOC_MAGIC, 0, struct at24c_info)
#define AT24C_ERASE_CHIP    _IO(AT24C_IOC_MAGIC, 1)

struct at24c_info {
    __u32 capacity;
    __u16 page_size;
    __u8  addr_width;
    __u8  flags;
};

static long at24c_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct at24c_data *at24 = filp->private_data;
    struct at24c_info info;
    int ret = 0;

    switch (cmd) {
    case AT24C_GET_INFO:
        info.capacity = at24->chip.byte_len;
        info.page_size = at24->page_size;
        info.addr_width = at24->addr_width;
        info.flags = at24->chip.flags;
        if (copy_to_user((void __user *)arg, &info, sizeof(info)))
            return -EFAULT;
        break;

    case AT24C_ERASE_CHIP:
        if (at24->chip.flags & AT24C_FLAG_READONLY)
            return -EPERM;
        {
            char *buf = kzalloc(at24->page_size, GFP_KERNEL);
            loff_t off;
            if (!buf)
                return -ENOMEM;
            memset(buf, 0xFF, at24->page_size);
            mutex_lock(&at24->lock);
            for (off = 0; off < at24->chip.byte_len; off += at24->page_size) {
                at24c_write(at24, buf, off, at24->page_size);
            }
            mutex_unlock(&at24->lock);
            kfree(buf);
        }
        break;

    default:
        ret = -ENOTTY;
    }

    return ret;
}

static const struct file_operations at24c_fops = {
    .owner          = THIS_MODULE,
    .open           = at24c_open,
    .read           = at24c_file_read,
    .write          = at24c_file_write,
    .llseek         = at24c_llseek,
    .unlocked_ioctl = at24c_ioctl,
};

/*
 * I2C探测函数
 */
static int at24c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct at24c_data *at24;
    struct device *dev = &client->dev;
    int chip_id;
    int ret;

    dev_info(dev, "Probing AT24C EEPROM\n");

    /* 检查I2C功能 */
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C |
                                  I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)) {
        dev_err(dev, "I2C adapter does not support required functionality\n");
        return -ENODEV;
    }

    at24 = devm_kzalloc(dev, sizeof(*at24), GFP_KERNEL);
    if (!at24)
        return -ENOMEM;

    mutex_init(&at24->lock);
    at24->client = client;

    /* 从Device Tree获取芯片类型 */
    if (dev->of_node) {
        const char *chip_name;
        ret = of_property_read_string(dev->of_node, "compatible", &chip_name);
        if (ret == 0) {
            /* 解析芯片型号 */
            if (strstr(chip_name, "24c02"))
                chip_id = AT24C02;
            else if (strstr(chip_name, "24c04"))
                chip_id = AT24C04;
            else if (strstr(chip_name, "24c08"))
                chip_id = AT24C08;
            else if (strstr(chip_name, "24c16"))
                chip_id = AT24C16;
            else if (strstr(chip_name, "24c32"))
                chip_id = AT24C32;
            else if (strstr(chip_name, "24c64"))
                chip_id = AT24C64;
            else if (strstr(chip_name, "24c128"))
                chip_id = AT24C128;
            else if (strstr(chip_name, "24c256"))
                chip_id = AT24C256;
            else if (strstr(chip_name, "24c512"))
                chip_id = AT24C512;
            else
                chip_id = AT24C256;  /* 默认 */
        } else {
            chip_id = AT24C256;
        }
    } else {
        chip_id = id->driver_data;
    }

    at24->chip = at24c_chip_data_table[chip_id];

    /* 设置参数 */
    at24->addr_width = (at24->chip.flags & AT24C_FLAG_ADDR16) ? 2 : 1;
    at24->page_size = (at24->chip.byte_len <= 256) ? 8 :
                      (at24->chip.byte_len <= 2048) ? 16 :
                      (at24->chip.byte_len <= 8192) ? 32 : 64;
    at24->write_max = at24->page_size;
    at24->write_timeout = 5000;  /* 5ms */

    /* 分配写入缓冲区 */
    at24->write_buf = devm_kzalloc(dev, at24->write_max + at24->addr_width, GFP_KERNEL);
    if (!at24->write_buf)
        return -ENOMEM;

    i2c_set_clientdata(client, at24);

    /* 分配设备号 */
    ret = alloc_chrdev_region(&at24->dev_num, 0, 1, DRIVER_NAME);
    if (ret) {
        dev_err(dev, "Failed to allocate device number\n");
        return ret;
    }

    /* 创建设备类 */
    at24->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(at24->class)) {
        ret = PTR_ERR(at24->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    at24->device = device_create(at24->class, dev, at24->dev_num, at24,
                                  "at24c_%02x", client->addr);
    if (IS_ERR(at24->device)) {
        ret = PTR_ERR(at24->device);
        goto destroy_class;
    }

    /* 初始化字符设备 */
    cdev_init(&at24->cdev, &at24c_fops);
    at24->cdev.owner = THIS_MODULE;
    ret = cdev_add(&at24->cdev, at24->dev_num, 1);
    if (ret) {
        dev_err(dev, "Failed to add cdev\n");
        goto destroy_device;
    }

    dev_info(dev, "AT24C EEPROM probed: %u bytes, page=%u, addr_width=%u\n",
             at24->chip.byte_len, at24->page_size, at24->addr_width);
    dev_info(dev, "Device node: /dev/at24c_%02x\n", client->addr);

    return 0;

destroy_device:
    device_destroy(at24->class, at24->dev_num);
destroy_class:
    class_destroy(at24->class);
unregister_chrdev:
    unregister_chrdev_region(at24->dev_num, 1);
    return ret;
}

static int at24c_remove(struct i2c_client *client)
{
    struct at24c_data *at24 = i2c_get_clientdata(client);

    cdev_del(&at24->cdev);
    device_destroy(at24->class, at24->dev_num);
    class_destroy(at24->class);
    unregister_chrdev_region(at24->dev_num, 1);

    dev_info(&client->dev, "AT24C EEPROM removed\n");

    return 0;
}

/* I2C设备ID表 */
static const struct i2c_device_id at24c_ids[] = {
    { "24c01", AT24C01 },
    { "24c02", AT24C02 },
    { "24c04", AT24C04 },
    { "24c08", AT24C08 },
    { "24c16", AT24C16 },
    { "24c32", AT24C32 },
    { "24c64", AT24C64 },
    { "24c128", AT24C128 },
    { "24c256", AT24C256 },
    { "24c512", AT24C512 },
    { "24cm01", AT24CM01 },
    { "24cm02", AT24CM02 },
    { }
};
MODULE_DEVICE_TABLE(i2c, at24c_ids);

/* Device Tree匹配 */
static const struct of_device_id at24c_of_match[] = {
    { .compatible = "atmel,24c01" },
    { .compatible = "atmel,24c02" },
    { .compatible = "atmel,24c04" },
    { .compatible = "atmel,24c08" },
    { .compatible = "atmel,24c16" },
    { .compatible = "atmel,24c32" },
    { .compatible = "atmel,24c64" },
    { .compatible = "atmel,24c128" },
    { .compatible = "atmel,24c256" },
    { .compatible = "atmel,24c512" },
    { .compatible = "atmel,24cm01" },
    { .compatible = "atmel,24cm02" },
    { }
};
MODULE_DEVICE_TABLE(of, at24c_of_match);

static struct i2c_driver at24c_driver = {
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(at24c_of_match),
    },
    .probe = at24c_probe,
    .remove = at24c_remove,
    .id_table = at24c_ids,
};

module_i2c_driver(at24c_driver);
