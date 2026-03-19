/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 5699
 * Language: c
 * Block ID: 58ee1470
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: concurrency_demo.c
 * Description: Linux内核并发控制机制完整演示
 * 包含自旋锁、互斥锁、读写锁、信号量、RCU等
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/semaphore.h>
#include <linux/rcupdate.h>
#include <linux/percpu.h>
#include <linux/atomic.h>
#include <linux/seqlock.h>
#include <linux/completion.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Concurrency Demo Developer");
MODULE_DESCRIPTION("Linux Kernel Concurrency Mechanisms Demo");

#define DRIVER_NAME     "concurrency_demo"
#define MAX_THREADS     8

/* 共享数据结构 */
struct shared_data {
    /* 1. 自旋锁保护的数据 */
    spinlock_t spin_lock;
    unsigned long spin_counter;

    /* 2. 互斥锁保护的数据 */
    struct mutex mutex_lock;
    u64 mutex_counter;

    /* 3. 读写锁保护的数据 */
    struct rw_semaphore rw_sem;
    char rw_buffer[1024];
    size_t rw_len;

    /* 4. 信号量 */
    struct semaphore semaphore;
    int sem_resources;

    /* 5. 顺序锁保护的数据 */
    seqlock_t seq_lock;
    u64 seq_counter;

    /* 6. RCU保护的数据 */
    struct rcu_head rcu;
    struct data_node __rcu *rcu_list;

    /* 7. 原子变量 */
    atomic64_t atomic_counter;
    atomic_t ref_count;
};

/* RCU数据节点 */
struct data_node {
    int id;
    char data[64];
    struct list_head list;
    struct rcu_head rcu;
};

/* 设备私有数据 */
struct concurrency_demo_dev {
    struct shared_data data;
    struct task_struct *threads[MAX_THREADS];
    int num_threads;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/*
 * 1. 自旋锁示例
 * 用于：中断上下文、短时间临界区、多核保护
 * 特点：忙等待，不能睡眠
 */
static void demo_spinlock(struct shared_data *data)
{
    unsigned long flags;

    /* 获取自旋锁并禁用中断 */
    spin_lock_irqsave(&data->spin_lock, flags);

    /* 临界区：访问共享数据 */
    data->spin_counter++;

    /* 释放锁并恢复中断状态 */
    spin_unlock_irqrestore(&data->spin_lock, flags);
}

/* 自旋锁下半部变体 */
static void demo_spinlock_bh(struct shared_data *data)
{
    /* 禁用底半部并获取锁 */
    spin_lock_bh(&data->spin_lock);

    data->spin_counter++;

    spin_unlock_bh(&data->spin_lock);
}

/*
 * 2. 互斥锁示例
 * 用于：进程上下文、可睡眠的临界区
 * 特点：不能在中断上下文使用
 */
static void demo_mutex(struct shared_data *data)
{
    /* 获取互斥锁 */
    mutex_lock(&data->mutex_lock);

    /* 临界区 */
    data->mutex_counter++;

    /* 条件等待示例 */
    /* mutex_unlock(&data->mutex_lock); */
    /* wait_event(wait_queue, condition); */
    /* mutex_lock(&data->mutex_lock); */

    mutex_unlock(&data->mutex_lock);
}

/* 互斥锁中断安全版本 */
static int demo_mutex_interruptible(struct shared_data *data)
{
    int ret;

    /* 可中断地获取锁 */
    ret = mutex_lock_interruptible(&data->mutex_lock);
    if (ret)
        return ret;  /* 被信号中断 */

    data->mutex_counter++;

    mutex_unlock(&data->mutex_lock);
    return 0;
}

/*
 * 3. 读写锁示例
 * 用于：读多写少的场景
 * 特点：读锁可共享，写锁独占
 */
static ssize_t demo_readlock(struct shared_data *data, char *buf, size_t size)
{
    ssize_t len;

    /* 获取读锁 */
    down_read(&data->rw_sem);

    /* 读取共享数据 */
    len = min(size, data->rw_len);
    memcpy(buf, data->rw_buffer, len);

    up_read(&data->rw_sem);

    return len;
}

static ssize_t demo_writelock(struct shared_data *data, const char *buf, size_t size)
{
    /* 获取写锁 */
    down_write(&data->rw_sem);

    /* 写入共享数据 */
    data->rw_len = min(size, sizeof(data->rw_buffer));
    memcpy(data->rw_buffer, buf, data->rw_len);

    up_write(&data->rw_sem);

    return data->rw_len;
}

/*
 * 4. 信号量示例
 * 用于：资源计数、睡眠等待
 */
static int demo_semaphore(struct shared_data *data)
{
    int ret;

    /* 获取信号量 (资源数减1) */
    ret = down_interruptible(&data->semaphore);
    if (ret)
        return ret;

    /* 使用资源 */
    data->sem_resources--;

    /* 释放信号量 (资源数加1) */
    up(&data->semaphore);
    data->sem_resources++;

    return 0;
}

/*
 * 5. 顺序锁示例
 * 用于：读优先、可容忍偶尔不一致的场景
 * 特点：读无锁、写需要同步读者
 */
static u64 demo_seqlock_read(struct shared_data *data)
{
    u64 counter;
    unsigned seq;

    /* 读取序列号 */
    do {
        seq = read_seqbegin(&data->seq_lock);

        /* 读取数据 (可能被写者打断) */
        counter = data->seq_counter;

        /* 检查序列号是否变化 */
    } while (read_seqretry(&data->seq_lock, seq));

    return counter;
}

static void demo_seqlock_write(struct shared_data *data, u64 value)
{
    /* 获取写锁 */
    write_seqlock(&data->seq_lock);

    /* 修改数据 */
    data->seq_counter = value;

    write_sequnlock(&data->seq_lock);
}

/*
 * 6. RCU示例
 * 用于：读多写少、读路径关键的场景
 * 特点：读几乎无开销，写需要同步
 */
static void demo_rcu_read(struct shared_data *data)
{
    struct data_node *node;

    /* 进入RCU读临界区 */
    rcu_read_lock();

    /* 遍历RCU保护的列表 */
    list_for_each_entry_rcu(node, &data->rcu_list->list, list) {
        pr_debug("RCU node %d: %s\n", node->id, node->data);
    }

    /* 退出RCU读临界区 */
    rcu_read_unlock();
}

static void demo_rcu_update(struct shared_data *data, struct data_node *new_node)
{
    struct data_node *old_node;

    /* 替换节点 */
    old_node = rcu_dereference_protected(data->rcu_list,
                                          lockdep_is_held(&data->mutex_lock));

    rcu_assign_pointer(data->rcu_list, new_node);

    /* 安排旧节点延迟释放 */
    if (old_node) {
        list_del_rcu(&old_node->list);
        kfree_rcu(old_node, rcu);
    }
}

/* RCU回调函数 */
static void rcu_free_callback(struct rcu_head *head)
{
    struct data_node *node = container_of(head, struct data_node, rcu);
    kfree(node);
}

/*
 * 7. 原子操作示例
 */
static void demo_atomic(struct shared_data *data)
{
    long long old_val;

    /* 原子递增 */
    atomic64_inc(&data->atomic_counter);

    /* 原子加法 */
    atomic64_add(10, &data->atomic_counter);

    /* 原子读取 */
    old_val = atomic64_read(&data->atomic_counter);

    /* 比较并交换 */
    atomic64_cmpxchg(&data->atomic_counter, old_val, old_val + 1);

    /* 引用计数 */
    atomic_inc(&data->ref_count);
    if (atomic_dec_and_test(&data->ref_count)) {
        /* 引用为0，可以释放 */
    }
}

/* 每CPU变量示例 */
static DEFINE_PER_CPU(unsigned long, percpu_counter);

static void demo_percpu(void)
{
    unsigned long *counter;
    unsigned long sum = 0;
    int cpu;

    /* 获取当前CPU的变量 */
    counter = get_cpu_var(percpu_counter);
    (*counter)++;
    put_cpu_var(percpu_counter);

    /* 汇总所有CPU的值 */
    for_each_online_cpu(cpu) {
        sum += per_cpu(percpu_counter, cpu);
    }
}

/* 工作线程 */
static int worker_thread(void *data)
{
    struct shared_data *shared = data;
    int i;

    for (i = 0; i < 1000 && !kthread_should_stop(); i++) {
        demo_spinlock(shared);
        demo_mutex(shared);
        demo_atomic(shared);

        if (i % 10 == 0)
            msleep(1);
    }

    return 0;
}

/* 字符设备接口 */
static int concurrency_demo_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int concurrency_demo_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t concurrency_demo_read(struct file *filp, char __user *user_buf,
                                      size_t count, loff_t *off)
{
    struct concurrency_demo_dev *dev =
        container_of(filp->f_inode->i_cdev, struct concurrency_demo_dev, cdev);
    char buf[512];
    int len;

    len = snprintf(buf, sizeof(buf),
                   "=== Concurrency Statistics ===\n"
                   "Spinlock counter: %lu\n"
                   "Mutex counter: %llu\n"
                   "Atomic counter: %lld\n"
                   "Reference count: %d\n"
                   "Seqlock counter: %llu\n"
                   "RW buffer len: %zu\n",
                   dev->data.spin_counter,
                   dev->data.mutex_counter,
                   atomic64_read(&dev->data.atomic_counter),
                   atomic_read(&dev->data.ref_count),
                   dev->data.seq_counter,
                   dev->data.rw_len);

    if (*off >= len)
        return 0;

    if (count > len - *off)
        count = len - *off;

    if (copy_to_user(user_buf, buf + *off, count))
        return -EFAULT;

    *off += count;
    return count;
}

/* ioctl 命令 */
#define CONC_IOC_MAGIC      'C'
#define CONC_RUN_DEMO       _IO(CONC_IOC_MAGIC, 0)
#define CONC_GET_STATS      _IOR(CONC_IOC_MAGIC, 1, struct conc_stats)
#define CONC_RW_READ        _IOR(CONC_IOC_MAGIC, 2, struct rw_request)
#define CONC_RW_WRITE       _IOW(CONC_IOC_MAGIC, 3, struct rw_request)
#define CONC_START_THREADS  _IOW(CONC_IOC_MAGIC, 4, __u32)
#define CONC_STOP_THREADS   _IO(CONC_IOC_MAGIC, 5)

struct conc_stats {
    __u64 spin_counter;
    __u64 mutex_counter;
    __u64 atomic_counter;
    __u64 seq_counter;
};

struct rw_request {
    char data[1024];
    __u32 len;
};

static long concurrency_demo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct concurrency_demo_dev *dev =
        container_of(filp->f_inode->i_cdev, struct concurrency_demo_dev, cdev);
    struct conc_stats stats;
    struct rw_request rw_req;
    __u32 num_threads;
    int ret = 0;
    int i;

    switch (cmd) {
    case CONC_RUN_DEMO:
        /* 运行所有演示 */
        demo_spinlock(&dev->data);
        demo_mutex(&dev->data);
        demo_atomic(&dev->data);
        demo_seqlock_write(&dev->data, dev->data.seq_counter + 1);
        break;

    case CONC_GET_STATS:
        stats.spin_counter = dev->data.spin_counter;
        stats.mutex_counter = dev->data.mutex_counter;
        stats.atomic_counter = atomic64_read(&dev->data.atomic_counter);
        stats.seq_counter = dev->data.seq_counter;
        if (copy_to_user((void __user *)arg, &stats, sizeof(stats)))
            return -EFAULT;
        break;

    case CONC_RW_READ:
        rw_req.len = demo_readlock(&dev->data, rw_req.data, sizeof(rw_req.data));
        if (copy_to_user((void __user *)arg, &rw_req, sizeof(rw_req)))
            return -EFAULT;
        break;

    case CONC_RW_WRITE:
        if (copy_from_user(&rw_req, (void __user *)arg, sizeof(rw_req)))
            return -EFAULT;
        demo_writelock(&dev->data, rw_req.data, rw_req.len);
        break;

    case CONC_START_THREADS:
        if (copy_from_user(&num_threads, (void __user *)arg, sizeof(num_threads)))
            return -EFAULT;
        if (num_threads > MAX_THREADS)
            num_threads = MAX_THREADS;

        for (i = 0; i < num_threads; i++) {
            dev->threads[i] = kthread_run(worker_thread, &dev->data,
                                           "concurrency_worker_%d", i);
            if (IS_ERR(dev->threads[i])) {
                ret = PTR_ERR(dev->threads[i]);
                dev->threads[i] = NULL;
                break;
            }
        }
        dev->num_threads = i;
        break;

    case CONC_STOP_THREADS:
        for (i = 0; i < dev->num_threads; i++) {
            if (dev->threads[i]) {
                kthread_stop(dev->threads[i]);
                dev->threads[i] = NULL;
            }
        }
        dev->num_threads = 0;
        break;

    default:
        ret = -ENOTTY;
    }

    return ret;
}

static const struct file_operations concurrency_demo_fops = {
    .owner          = THIS_MODULE,
    .open           = concurrency_demo_open,
    .release        = concurrency_demo_release,
    .read           = concurrency_demo_read,
    .unlocked_ioctl = concurrency_demo_ioctl,
};

static int __init concurrency_demo_init(void)
{
    struct concurrency_demo_dev *dev;
    int ret;

    pr_info("Concurrency Demo initializing\n");

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    /* 初始化各种锁 */
    spin_lock_init(&dev->data.spin_lock);
    mutex_init(&dev->data.mutex_lock);
    init_rwsem(&dev->data.rw_sem);
    sema_init(&dev->data.semaphore, 4);  /* 初始4个资源 */
    seqlock_init(&dev->data.seq_lock);
    atomic64_set(&dev->data.atomic_counter, 0);
    atomic_set(&dev->data.ref_count, 1);

    /* 初始化RCU列表 */
    INIT_LIST_HEAD(&dev->data.rcu_list);

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret)
        goto free_dev;

    /* 创建设备类 */
    dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    dev->device = device_create(dev->class, NULL, dev->dev_num, dev, DRIVER_NAME);
    if (IS_ERR(dev->device)) {
        ret = PTR_ERR(dev->device);
        goto destroy_class;
    }

    /* 初始化字符设备 */
    cdev_init(&dev->cdev, &concurrency_demo_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->dev_num, 1);
    if (ret)
        goto destroy_device;

    pr_info("Concurrency Demo loaded: /dev/%s\n", DRIVER_NAME);

    return 0;

destroy_device:
    device_destroy(dev->class, dev->dev_num);
destroy_class:
    class_destroy(dev->class);
unregister_chrdev:
    unregister_chrdev_region(dev->dev_num, 1);
free_dev:
    kfree(dev);
    return ret;
}

static void __exit concurrency_demo_exit(void)
{
    /* 这里应该遍历所有实例并清理，简化处理 */
    pr_info("Concurrency Demo unloaded\n");
}

module_init(concurrency_demo_init);
module_exit(concurrency_demo_exit);
