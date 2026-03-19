/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 1486
 * Language: c
 * Block ID: 7e22a1c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: platform_driver.c
 * Description: 平台设备驱动完整实现
 * 支持Device Tree和平台数据两种配置方式
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>           /* Device Tree */
#include <linux/of_device.h>
#include <linux/gpio/consumer.h> /* GPIO描述符 */
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/reset.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Platform Driver Developer");
MODULE_DESCRIPTION("Platform Device Driver with Device Tree Support");

#define DRIVER_NAME     "myplatform"
#define DEVICE_NAME     "myplatform"
#define CLASS_NAME      "myplatform_class"
#define MAX_DEVICES     8
#define BUFFER_SIZE     256

/* 平台特定配置数据 */
struct myplatform_config {
    u32 base_addr;
    u32 reg_size;
    u32 irq_num;
    u32 clock_freq;
    bool use_dma;
    u8  version;
};

/* 设备私有数据 */
struct myplatform_device {
    struct platform_device *pdev;
    struct myplatform_config config;

    /* 资源 */
    void __iomem *regs;
    int irq;
    struct clk *clk;
    struct reset_control *rst;
    struct gpio_desc *gpio_reset;
    struct gpio_desc *gpio_irq;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;

    /* 数据缓冲区 */
    u8 *buffer;
    size_t buf_len;
    struct mutex lock;

    /* 中断统计 */
    u32 irq_count;
    bool irq_enabled;
};

/* 平台ID表 (非Device Tree匹配) */
static const struct platform_device_id myplatform_id_table[] = {
    { "myplatform-v1", (kernel_ulong_t)&(const struct myplatform_config){
        .base_addr = 0x10000000,
        .reg_size = 0x1000,
        .irq_num = 32,
        .clock_freq = 100000000,
        .use_dma = false,
        .version = 1,
    }},
    { "myplatform-v2", (kernel_ulong_t)&(const struct myplatform_config){
        .base_addr = 0x20000000,
        .reg_size = 0x2000,
        .irq_num = 48,
        .clock_freq = 200000000,
        .use_dma = true,
        .version = 2,
    }},
    { /* 结束标志 */ }
};
MODULE_DEVICE_TABLE(platform, myplatform_id_table);

/* Device Tree匹配表 */
static const struct of_device_id myplatform_of_match[] = {
    {
        .compatible = "myvendor,myplatform-v1",
        .data = &myplatform_id_table[0],
    },
    {
        .compatible = "myvendor,myplatform-v2",
        .data = &myplatform_id_table[1],
    },
    { /* 结束标志 */ }
};
MODULE_DEVICE_TABLE(of, myplatform_of_match);

/* 寄存器偏移定义 */
#define REG_CTRL        0x00
#define REG_STATUS      0x04
#define REG_DATA        0x08
#define REG_IRQ_MASK    0x0C
#define REG_IRQ_STAT    0x10
#define REG_VERSION     0x14

/* 寄存器访问辅助函数 */
static inline u32 myplatform_read_reg(struct myplatform_device *dev, u32 offset)
{
    return readl(dev->regs + offset);
}

static inline void myplatform_write_reg(struct myplatform_device *dev,
                                         u32 offset, u32 value)
{
    writel(value, dev->regs + offset);
}

/*
 * 字符设备 - 打开
 */
static int myplatform_open(struct inode *inode, struct file *filp)
{
    struct myplatform_device *dev;

    dev = container_of(inode->i_cdev, struct myplatform_device, cdev);
    filp->private_data = dev;

    printk(KERN_INFO "myplatform: device opened\n");
    return 0;
}

/*
 * 字符设备 - 关闭
 */
static int myplatform_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "myplatform: device released\n");
    return 0;
}

/*
 * 字符设备 - 读取
 */
static ssize_t myplatform_read(struct file *filp, char __user *user_buf,
                                size_t count, loff_t *f_pos)
{
    struct myplatform_device *dev = filp->private_data;
    ssize_t bytes_read = 0;

    mutex_lock(&dev->lock);

    if (*f_pos >= dev->buf_len) {
        mutex_unlock(&dev->lock);
        return 0;  /* EOF */
    }

    count = min(count, (size_t)(dev->buf_len - *f_pos));

    if (copy_to_user(user_buf, dev->buffer + *f_pos, count)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;
    }

    *f_pos += count;
    bytes_read = count;

    mutex_unlock(&dev->lock);

    return bytes_read;
}

/*
 * 字符设备 - 写入
 */
static ssize_t myplatform_write(struct file *filp, const char __user *user_buf,
                                 size_t count, loff_t *f_pos)
{
    struct myplatform_device *dev = filp->private_data;

    mutex_lock(&dev->lock);

    count = min(count, (size_t)BUFFER_SIZE);

    if (copy_from_user(dev->buffer, user_buf, count)) {
        mutex_unlock(&dev->lock);
        return -EFAULT;
    }

    dev->buf_len = count;
    *f_pos = count;

    /* 写入硬件 (示例) */
    myplatform_write_reg(dev, REG_DATA, *(u32 *)dev->buffer);

    mutex_unlock(&dev->lock);

    return count;
}

static const struct file_operations myplatform_fops = {
    .owner   = THIS_MODULE,
    .open    = myplatform_open,
    .release = myplatform_release,
    .read    = myplatform_read,
    .write   = myplatform_write,
};

/*
 * 中断处理函数
 */
static irqreturn_t myplatform_irq_handler(int irq, void *dev_id)
{
    struct myplatform_device *dev = dev_id;
    u32 status;

    /* 读取中断状态 */
    status = myplatform_read_reg(dev, REG_IRQ_STAT);

    if (status & 0x01) {
        /* 处理中断 */
        dev->irq_count++;

        /* 清除中断 */
        myplatform_write_reg(dev, REG_IRQ_STAT, status);

        printk(KERN_DEBUG "myplatform: IRQ handled (count=%u)\n", dev->irq_count);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

/*
 * 从Device Tree解析配置
 */
static int myplatform_parse_dt(struct platform_device *pdev,
                                struct myplatform_config *config)
{
    struct device_node *np = pdev->dev.of_node;
    int ret;

    if (!np)
        return -ENODEV;

    /* 读取属性 */
    ret = of_property_read_u32(np, "clock-frequency", &config->clock_freq);
    if (ret) {
        dev_warn(&pdev->dev, "clock-frequency not specified, using default\n");
        config->clock_freq = 100000000;
    }

    config->use_dma = of_property_read_bool(np, "use-dma");

    ret = of_property_read_u8(np, "version", &config->version);
    if (ret)
        config->version = 1;

    dev_info(&pdev->dev, "DT config: clk=%u, dma=%s, ver=%u\n",
             config->clock_freq, config->use_dma ? "yes" : "no", config->version);

    return 0;
}

/*
 * 平台设备探测函数
 */
static int myplatform_probe(struct platform_device *pdev)
{
    struct myplatform_device *dev;
    struct resource *res;
    const struct myplatform_config *config = NULL;
    const struct of_device_id *of_id;
    int ret;

    dev_info(&pdev->dev, "Probing myplatform device\n");

    /* 分配设备结构体 */
    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->pdev = pdev;
    platform_set_drvdata(pdev, dev);

    /* 获取配置 */
    of_id = of_match_device(myplatform_of_match, &pdev->dev);
    if (of_id) {
        /* Device Tree匹配 */
        config = (const struct myplatform_config *)of_id->data;
        myplatform_parse_dt(pdev, &dev->config);
    } else if (pdev->id_entry) {
        /* 平台ID表匹配 */
        config = (const struct myplatform_config *)pdev->id_entry->driver_data;
    }

    if (!config) {
        dev_err(&pdev->dev, "No configuration found\n");
        return -EINVAL;
    }

    memcpy(&dev->config, config, sizeof(*config));

    /* 获取内存资源 */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_err(&pdev->dev, "Failed to get memory resource\n");
        return -ENODEV;
    }

    dev->config.base_addr = res->start;
    dev->config.reg_size = resource_size(res);

    /* 映射寄存器 */
    dev->regs = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(dev->regs))
        return PTR_ERR(dev->regs);

    dev_info(&pdev->dev, "Mapped registers at 0x%08X (size=0x%X)\n",
             dev->config.base_addr, dev->config.reg_size);

    /* 获取中断 */
    dev->irq = platform_get_irq(pdev, 0);
    if (dev->irq < 0) {
        dev_err(&pdev->dev, "Failed to get IRQ\n");
        return dev->irq;
    }

    dev->config.irq_num = dev->irq;

    /* 请求中断 */
    ret = devm_request_irq(&pdev->dev, dev->irq, myplatform_irq_handler,
                           IRQF_SHARED, DRIVER_NAME, dev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to request IRQ %d\n", dev->irq);
        return ret;
    }

    dev_info(&pdev->dev, "IRQ %d registered\n", dev->irq);

    /* 获取时钟 */
    dev->clk = devm_clk_get(&pdev->dev, NULL);
    if (IS_ERR(dev->clk)) {
        ret = PTR_ERR(dev->clk);
        if (ret != -ENOENT) {
            dev_err(&pdev->dev, "Failed to get clock\n");
            return ret;
        }
        dev->clk = NULL;
    } else {
        ret = clk_prepare_enable(dev->clk);
        if (ret) {
            dev_err(&pdev->dev, "Failed to enable clock\n");
            return ret;
        }
    }

    /* 获取复位控制 */
    dev->rst = devm_reset_control_get_optional(&pdev->dev, NULL);
    if (IS_ERR(dev->rst)) {
        ret = PTR_ERR(dev->rst);
        if (ret != -EPROBE_DEFER)
            dev_warn(&pdev->dev, "Failed to get reset control: %d\n", ret);
        return ret;
    }

    /* 释放复位 */
    if (dev->rst) {
        reset_control_deassert(dev->rst);
    }

    /* 获取GPIO */
    dev->gpio_reset = devm_gpiod_get_optional(&pdev->dev, "reset", GPIOD_OUT_LOW);
    if (IS_ERR(dev->gpio_reset)) {
        ret = PTR_ERR(dev->gpio_reset);
        if (ret != -EPROBE_DEFER)
            dev_warn(&pdev->dev, "Failed to get reset GPIO\n");
        return ret;
    }

    /* 硬件复位 */
    if (dev->gpio_reset) {
        gpiod_set_value(dev->gpio_reset, 1);
        msleep(10);
        gpiod_set_value(dev->gpio_reset, 0);
        dev_info(&pdev->dev, "Hardware reset done\n");
    }

    /* 初始化硬件 */
    myplatform_write_reg(dev, REG_CTRL, 0x00);
    myplatform_write_reg(dev, REG_IRQ_MASK, 0x01);

    u32 version = myplatform_read_reg(dev, REG_VERSION);
    dev_info(&pdev->dev, "Hardware version: 0x%08X\n", version);

    /* 分配缓冲区 */
    dev->buffer = devm_kzalloc(&pdev->dev, BUFFER_SIZE, GFP_KERNEL);
    if (!dev->buffer)
        return -ENOMEM;

    mutex_init(&dev->lock);

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev->dev_num, 0, 1, DEVICE_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to allocate device number\n");
        goto disable_clk;
    }

    /* 创建设备类 */
    dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        goto unregister_chrdev;
    }

    /* 创建设备节点 */
    device_create(dev->class, &pdev->dev, dev->dev_num, NULL, "%s", DEVICE_NAME);

    /* 初始化字符设备 */
    cdev_init(&dev->cdev, &myplatform_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->dev_num, 1);
    if (ret) {
        dev_err(&pdev->dev, "Failed to add cdev\n");
        goto destroy_device;
    }

    dev_info(&pdev->dev, "Device created: /dev/%s (major=%d)\n",
             DEVICE_NAME, MAJOR(dev->dev_num));

    return 0;

destroy_device:
    device_destroy(dev->class, dev->dev_num);
    class_destroy(dev->class);
unregister_chrdev:
    unregister_chrdev_region(dev->dev_num, 1);
disable_clk:
    if (dev->clk)
        clk_disable_unprepare(dev->clk);
    return ret;
}

/*
 * 平台设备移除函数
 */
static int myplatform_remove(struct platform_device *pdev)
{
    struct myplatform_device *dev = platform_get_drvdata(pdev);

    dev_info(&pdev->dev, "Removing myplatform device\n");

    /* 删除字符设备 */
    cdev_del(&dev->cdev);

    /* 销毁设备节点 */
    device_destroy(dev->class, dev->dev_num);
    class_destroy(dev->class);

    /* 释放设备号 */
    unregister_chrdev_region(dev->dev_num, 1);

    /* 禁用中断 */
    myplatform_write_reg(dev, REG_IRQ_MASK, 0x00);

    /* 禁用时钟 */
    if (dev->clk)
        clk_disable_unprepare(dev->clk);

    /* 断言复位 */
    if (dev->rst)
        reset_control_assert(dev->rst);

    return 0;
}

/*
 * 电源管理 - 挂起
 */
static int __maybe_unused myplatform_suspend(struct device *dev)
{
    struct myplatform_device *pdev = dev_get_drvdata(dev);

    /* 保存状态 */
    myplatform_write_reg(pdev, REG_IRQ_MASK, 0x00);

    if (pdev->clk)
        clk_disable_unprepare(pdev->clk);

    return 0;
}

/*
 * 电源管理 - 恢复
 */
static int __maybe_unused myplatform_resume(struct device *dev)
{
    struct myplatform_device *pdev = dev_get_drvdata(dev);

    if (pdev->clk)
        clk_prepare_enable(pdev->clk);

    /* 恢复状态 */
    myplatform_write_reg(pdev, REG_IRQ_MASK, 0x01);

    return 0;
}

static const struct dev_pm_ops myplatform_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(myplatform_suspend, myplatform_resume)
};

/* 平台驱动结构体 */
static struct platform_driver myplatform_driver = {
    .probe  = myplatform_probe,
    .remove = myplatform_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(myplatform_of_match),
        .pm = &myplatform_pm_ops,
    },
    .id_table = myplatform_id_table,
};

module_platform_driver(myplatform_driver);
