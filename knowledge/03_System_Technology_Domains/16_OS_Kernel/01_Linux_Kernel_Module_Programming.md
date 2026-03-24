# Linux内核模块编程深度指南

> **层级定位**: 03 System Technology Domains / 16 OS Kernel
> **参考课程**: MIT 6.828, Stanford CS140, Berkeley CS162
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 目录

- [Linux内核模块编程深度指南](#linux内核模块编程深度指南)
  - [目录](#目录)
  - [1. 内核编程基础](#1-内核编程基础)
    - [1.1 用户空间vs内核空间](#11-用户空间vs内核空间)
    - [1.2 最小内核模块](#12-最小内核模块)
    - [1.3 内核调试输出](#13-内核调试输出)
  - [2. 内核内存管理](#2-内核内存管理)
    - [2.1 内核内存分配器](#21-内核内存分配器)
    - [2.2 内存池](#22-内存池)
    - [2.3 页面分配](#23-页面分配)
  - [3. 并发与同步](#3-并发与同步)
    - [3.1 内核同步原语](#31-内核同步原语)
    - [3.2 工作队列与延迟工作](#32-工作队列与延迟工作)
  - [4. 字符设备驱动](#4-字符设备驱动)
    - [4.1 基础框架](#41-基础框架)
  - [5. 内核时间与时钟](#5-内核时间与时钟)
    - [5.1 时间API](#51-时间api)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 内核编程基础

### 1.1 用户空间vs内核空间

```
┌─────────────────────────────────────────────────────────────┐
│                         用户空间                             │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                     │
│  │ 应用A   │  │ 应用B   │  │ Shell   │                     │
│  └────┬────┘  └────┬────┘  └────┬────┘                     │
│       └─────────────┴─────────────┘                         │
│              ↓ 系统调用 (int 0x80 / syscall)                │
├─────────────────────────────────────────────────────────────┤
│                         内核空间                             │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              系统调用处理层                          │    │
│  └─────────────────────────────────────────────────────┘    │
│              ↓                                               │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐        │
│  │ VFS层   │  │ 进程调度 │  │ 内存管理 │  │ 网络栈  │        │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘        │
│       └─────────────┴─────────────┴─────────────┘            │
│              ↓                                               │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                     │
│  │ 设备驱动 │  │ 文件系统 │  │ 网络驱动 │                     │
│  └────┬────┘  └────┬────┘  └────┬────┘                     │
│       └─────────────┴─────────────┘                         │
│              ↓                                               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              硬件抽象层 (HAL/BIOS)                   │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘

关键区别：
┌─────────────┬─────────────┬─────────────┐
│   特性      │   用户空间   │   内核空间   │
├─────────────┼─────────────┼─────────────┤
│ 地址空间    │ 独立虚拟地址 │ 共享内核空间 │
│ 错误影响    │ 仅当前进程   │ 系统崩溃     │
│ 内存访问    │ 受限         │ 完全访问     │
│ 调度        │ 可抢占       │ 原子/可睡眠  │
│ 浮点运算    │ 支持         │ 需手动保存   │
│ 标准库      │ glibc        │ 内核API      │
└─────────────┴─────────────┴─────────────┘
```

### 1.2 最小内核模块

```c
// hello.c - 最小Linux内核模块
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kernel Developer");
MODULE_DESCRIPTION("A simple Linux kernel module");
MODULE_VERSION("1.0");

// 模块加载时执行
static int __init hello_init(void)
{
    printk(KERN_INFO "[HELLO] Module loaded\n");
    printk(KERN_INFO "[HELLO] Hello, Kernel World!\n");
    return 0;  // 0 = 成功
}

// 模块卸载时执行
static void __exit hello_exit(void)
{
    printk(KERN_INFO "[HELLO] Module unloaded\n");
}

// 注册入口/出口
module_init(hello_init);
module_exit(hello_exit);
```

```makefile
# Makefile
obj-m += hello.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
 $(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
 $(MAKE) -C $(KDIR) M=$(PWD) clean

load:
 sudo insmod hello.ko
 sudo dmesg | tail -5

unload:
 sudo rmmod hello
 sudo dmesg | tail -5
```

### 1.3 内核调试输出

```c
// printk日志级别
printk(KERN_EMERG   "System is unusable\n");        // 系统崩溃
printk(KERN_ALERT   "Action must be taken\n");      // 立即行动
printk(KERN_CRIT    "Critical conditions\n");        // 临界条件
printk(KERN_ERR     "Error conditions\n");          // 错误
printk(KERN_WARNING "Warning conditions\n");        // 警告
printk(KERN_NOTICE  "Normal but significant\n");     // 注意
printk(KERN_INFO    "Informational\n");              // 信息
printk(KERN_DEBUG   "Debug-level messages\n");      // 调试

// 查看内核日志
// dmesg -w                    # 实时查看
// dmesg | grep "HELLO"        # 过滤
// dmesg -C                    # 清空
// cat /proc/kmsg              # 原始内核消息
// journalctl -k               # systemd系统

// 动态调试（pr_debug）
#define DEBUG  // 必须在include之前定义
#include <linux/kernel.h>

// 或使用动态调试
// echo 'file hello.c +p' > /sys/kernel/debug/dynamic_debug/control
```

---

## 2. 内核内存管理

### 2.1 内核内存分配器

```c
#include <linux/slab.h>
#include <linux/vmalloc.h>

// ========== kmalloc家族 ==========
// 物理连续，适合DMA，上限128KB（x86）

void *ptr;

// GFP标志：分配行为
// GFP_KERNEL    - 标准，可睡眠，用于进程上下文
// GFP_ATOMIC    - 原子，不睡眠，用于中断上下文
// GFP_DMA       - 适合DMA的内存
// GFP_USER      - 为用户空间分配
// GFP_HIGHUSER  - 高端内存（x86 PAE）

// 基本分配
ptr = kmalloc(size, GFP_KERNEL);
if (!ptr)
    return -ENOMEM;  // 标准错误码

// 清零分配
ptr = kzalloc(size, GFP_KERNEL);

// 分配数组
ptr = kcalloc(n, size, GFP_KERNEL);

// 重新分配
ptr = krealloc(ptr, new_size, GFP_KERNEL);

// 释放
kfree(ptr);

// ========== kmem_cache（对象缓存）==========
// 适用于频繁分配/释放固定大小对象

struct kmem_cache *my_cache;

// 创建缓存
my_cache = kmem_cache_create(
    "my_struct_cache",      // 名称
    sizeof(struct my_struct), // 对象大小
    0,                        // 对齐（0=默认）
    SLAB_HWCACHE_ALIGN,       // 标志
    NULL                      // 构造函数（现代内核已废弃）
);

// 分配/释放
struct my_struct *obj = kmem_cache_alloc(my_cache, GFP_KERNEL);
kmem_cache_free(my_cache, obj);

// 销毁缓存
kmem_cache_destroy(my_cache);

// ========== vmalloc ==========
// 虚拟连续，物理不连续，适合大内存（>1MB）

void *vptr = vmalloc(size);  // 分配
vfree(vptr);                  // 释放

// 页表操作开销大，仅用于大块内存
```

### 2.2 内存池

```c
#include <linux/mempool.h>

// 内存池保证分配不会失败（预分配）
mempool_t *pool;

// 创建池：min_nr个预分配对象
pool = mempool_create(
    min_nr,                    // 最小保证数量
    mempool_alloc_slab,        // 分配函数
    mempool_free_slab,         // 释放函数
    my_cache                   // 私有数据（缓存指针）
);

// 分配（保证成功，除非池耗尽且无法增长）
void *obj = mempool_alloc(pool, GFP_KERNEL);

// 释放回池
mempool_free(obj, pool);

// 销毁池
mempool_destroy(pool);
```

### 2.3 页面分配

```c
#include <linux/gfp.h>

// ========== 页分配 ==========
struct page *page;
void *addr;

// 分配单个页
page = alloc_page(GFP_KERNEL);        // 返回struct page*
addr = page_address(page);             // 转换为虚拟地址
__free_page(page);                     // 释放

// 分配2^order个连续页
page = alloc_pages(GFP_KERNEL, order); // order=0分配1页
__free_pages(page, order);

// 快速分配/释放
unsigned long pfn = __get_free_page(GFP_KERNEL);
free_page(pfn);

// ========== 高端内存（已过时，现代64位系统一般不需要）==========
#ifdef CONFIG_HIGHMEM
void *kmap(struct page *page);        // 映射到内核空间
void kunmap(struct page *page);
#endif
```

---

## 3. 并发与同步

### 3.1 内核同步原语

```c
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/rcupdate.h>

// ========== 自旋锁 ==========
// 适用：短临界区，中断上下文

define_spinlock(my_lock);
spin_lock(&my_lock);
/* 临界区 - 不可睡眠！ */
spin_unlock(&my_lock);

// 中断安全版本
unsigned long flags;
spin_lock_irqsave(&my_lock, flags);   // 保存中断状态并禁用
/* 临界区 */
spin_unlock_irqrestore(&my_lock, flags); // 恢复中断状态

// 仅禁用软中断
spin_lock_bh(&my_lock);
spin_unlock_bh(&my_lock);

// ========== 互斥锁 ==========
// 适用：长临界区，进程上下文，可睡眠

define_MUTEX(my_mutex);

mutex_lock(&my_mutex);        // 可能睡眠
/* 临界区 - 可睡眠 */
mutex_unlock(&my_mutex);

// 非阻塞尝试
if (mutex_trylock(&my_mutex)) {
    /* 临界区 */
    mutex_unlock(&my_mutex);
}

// ========== 读写信号量 ==========
// 适用：读多写少场景

define_RWSEM(my_rwsem);

down_read(&my_rwsem);         // 读锁定
/* 只读访问 - 可多读者并发 */
up_read(&my_rwsem);

down_write(&my_rwsem);        // 写锁定
/* 独占写访问 */
up_write(&my_rwsem);

// ========== RCU (Read-Copy-Update) ==========
// 适用：读极多写极少，无锁读

struct my_data {
    int value;
    struct rcu_head rcu;      // RCU释放用
};

struct my_data *global_ptr;

define_SPINLOCK(update_lock);

// 读端（无锁！）
rcu_read_lock();
struct my_data *ptr = rcu_dereference(global_ptr);
// 使用ptr（只读！）
printk("Value: %d\n", ptr->value);
rcu_read_unlock();

// 写端
struct my_data *new_data = kmalloc(sizeof(*new_data), GFP_KERNEL);
new_data->value = 42;

spin_lock(&update_lock);
struct my_data *old = global_ptr;
rcu_assign_pointer(global_ptr, new_data);  // 原子更新指针
spin_unlock(&update_lock);

// 等待所有读者完成，安全释放旧数据
synchronize_rcu();  // 或 call_rcu(&old->rcu, free_callback);
kfree(old);
```

### 3.2 工作队列与延迟工作

```c
#include <linux/workqueue.h>
#include <linux/interrupt.h>

// ========== 工作队列 ==========
// 用于异步执行工作，进程上下文（可睡眠）

declare_work(my_work, my_work_handler);

void my_work_handler(struct work_struct *work)
{
    // 在进程上下文执行
    // 可以睡眠，使用mutex等
}

// 调度工作
schedule_work(&my_work);

// 延迟工作队列
declare_delayed_work(my_delayed_work, my_work_handler);
schedule_delayed_work(&my_delayed_work, msecs_to_jiffies(1000)); // 1秒后

// 取消工作
cancel_work_sync(&my_work);
cancel_delayed_work_sync(&my_delayed_work);

// ========== Tasklet ==========
// 中断上下文，不可睡眠，同一tasklet不会并行

declare_tasklet(my_tasklet, my_tasklet_handler, 0);

void my_tasklet_handler(unsigned long data)
{
    // 中断上下文
    // 不可睡眠！
}

tasklet_schedule(&my_tasklet);

// ========== Softirq ==========
// 更底层，静态定义，通常不直接使用
```

---

## 4. 字符设备驱动

### 4.1 基础框架

```c
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mydev"
#define CLASS_NAME "myclass"
#define BUFFER_SIZE 1024

static int major;
static struct class *my_class;
static struct cdev my_cdev;
static char device_buffer[BUFFER_SIZE];
static size_t buffer_pos = 0;

// ========== 文件操作 ==========
static int mydev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[MYDEV] Device opened\n");
    return 0;
}

static int mydev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "[MYDEV] Device closed\n");
    return 0;
}

static ssize_t mydev_read(struct file *file, char __user *user_buf,
                         size_t count, loff_t *offset)
{
    size_t to_read = min(count, buffer_pos - *offset);

    if (to_read == 0)
        return 0;  // EOF

    // 复制到用户空间
    if (copy_to_user(user_buf, device_buffer + *offset, to_read))
        return -EFAULT;

    *offset += to_read;
    return to_read;
}

static ssize_t mydev_write(struct file *file, const char __user *user_buf,
                          size_t count, loff_t *offset)
{
    size_t to_write = min(count, BUFFER_SIZE - 1);

    if (copy_from_user(device_buffer, user_buf, to_write))
        return -EFAULT;

    device_buffer[to_write] = '\0';
    buffer_pos = to_write;

    printk(KERN_INFO "[MYDEV] Received: %s\n", device_buffer);
    return to_write;
}

static long mydev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    // 定义ioctl命令
    #define MYDEV_RESET _IO('M', 0)
    #define MYDEV_GET_SIZE _IOR('M', 1, int)

    switch (cmd) {
    case MYDEV_RESET:
        buffer_pos = 0;
        memset(device_buffer, 0, BUFFER_SIZE);
        printk(KERN_INFO "[MYDEV] Buffer reset\n");
        break;

    case MYDEV_GET_SIZE:
        if (put_user(buffer_pos, (int __user *)arg))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mydev_open,
    .release = mydev_release,
    .read = mydev_read,
    .write = mydev_write,
    .unlocked_ioctl = mydev_ioctl,
    .llseek = no_llseek,
};

// ========== 模块初始化/清理 ==========
static int __init mydev_init(void)
{
    dev_t dev;
    int ret;

    // 分配设备号
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "[MYDEV] Failed to allocate device number\n");
        return ret;
    }
    major = MAJOR(dev);

    // 创建设备类（/sys/class/myclass）
    my_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(my_class)) {
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(my_class);
    }

    // 初始化并添加字符设备
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    ret = cdev_add(&my_cdev, dev, 1);
    if (ret) {
        class_destroy(my_class);
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    // 创建设备节点（/dev/mydev）
    device_create(my_class, NULL, dev, NULL, DEVICE_NAME);

    printk(KERN_INFO "[MYDEV] Device registered, major: %d\n", major);
    return 0;
}

static void __exit mydev_exit(void)
{
    dev_t dev = MKDEV(major, 0);

    device_destroy(my_class, dev);
    cdev_del(&my_cdev);
    class_destroy(my_class);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "[MYDEV] Device unregistered\n");
}

module_init(mydev_init);
module_exit(mydev_exit);
MODULE_LICENSE("GPL");
```

---

## 5. 内核时间与时钟

### 5.1 时间API

```c
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>

// ========== Jiffies ==========
// 内核心跳计数器，通常是1-10ms

unsigned long j = jiffies;           // 当前jiffies
unsigned long later = j + HZ;        // 1秒后
unsigned long timeout = jiffies + msecs_to_jiffies(100); // 100ms后

// 时间比较（处理回绕）
if (time_after(jiffies, timeout)) {
    // 超时
}

// 忙等待（不推荐）
while (time_before(jiffies, timeout));

// ========== 高精度时间 ==========
ktime_t now = ktime_get();           // 单调时钟
ktime_t real = ktime_get_real();     // 实时时钟

// 时间操作
ktime_t later = ktime_add_ns(now, 1000000);  // +1ms
s64 delta = ktime_to_ns(ktime_sub(later, now));

// ========== 高精度定时器 (hrtimer) ==========
struct hrtimer timer;

enum hrtimer_restart my_timer_callback(struct hrtimer *t)
{
    // 定时器到期
    // 返回HRTIMER_NORESTART或HRTIMER_RESTART
    return HRTIMER_NORESTART;
}

hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
timer.function = my_timer_callback;
hrtimer_start(&timer, ms_to_ktime(100), HRTIMER_MODE_REL);

hrtimer_cancel(&timer);  // 取消并等待完成
```

---

## 关联导航

### 前置知识

- [系统调用机制](../13_System_Call_Implementation.md)
- [并发编程](../14_Concurrency_Parallelism/readme.md)
- [内存管理](../../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/05_Memory_Management_Hardware/readme.md)

### 后续延伸

- [进程调度算法深度](./02_Process_Scheduling_Deep.md)
- [虚拟内存子系统](./03_Virtual_Memory_Subsystem.md)
- [中断处理与软中断](./04_Interrupt_Handling.md)

### 参考

- Linux Device Drivers, 3rd Edition
- Linux Kernel Development, Robert Love
- MIT 6.828: Operating System Engineering
- Stanford CS140: Operating Systems
- Berkeley CS162: Operating Systems
