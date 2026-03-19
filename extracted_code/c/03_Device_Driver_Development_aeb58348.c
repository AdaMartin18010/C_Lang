/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 5253
 * Language: c
 * Block ID: aeb58348
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: dma_engine_example.c
 * Description: Linux DMA引擎使用示例
 * 展示如何使用DMA Engine API进行内存到设备和设备到内存传输
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/scatterlist.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DMA Driver Developer");
MODULE_DESCRIPTION("DMA Engine Example Driver");

#define DRIVER_NAME     "dmaexample"
#define CLASS_NAME      "dmaexample"
#define BUFFER_SIZE     (256 * 1024)  /* 256KB */

/* DMA传输类型 */
enum dma_transfer_type {
    DMA_MEM_TO_MEM,     /* 内存到内存 */
    DMA_MEM_TO_DEV,     /* 内存到设备 */
    DMA_DEV_TO_MEM,     /* 设备到内存 */
    DMA_SCATTER_GATHER, /* 分散/聚集 */
    DMA_CYCLIC,         /* 循环传输 */
};

/* 传输请求 */
struct dma_request {
    enum dma_transfer_type type;
    dma_addr_t src_addr;
    dma_addr_t dst_addr;
    size_t len;
    struct completion completion;
    dma_cookie_t cookie;
    enum dma_status status;
    u32 residue;
};

/* 设备私有数据 */
struct dmaexample_dev {
    struct device *dev;
    struct dma_chan *chan;

    /* DMA缓冲区 */
    void *src_buf;
    dma_addr_t src_dma_addr;
    void *dst_buf;
    dma_addr_t dst_dma_addr;
    size_t buf_size;

    /* 同步 */
    struct mutex lock;
    struct dma_request req;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/*
 * DMA传输完成回调
 */
static void dma_transfer_complete(void *param)
{
    struct dmaexample_dev *dev = param;

    dev->req.status = dma_async_is_tx_complete(dev->chan, dev->req.cookie,
                                                NULL, &dev->req.residue);

    complete(&dev->req.completion);

    dev_dbg(dev->dev, "DMA transfer completed, status: %d\n", dev->req.status);
}

/*
 * 执行内存到内存DMA传输
 */
static int dma_mem_to_mem_transfer(struct dmaexample_dev *dev, size_t len)
{
    struct dma_async_tx_descriptor *desc;
    dma_cookie_t cookie;
    int ret;

    if (len > dev->buf_size)
        len = dev->buf_size;

    /* 准备DMA传输描述符 */
    desc = dmaengine_prep_dma_memcpy(dev->chan, dev->dst_dma_addr,
                                      dev->src_dma_addr, len,
                                      DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
    if (!desc) {
        dev_err(dev->dev, "Failed to prepare DMA memcpy descriptor\n");
        return -ENOMEM;
    }

    /* 设置完成回调 */
    desc->callback = dma_transfer_complete;
    desc->callback_param = dev;

    /* 提交传输 */
    cookie = desc->tx_submit(desc);
    if (dma_submit_error(cookie)) {
        dev_err(dev->dev, "Failed to submit DMA transfer\n");
        return -EIO;
    }

    dev->req.cookie = cookie;
    init_completion(&dev->req.completion);

    /* 启动传输 */
    dma_async_issue_pending(dev->chan);

    /* 等待完成 */
    ret = wait_for_completion_timeout(&dev->req.completion, 5 * HZ);
    if (ret == 0) {
        dev_err(dev->dev, "DMA transfer timeout\n");
        dmaengine_terminate_sync(dev->chan);
        return -ETIMEDOUT;
    }

    return (len - dev->req.residue);
}

/*
 * 执行分散/聚集DMA传输
 */
static int dma_scatter_gather_transfer(struct dmaexample_dev *dev)
{
    struct scatterlist sg[4];
    struct dma_async_tx_descriptor *desc;
    struct dma_slave_config cfg;
    dma_cookie_t cookie;
    int i;
    int ret;

    /* 配置DMA通道 */
    memset(&cfg, 0, sizeof(cfg));
    cfg.direction = DMA_MEM_TO_MEM;
    cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
    cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;

    ret = dmaengine_slave_config(dev->chan, &cfg);
    if (ret) {
        dev_err(dev->dev, "Failed to configure DMA channel\n");
        return ret;
    }

    /* 构建scatterlist */
    sg_init_table(sg, ARRAY_SIZE(sg));

    /* 将缓冲区分成4段 */
    for (i = 0; i < ARRAY_SIZE(sg); i++) {
        size_t offset = i * (dev->buf_size / 4);
        sg_dma_address(&sg[i]) = dev->src_dma_addr + offset;
        sg_dma_len(&sg[i]) = dev->buf_size / 4;
    }

    /* 准备分散/聚集传输 */
    desc = dmaengine_prep_slave_sg(dev->chan, sg, ARRAY_SIZE(sg),
                                    DMA_MEM_TO_DEV,
                                    DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
    if (!desc) {
        dev_err(dev->dev, "Failed to prepare SG descriptor\n");
        return -ENOMEM;
    }

    desc->callback = dma_transfer_complete;
    desc->callback_param = dev;

    cookie = desc->tx_submit(desc);
    if (dma_submit_error(cookie)) {
        dev_err(dev->dev, "Failed to submit SG transfer\n");
        return -EIO;
    }

    dev->req.cookie = cookie;
    init_completion(&dev->req.completion);

    dma_async_issue_pending(dev->chan);

    ret = wait_for_completion_timeout(&dev->req.completion, 5 * HZ);
    if (ret == 0) {
        dev_err(dev->dev, "SG DMA transfer timeout\n");
        dmaengine_terminate_sync(dev->chan);
        return -ETIMEDOUT;
    }

    return 0;
}

/* 字符设备接口 */
static int dmaexample_open(struct inode *inode, struct file *filp)
{
    struct dmaexample_dev *dev = container_of(inode->i_cdev, struct dmaexample_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static int dmaexample_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t dmaexample_read(struct file *filp, char __user *user_buf,
                                size_t count, loff_t *off)
{
    struct dmaexample_dev *dev = filp->private_data;

    if (*off >= dev->buf_size)
        return 0;

    if (count > dev->buf_size - *off)
        count = dev->buf_size - *off;

    if (copy_to_user(user_buf, dev->dst_buf + *off, count))
        return -EFAULT;

    *off += count;
    return count;
}

static ssize_t dmaexample_write(struct file *filp, const char __user *user_buf,
                                 size_t count, loff_t *off)
{
    struct dmaexample_dev *dev = filp->private_data;

    if (*off >= dev->buf_size)
        return -ENOSPC;

    if (count > dev->buf_size - *off)
        count = dev->buf_size - *off;

    if (copy_from_user(dev->src_buf + *off, user_buf, count))
        return -EFAULT;

    *off += count;
    return count;
}

/* ioctl */
#define DMAEX_IOC_MAGIC     'D'
#define DMAEX_MEMCPY        _IOW(DMAEX_IOC_MAGIC, 0, __u32)
#define DMAEX_MEMCPY_ASYNC  _IOW(DMAEX_IOC_MAGIC, 1, __u32)
#define DMAEX_SG_TRANSFER   _IO(DMAEX_IOC_MAGIC, 2)
#define DMAEX_GET_STATUS    _IOR(DMAEX_IOC_MAGIC, 3, struct dmaex_status)

struct dmaex_status {
    __u32 chan_status;
    __u32 residue;
    __u64 transferred;
};

static long dmaexample_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct dmaexample_dev *dev = filp->private_data;
    __u32 len;
    struct dmaex_status status;
    int ret;

    switch (cmd) {
    case DMAEX_MEMCPY:
        if (copy_from_user(&len, (void __user *)arg, sizeof(len)))
            return -EFAULT;

        mutex_lock(&dev->lock);
        ret = dma_mem_to_mem_transfer(dev, len);
        mutex_unlock(&dev->lock);

        return ret;

    case DMAEX_SG_TRANSFER:
        mutex_lock(&dev->lock);
        ret = dma_scatter_gather_transfer(dev);
        mutex_unlock(&dev->lock);
        return ret;

    case DMAEX_GET_STATUS:
        status.chan_status = dma_async_is_tx_complete(dev->chan, dev->req.cookie,
                                                       NULL, &status.residue);
        status.transferred = dev->buf_size - status.residue;
        if (copy_to_user((void __user *)arg, &status, sizeof(status)))
            return -EFAULT;
        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations dmaexample_fops = {
    .owner          = THIS_MODULE,
    .open           = dmaexample_open,
    .release        = dmaexample_release,
    .read           = dmaexample_read,
    .write          = dmaexample_write,
    .unlocked_ioctl = dmaexample_ioctl,
};

/*
 * 平台探测函数
 */
static int dmaexample_probe(struct platform_device *pdev)
{
    struct dmaexample_dev *dev;
    struct device *device = &pdev->dev;
    dma_cap_mask_t mask;
    int ret;

    dev_info(device, "Probing DMA Example driver\n");

    dev = devm_kzalloc(device, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->dev = device;
    dev->buf_size = BUFFER_SIZE;
    mutex_init(&dev->lock);
    platform_set_drvdata(pdev, dev);

    /* 分配DMA缓冲区 */
    dev->src_buf = dmam_alloc_coherent(device, dev->buf_size,
                                        &dev->src_dma_addr, GFP_KERNEL);
    if (!dev->src_buf)
        return -ENOMEM;

    dev->dst_buf = dmam_alloc_coherent(device, dev->buf_size,
                                        &dev->dst_dma_addr, GFP_KERNEL);
    if (!dev->dst_buf)
        return -ENOMEM;

    dev_info(device, "DMA buffers allocated:\n");
    dev_info(device, "  Source: virt=%p, dma=0x%pad\n",
             dev->src_buf, &dev->src_dma_addr);
    dev_info(device, "  Dest:   virt=%p, dma=0x%pad\n",
             dev->dst_buf, &dev->dst_dma_addr);

    /* 请求DMA通道 */
    dma_cap_zero(mask);
    dma_cap_set(DMA_MEMCPY, mask);

    dev->chan = dma_request_chan_by_mask(&mask);
    if (IS_ERR(dev->chan)) {
        ret = PTR_ERR(dev->chan);
        dev_err(device, "Failed to request DMA channel: %d\n", ret);
        return ret;
    }

    dev_info(device, "Got DMA channel: %s\n", dma_chan_name(dev->chan));

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret)
        goto release_channel;

    /* 创建设备类 */
    dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    dev->device = device_create(dev->class, device, dev->dev_num, dev, DRIVER_NAME);
    if (IS_ERR(dev->device)) {
        ret = PTR_ERR(dev->device);
        goto destroy_class;
    }

    /* 初始化字符设备 */
    cdev_init(&dev->cdev, &dmaexample_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->dev_num, 1);
    if (ret)
        goto destroy_device;

    dev_info(device, "DMA Example driver loaded\n");
    dev_info(device, "Device: /dev/%s\n", DRIVER_NAME);

    return 0;

destroy_device:
    device_destroy(dev->class, dev->dev_num);
destroy_class:
    class_destroy(dev->class);
unregister_chrdev:
    unregister_chrdev_region(dev->dev_num, 1);
release_channel:
    dma_release_channel(dev->chan);
    return ret;
}

static int dmaexample_remove(struct platform_device *pdev)
{
    struct dmaexample_dev *dev = platform_get_drvdata(pdev);

    cdev_del(&dev->cdev);
    device_destroy(dev->class, dev->dev_num);
    class_destroy(dev->class);
    unregister_chrdev_region(dev->dev_num, 1);

    dma_release_channel(dev->chan);

    dev_info(dev->dev, "DMA Example driver removed\n");

    return 0;
}

static const struct of_device_id dmaexample_of_match[] = {
    { .compatible = "dmaexample,dma-memcpy", },
    { }
};
MODULE_DEVICE_TABLE(of, dmaexample_of_match);

static struct platform_driver dmaexample_driver = {
    .probe = dmaexample_probe,
    .remove = dmaexample_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(dmaexample_of_match),
    },
};

module_platform_driver(dmaexample_driver);
