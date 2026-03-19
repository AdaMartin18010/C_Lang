/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 1392
 * Language: c
 * Block ID: eb21e729
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 完整的DMA设备驱动示例
 * 实现一个简单的DMA测试设备
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/completion.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "dma_test"
#define DMA_BUFFER_SIZE (4 * 1024 * 1024)  // 4MB

struct dma_test_device {
    struct cdev cdev;
    struct device *dev;
    dev_t devno;

    struct dma_chan *chan;

    // DMA缓冲区
    void *dma_buf;
    dma_addr_t dma_handle;
    size_t buf_size;

    // 同步
    struct completion done;
    struct mutex lock;

    // 统计
    u64 bytes_transferred;
    u32 transfer_count;
};

static struct dma_test_device *g_dma_dev;

// DMA完成回调
static void dma_test_callback(void *param)
{
    struct dma_test_device *dev = param;

    dev->transfer_count++;
    complete(&dev->done);
}

// 执行DMA内存拷贝
static int dma_test_memcpy(struct dma_test_device *dev,
                            dma_addr_t dst, dma_addr_t src, size_t len)
{
    struct dma_async_tx_descriptor *desc;
    dma_cookie_t cookie;
    int ret;

    reinit_completion(&dev->done);

    // 准备DMA描述符
    desc = dmaengine_prep_dma_memcpy(dev->chan, dst, src, len,
                                      DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
    if (!desc) {
        dev_err(dev->dev, "Failed to prepare DMA\n");
        return -ENOMEM;
    }

    desc->callback = dma_test_callback;
    desc->callback_param = dev;

    // 提交
    cookie = desc->tx_submit(desc);
    if (dma_submit_error(cookie)) {
        dev_err(dev->dev, "Failed to submit DMA\n");
        return -EINVAL;
    }

    // 启动
    dma_async_issue_pending(dev->chan);

    // 等待完成
    ret = wait_for_completion_interruptible_timeout(&dev->done, 10 * HZ);
    if (ret == 0) {
        dev_err(dev->dev, "DMA timeout\n");
        dmaengine_terminate_sync(dev->chan);
        return -ETIMEDOUT;
    } else if (ret < 0) {
        return ret;
    }

    dev->bytes_transferred += len;

    return 0;
}

// 文件操作: ioctl
static long dma_test_ioctl(struct file *filp, unsigned int cmd,
                            unsigned long arg)
{
    struct dma_test_device *dev = g_dma_dev;
    int ret = 0;

    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    switch (cmd) {
    case DMA_TEST_IOC_MEMCPY: {
        struct dma_test_xfer xfer;

        if (copy_from_user(&xfer, (void __user *)arg, sizeof(xfer))) {
            ret = -EFAULT;
            goto out;
        }

        if (xfer.len > dev->buf_size) {
            ret = -EINVAL;
            goto out;
        }

        // 从用户空间复制到DMA缓冲区
        if (copy_from_user(dev->dma_buf, (void __user *)xfer.src, xfer.len)) {
            ret = -EFAULT;
            goto out;
        }

        // 执行DMA拷贝 (缓冲区后半部分)
        ret = dma_test_memcpy(dev,
                               dev->dma_handle + dev->buf_size / 2,
                               dev->dma_handle,
                               xfer.len);
        if (ret)
            goto out;

        // 复制结果回用户空间
        if (copy_to_user((void __user *)xfer.dst,
                          dev->dma_buf + dev->buf_size / 2, xfer.len))
            ret = -EFAULT;

        break;
    }

    case DMA_TEST_IOC_FILL: {
        u8 pattern;

        if (get_user(pattern, (u8 __user *)arg)) {
            ret = -EFAULT;
            goto out;
        }

        // 用pattern填充DMA缓冲区
        memset(dev->dma_buf, pattern, dev->buf_size);

        // 同步给设备
        dma_sync_single_for_device(dev->dev, dev->dma_handle,
                                    dev->buf_size, DMA_TO_DEVICE);
        break;
    }

    case DMA_TEST_IOC_GET_STATS: {
        struct dma_test_stats stats;

        stats.bytes_transferred = dev->bytes_transferred;
        stats.transfer_count = dev->transfer_count;

        if (copy_to_user((void __user *)arg, &stats, sizeof(stats)))
            ret = -EFAULT;
        break;
    }

    default:
        ret = -EINVAL;
    }

out:
    mutex_unlock(&dev->lock);
    return ret;
}

// 文件操作结构
static const struct file_operations dma_test_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dma_test_ioctl,
};

// 模块初始化
static int __init dma_test_init(void)
{
    struct dma_test_device *dev;
    dma_cap_mask_t mask;
    int ret;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    g_dma_dev = dev;
    mutex_init(&dev->lock);
    init_completion(&dev->done);

    // 分配设备号
    ret = alloc_chrdev_region(&dev->devno, 0, 1, DEVICE_NAME);
    if (ret)
        goto err_free;

    // 初始化cdev
    cdev_init(&dev->cdev, &dma_test_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->devno, 1);
    if (ret)
        goto err_chrdev;

    // 创建设备
    dev->dev = device_create(dma_test_class, NULL, dev->devno,
                              NULL, DEVICE_NAME);
    if (IS_ERR(dev->dev)) {
        ret = PTR_ERR(dev->dev);
        goto err_cdev;
    }

    // 申请DMA通道
    dma_cap_zero(mask);
    dma_cap_set(DMA_MEMCPY, mask);

    dev->chan = dma_request_channel(mask, NULL, NULL);
    if (!dev->chan) {
        dev_err(dev->dev, "No DMA channel available\n");
        ret = -ENODEV;
        goto err_device;
    }

    dev_info(dev->dev, "Using DMA channel: %s\n",
             dma_chan_name(dev->chan));

    // 分配DMA缓冲区
    dev->buf_size = DMA_BUFFER_SIZE;
    dev->dma_buf = dmam_alloc_coherent(dev->dev, dev->buf_size,
                                        &dev->dma_handle, GFP_KERNEL);
    if (!dev->dma_buf) {
        ret = -ENOMEM;
        goto err_dma;
    }

    dev_info(dev->dev, "DMA test driver loaded (buffer: %zu bytes @ %pad)\n",
             dev->buf_size, &dev->dma_handle);

    return 0;

err_dma:
    dma_release_channel(dev->chan);
err_device:
    device_destroy(dma_test_class, dev->devno);
err_cdev:
    cdev_del(&dev->cdev);
err_chrdev:
    unregister_chrdev_region(dev->devno, 1);
err_free:
    kfree(dev);
    return ret;
}

static void __exit dma_test_exit(void)
{
    struct dma_test_device *dev = g_dma_dev;

    dma_release_channel(dev->chan);
    device_destroy(dma_test_class, dev->devno);
    cdev_del(&dev->cdev);
    unregister_chrdev_region(dev->devno, 1);
    kfree(dev);

    pr_info("DMA test driver unloaded\n");
}

module_init(dma_test_init);
module_exit(dma_test_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DMA Test Driver");
