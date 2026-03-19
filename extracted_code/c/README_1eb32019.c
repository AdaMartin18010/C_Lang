/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\README.md
 * Line: 1165
 * Language: c
 * Block ID: 1eb32019
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 高级中断处理：顶半部 + 底半部
 */

#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/percpu.h>

/* 设备数据结构 */
struct irq_dev {
    int irq;
    void __iomem *base;

    /* 顶半部：快速处理 */
    struct irq_desc *desc;

    /* 底半部选项1: SoftIRQ */
    struct softnet_data *softnet_data;

    /* 底半部选项2: Tasklet（已弃用，推荐使用workqueue） */
    struct tasklet_struct tasklet;

    /* 底半部选项3: Workqueue */
    struct work_struct work;
    struct workqueue_struct *workqueue;

    /* 底半部选项4: Threaded IRQ */
    struct irqaction threaded_action;

    /* 统计 */
    atomic_t interrupts;
    atomic_t bottom_half_count;

    /* 缓冲区 */
    DEFINE_SPINLOCK(buf_lock);
    struct circ_buf rx_ring;
    struct circ_buf tx_ring;
};

/* 顶半部：硬中断处理 */
static irqreturn_t irq_top_half(int irq, void *dev_id)
{
    struct irq_dev *dev = dev_id;
    u32 status;

    /* 读取中断状态 */
    status = readl(dev->base + REG_IRQ_STATUS);

    /* 禁用设备中断（可选，如果使用threaded IRQ则不需要） */
    writel(0, dev->base + REG_IRQ_MASK);

    /* 快速处理：清除中断源 */
    writel(status, dev->base + REG_IRQ_STATUS);

    atomic_inc(&dev->interrupts);

    /* 调度底半部 */
    if (status & IRQ_RX_DATA) {
        /* 方案1: 使用tasklet（已弃用） */
        // tasklet_schedule(&dev->tasklet);

        /* 方案2: 使用workqueue */
        queue_work(dev->workqueue, &dev->work);

        /* 方案3: 唤醒等待线程 */
        // wake_up_interruptible(&dev->wait_queue);
    }

    /* 返回IRQ_WAKE_THREAD将唤醒threaded handler */
    return IRQ_WAKE_THREAD;
}

/* 底半部：threaded IRQ处理 */
static irqreturn_t irq_thread_fn(int irq, void *dev_id)
{
    struct irq_dev *dev = dev_id;
    u32 status;

    /* 读取和处理数据 */
    status = readl(dev->base + REG_IRQ_STATUS);

    while (status & IRQ_RX_DATA) {
        u8 data = readb(dev->base + REG_RX_FIFO);

        /* 写入环形缓冲区 */
        unsigned long flags;
        spin_lock_irqsave(&dev->buf_lock, flags);

        struct circ_buf *ring = &dev->rx_ring;
        if (CIRC_SPACE(ring->head, ring->tail, RING_SIZE) >= 1) {
            ring->buf[ring->head] = data;
            ring->head = (ring->head + 1) & (RING_SIZE - 1);
        } else {
            /* 缓冲区溢出 */
            dev->stats.rx_overruns++;
        }

        spin_unlock_irqrestore(&dev->buf_lock, flags);

        status = readl(dev->base + REG_IRQ_STATUS);
    }

    /* 重新启用中断 */
    writel(IRQ_MASK_ALL, dev->base + REG_IRQ_MASK);

    atomic_inc(&dev->bottom_half_count);

    return IRQ_HANDLED;
}

/* 工作队列底半部 */
static void irq_work_handler(struct work_struct *work)
{
    struct irq_dev *dev = container_of(work, struct irq_dev, work);

    /* 执行耗时操作 */
    process_buffered_data(dev);

    /* 重新启用中断 */
    writel(IRQ_MASK_ALL, dev->base + REG_IRQ_MASK);
}

/* 中断注册 */
static int setup_interrupts(struct irq_dev *dev)
{
    int ret;

    /* 方案1: 传统方式（顶半部 + tasklet/workqueue） */
    #if 0
    ret = request_irq(dev->irq, irq_top_half_only, IRQF_SHARED,
                      "mydevice", dev);
    #endif

    /* 方案2: Threaded IRQ（推荐） */
    ret = request_threaded_irq(dev->irq,
                               irq_top_half,      /* handler */
                               irq_thread_fn,     /* thread_fn */
                               IRQF_ONESHOT | IRQF_SHARED,
                               "mydevice", dev);
    if (ret) {
        dev_err(dev->dev, "Failed to request IRQ %d: %d\n", dev->irq, ret);
        return ret;
    }

    /* 创建专用工作队列 */
    dev->workqueue = alloc_workqueue("mydevice_wq",
                                      WQ_UNBOUND | WQ_HIGHPRI, 0);
    if (!dev->workqueue) {
        free_irq(dev->irq, dev);
        return -ENOMEM;
    }

    INIT_WORK(&dev->work, irq_work_handler);

    return 0;
}

/* 每CPU变量使用 */
static DEFINE_PER_CPU(struct softnet_data, my_softnet_data);

static void percpu_stat_update(void)
{
    /* 获取当前CPU的私有数据 */
    struct softnet_data *sd = this_cpu_ptr(&my_softnet_data);

    /* 无需锁定，每CPU数据天然避免竞争 */
    sd->processed++;

    /* 访问其他CPU的数据 */
    int cpu;
    for_each_online_cpu(cpu) {
        struct softnet_data *other = per_cpu_ptr(&my_softnet_data, cpu);
        /* 需要适当的同步机制 */
        (void)other;
    }
}

/* RCU读端临界区 */
static void rcu_read_example(struct my_data *global_ptr)
{
    struct my_data *data;

    /* 进入RCU读临界区 */
    rcu_read_lock();

    /* 获取指针（可能随时变化） */
    data = rcu_dereference(global_ptr);

    /* 安全使用data（不会被释放，但可能被更新） */
    if (data)
        process_data(data);

    /* 退出RCU读临界区 */
    rcu_read_unlock();
}

/* RCU更新端 */
static void rcu_update_example(struct my_data **global_ptr,
                               struct my_data *new_data)
{
    struct my_data *old_data;

    /* 原子更新指针 */
    old_data = rcu_dereference_protected(*global_ptr,
                                          lockdep_is_held(&my_mutex));
    rcu_assign_pointer(*global_ptr, new_data);

    /* 等待所有读端完成，然后释放旧数据 */
    synchronize_rcu();
    kfree(old_data);
}
