# Raspberry Pi 嵌入式Linux开发完全指南

## 目录

- [Raspberry Pi 嵌入式Linux开发完全指南](#raspberry-pi-嵌入式linux开发完全指南)
  - [目录](#目录)
  - [概述](#概述)
    - [适用硬件](#适用硬件)
    - [开发环境要求](#开发环境要求)
  - [嵌入式Linux架构](#嵌入式linux架构)
    - [1. 系统架构概览](#1-系统架构概览)
    - [2. 启动流程](#2-启动流程)
    - [3. 内存映射（Raspberry Pi 4）](#3-内存映射raspberry-pi-4)
  - [Buildroot/Yocto环境搭建](#buildrootyocto环境搭建)
    - [1. Buildroot搭建](#1-buildroot搭建)
      - [1.1 安装依赖](#11-安装依赖)
      - [1.2 获取并配置Buildroot](#12-获取并配置buildroot)
      - [1.3 Buildroot配置示例](#13-buildroot配置示例)
    - [2. Yocto Project搭建](#2-yocto-project搭建)
  - [设备树(Device Tree)配置](#设备树device-tree配置)
    - [1. 设备树源文件](#1-设备树源文件)
    - [2. 编译设备树](#2-编译设备树)
  - [内核模块开发](#内核模块开发)
    - [1. Hello World模块](#1-hello-world模块)
    - [2. Makefile](#2-makefile)
  - [字符设备驱动](#字符设备驱动)
    - [1. 完整字符设备驱动](#1-完整字符设备驱动)
  - [GPIO控制](#gpio控制)
    - [1. libgpiod用户空间控制](#1-libgpiod用户空间控制)
  - [I2C设备访问](#i2c设备访问)
    - [1. 用户空间I2C访问](#1-用户空间i2c访问)
  - [SPI设备访问](#spi设备访问)
    - [1. 用户空间SPI访问](#1-用户空间spi访问)
  - [实时补丁PREEMPT\_RT](#实时补丁preempt_rt)
    - [1. 实时补丁应用](#1-实时补丁应用)
    - [2. 实时性测试程序](#2-实时性测试程序)
  - [系统启动优化](#系统启动优化)
    - [1. 启动优化配置](#1-启动优化配置)
    - [2. 系统服务优化脚本](#2-系统服务优化脚本)
  - [实际项目：Modbus网关](#实际项目modbus网关)
    - [1. Modbus网关主程序](#1-modbus网关主程序)
    - [2. Makefile](#2-makefile-1)
    - [3. systemd服务](#3-systemd服务)
    - [4. 配置文件](#4-配置文件)
  - [容器化部署](#容器化部署)
    - [1. Dockerfile](#1-dockerfile)
    - [2. Docker Compose](#2-docker-compose)
    - [3. Kubernetes部署](#3-kubernetes部署)
  - [项目目录结构](#项目目录结构)
  - [总结](#总结)
  - [参考资源](#参考资源)

---

## 概述

本文档提供Raspberry Pi嵌入式Linux开发的全面指南，涵盖从底层驱动开发到系统级优化的完整技术栈。

### 适用硬件

- Raspberry Pi 4 Model B (推荐)
- Raspberry Pi 3 Model B/B+
- Raspberry Pi Zero 2 W
- Raspberry Pi Compute Module 4

### 开发环境要求

- 主机系统：Ubuntu 22.04 LTS / Debian 11
- 交叉编译工具链：aarch64-linux-gnu-gcc
- 内核源码：linux-rpi 5.15+

---

## 嵌入式Linux架构

### 1. 系统架构概览

```
┌─────────────────────────────────────────────────────────┐
│                    用户空间应用程序                        │
├─────────────────────────────────────────────────────────┤
│  glibc  │  systemd  │  设备节点(/dev)  │  sysfs(/sys)   │
├─────────────────────────────────────────────────────────┤
│                    Linux内核                             │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐     │
│  │  进程调度 │  │ 内存管理 │  │ 文件系统│  │ 网络栈  │     │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘     │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐     │
│  │GPIO驱动 │  │ I2C驱动 │  │ SPI驱动 │  │ UART驱动│     │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘     │
├─────────────────────────────────────────────────────────┤
│              板级支持包(BSP) / 设备树                     │
├─────────────────────────────────────────────────────────┤
│              硬件抽象层 (ARM固件/bootloader)               │
├─────────────────────────────────────────────────────────┤
│  BCM2711 SoC (ARM Cortex-A72 + 外设控制器)               │
└─────────────────────────────────────────────────────────┘
```

### 2. 启动流程

```
GPU ROM → bootcode.bin → start4.elf → config.txt → u-boot/kernel
                                          ↓
                                    Device Tree
                                          ↓
                               Kernel + initramfs
                                          ↓
                                    systemd/init
```

### 3. 内存映射（Raspberry Pi 4）

| 地址范围 | 用途 |
|---------|------|
| 0x00000000 - 0x3B400000 | 系统RAM (948MB) |
| 0x3B400000 - 0x40000000 | GPU保留内存 |
| 0xFE000000 - 0xFE200000 | GPIO控制器 |
| 0xFE804000 - 0xFE804100 | I2C0 (BSC0) |
| 0xFE805000 - 0xFE805100 | I2C1 (BSC1) |
| 0xFE204000 - 0xFE205000 | SPI0 |
| 0xFE215000 - 0xFE215100 | Mini UART |
| 0xFE201000 - 0xFE201100 | PL011 UART |

---

## Buildroot/Yocto环境搭建

### 1. Buildroot搭建

#### 1.1 安装依赖

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential git wget unzip bc \
    libncurses5-dev libncursesw5-dev \
    zlib1g-dev gawk bison flex patch \
    texinfo subversion \
    libssl-dev libelf-dev \
    autoconf-archive \
    device-tree-compiler
```

#### 1.2 获取并配置Buildroot

```bash
# 克隆Buildroot
git clone https://github.com/buildroot/buildroot.git
cd buildroot
git checkout 2023.11

# 应用Raspberry Pi配置
make raspberrypi4_64_defconfig

# 配置选项
make menuconfig
```

#### 1.3 Buildroot配置示例

```makefile
# 目标架构
BR2_aarch64=y
BR2_cortex_a72=y

# 目标配置
BR2_TARGET_GENERIC_HOSTNAME="rpi4-gateway"
BR2_TARGET_GENERIC_ISSUE="Welcome to Raspberry Pi 4 Modbus Gateway"
BR2_TARGET_GENERIC_ROOT_PASSWD="root"
BR2_TARGET_GENERIC_GETTY_PORT="tty1"

# 内核
BR2_LINUX_KERNEL=y
BR2_LINUX_KERNEL_CUSTOM_TARBALL=y
BR2_LINUX_KERNEL_CUSTOM_TARBALL_LOCATION="https://github.com/raspberrypi/linux/archive/refs/tags/rpi-5.15.y.tar.gz"
BR2_LINUX_KERNEL_DEFCONFIG="bcm2711"
BR2_LINUX_KERNEL_DTS_SUPPORT=y
BR2_LINUX_KERNEL_INTREE_DTS_NAME="broadcom/bcm2711-rpi-4-b"

# 文件系统
BR2_TARGET_ROOTFS_EXT2=y
BR2_TARGET_ROOTFS_EXT2_4=y
BR2_TARGET_ROOTFS_EXT2_SIZE="256M"
BR2_TARGET_ROOTFS_TAR=y

# 软件包
BR2_PACKAGE_I2C_TOOLS=y
BR2_PACKAGE_SPI_TOOLS=y
BR2_PACKAGE_DHCPCD=y
BR2_PACKAGE_DROPBEAR=y

# 实时补丁
BR2_LINUX_KERNEL_PATCH="/path/to/patch-5.15.35-rt34.patch.xz"
```

### 2. Yocto Project搭建

```bash
# 创建Yocto工作目录
mkdir ~/yocto-rpi && cd ~/yocto-rpi

# 克隆必要的层
git clone -b kirkstone git://git.yoctoproject.org/poky.git
git clone -b kirkstone git://git.openembedded.org/meta-openembedded
git clone -b kirkstone git://git.yoctoproject.org/meta-raspberrypi

# 初始化构建环境
source poky/oe-init-build-env rpi-build
```

---

## 设备树(Device Tree)配置

### 1. 设备树源文件

```dts
// bcm2711-rpi-4-b-custom.dts
/dts-v1/;
/plugin/;

#include "bcm2711.dtsi"
#include "bcm2711-rpi.dtsi"

/ {
    compatible = "raspberrypi,4-model-b", "brcm,bcm2711";
    model = "Raspberry Pi 4 Model B - Industrial Gateway";

    /* 自定义属性 */
    gateway {
        compatible = "custom,modbus-gateway";
        status = "okay";
        serial-number = "GW-2024-0001";
        hardware-revision = "v1.2";
    };
};

/* GPIO扩展配置 */
&gpio {
    led_ctrl: led_ctrl {
        brcm,pins = <16 17 18>;
        brcm,function = <BCM2835_GPIO_F_OUT>;
        brcm,pull = <BCM2835_GPIO_PUD_OFF>;
    };

    button_inputs: button_inputs {
        brcm,pins = <20 21 26>;
        brcm,function = <BCM2835_GPIO_F_IN>;
        brcm,pull = <BCM2835_GPIO_PUD_UP>;
    };
};

/* I2C0配置 */
&i2c0 {
    pinctrl-names = "default";
    pinctrl-0 = <&i2c0_gpio40>;
    status = "okay";
    clock-frequency = <400000>;

    /* MCP23017 GPIO扩展器 */
    mcp23017@20 {
        compatible = "microchip,mcp23017";
        reg = <0x20>;
        gpio-controller;
        #gpio-cells = <2>;
        interrupt-parent = <&gpio>;
        interrupts = <24 IRQ_TYPE_LEVEL_LOW>;
        interrupt-controller;
        #interrupt-cells = <2>;
    };
};

/* SPI0配置 */
&spi0 {
    pinctrl-names = "default";
    pinctrl-0 = <&spi0_gpio7>;
    cs-gpios = <&gpio 8 1>, <&gpio 7 1>;
    status = "okay";

    /* ADS1118 ADC */
    adc@0 {
        compatible = "ti,ads1118";
        reg = <0>;
        spi-max-frequency = <1000000>;
        status = "okay";
    };
};

/* UART2 - Modbus RTU */
&uart2 {
    pinctrl-names = "default";
    pinctrl-0 = <&uart2_gpio0>;
    status = "okay";
    current-speed = <9600>;
};
```

### 2. 编译设备树

```bash
# 编译DTS为DTB
dtc -@ -I dts -O dtb -o bcm2711-rpi-4-b-custom.dtb \
    bcm2711-rpi-4-b-custom.dts

# 编译覆盖层
dtc -@ -I dts -O dtb -o rpi-custom-overlay.dtbo \
    rpi-custom-overlay.dts
```

---

## 内核模块开发

### 1. Hello World模块

```c
// hello_module.c - 基础内核模块
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>

#define MODULE_NAME     "rpi_hello"
#define PROC_FILENAME   "rpi_hello"

static char *user_name = "Raspberry Pi";
module_param(user_name, charp, 0644);
MODULE_PARM_DESC(user_name, "User name to greet");

static int loop_count = 3;
module_param(loop_count, int, 0644);
MODULE_PARM_DESC(loop_count, "Number of times to print greeting");

/* 模块统计信息 */
struct hello_stats {
    u64 load_time;
    u64 unload_time;
    u64 read_count;
};

static struct hello_stats stats;

/* proc文件系统操作 */
static int hello_seq_show(struct seq_file *s, void *v)
{
    u64 now = ktime_get_ns();

    seq_printf(s, "=== %s Module Statistics ===\n\n", MODULE_NAME);
    seq_printf(s, "User Name:   %s\n", user_name);
    seq_printf(s, "Loop Count:  %d\n", loop_count);
    seq_printf(s, "Load Time:   %llu ns\n", stats.load_time);
    seq_printf(s, "Uptime:      %llu ms\n", (now - stats.load_time) / 1000000);
    seq_printf(s, "Read Count:  %llu\n", stats.read_count);

    stats.read_count++;
    return 0;
}

static int hello_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, hello_seq_show, NULL);
}

static const struct proc_ops hello_proc_ops = {
    .proc_open    = hello_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static struct proc_dir_entry *hello_proc_entry;

/* 模块初始化 */
static int __init hello_init(void)
{
    printk(KERN_INFO "[%s] Module loading...\n", MODULE_NAME);

    stats.load_time = ktime_get_ns();

    printk(KERN_INFO "[%s] Hello, %s!\n", MODULE_NAME, user_name);

    for (int i = 0; i < loop_count; i++) {
        printk(KERN_INFO "[%s] Greeting %d/%d\n", MODULE_NAME, i + 1, loop_count);
    }

    hello_proc_entry = proc_create(PROC_FILENAME, 0444, NULL, &hello_proc_ops);
    if (!hello_proc_entry) {
        printk(KERN_ERR "[%s] Failed to create proc entry\n", MODULE_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "[%s] Module loaded successfully\n", MODULE_NAME);
    return 0;
}

/* 模块清理 */
static void __exit hello_exit(void)
{
    stats.unload_time = ktime_get_ns();

    if (hello_proc_entry)
        proc_remove(hello_proc_entry);

    printk(KERN_INFO "[%s] Goodbye, %s!\n", MODULE_NAME, user_name);
    printk(KERN_INFO "[%s] Module uptime: %llu ms\n",
           MODULE_NAME,
           (stats.unload_time - stats.load_time) / 1000000);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Industrial IoT Team");
MODULE_DESCRIPTION("Raspberry Pi Hello World Kernel Module");
MODULE_VERSION("1.0.0");
```

### 2. Makefile

```makefile
# Makefile - 内核模块编译
KDIR ?= /lib/modules/$(shell uname -r)/build
ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

obj-m += hello_module.o
obj-m += gpio_driver.o
obj-m += i2c_device.o
obj-m += char_device.o

all:
 $(MAKE) -C $(KDIR) M=$(PWD) modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean:
 $(MAKE) -C $(KDIR) M=$(PWD) clean ARCH=$(ARCH)

install:
 scp *.ko pi@raspberrypi:/tmp/

test:
 sudo insmod hello_module.ko user_name="Test" loop_count=5
 cat /proc/rpi_hello
 sudo rmmod hello_module
 sudo dmesg | tail -20

.PHONY: all clean install test
```

---

## 字符设备驱动

### 1. 完整字符设备驱动

```c
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
```

---

## GPIO控制

### 1. libgpiod用户空间控制

```c
// gpio_control.c - libgpiod GPIO控制
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <gpiod.h>
#include <pthread.h>

#define GPIO_CHIP       "gpiochip0"
#define GPIO_LED        16
#define GPIO_BUTTON     20
#define GPIO_INPUT      21
#define GPIO_RELAY1     23
#define GPIO_RELAY2     24

static volatile int running = 1;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

static void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

static void get_timestamp(char *buf, size_t len)
{
    struct timespec ts;
    struct tm tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &tm_info);

    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
             tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
             ts.tv_nsec / 1000000);
}

int gpio_basic_example(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line *led_line;
    struct gpiod_line *btn_line;
    int ret = 0;

    printf("=== Basic GPIO Control ===\n");

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) {
        fprintf(stderr, "Failed to open GPIO chip\n");
        return -1;
    }

    led_line = gpiod_chip_get_line(chip, GPIO_LED);
    if (!led_line) { ret = -1; goto close_chip; }

    ret = gpiod_line_request_output(led_line, "gpio_basic", 0);
    if (ret < 0) goto release_led;

    btn_line = gpiod_chip_get_line(chip, GPIO_BUTTON);
    if (!btn_line) { ret = -1; goto release_led; }

    ret = gpiod_line_request_input(btn_line, "gpio_basic");
    if (ret < 0) goto release_btn;

    printf("LED blinking (press Ctrl+C to stop)...\n");
    for (int i = 0; i < 10 && running; i++) {
        gpiod_line_set_value(led_line, 1);
        printf("LED ON, Button: %d\n", gpiod_line_get_value(btn_line));
        usleep(500000);

        gpiod_line_set_value(led_line, 0);
        printf("LED OFF, Button: %d\n", gpiod_line_get_value(btn_line));
        usleep(500000);
    }
    printf("\n");

release_btn:
    gpiod_line_release(btn_line);
release_led:
    gpiod_line_release(led_line);
close_chip:
    gpiod_chip_close(chip);
    return ret;
}

void *gpio_interrupt_thread(void *arg)
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_event event;
    struct timespec ts;
    char timestamp[32];
    int ret;
    int pin = *(int *)arg;

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) return NULL;

    line = gpiod_chip_get_line(chip, pin);
    if (!line) { gpiod_chip_close(chip); return NULL; }

    ret = gpiod_line_request_both_edges_events(line, "gpio_interrupt");
    if (ret < 0) {
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return NULL;
    }

    pthread_mutex_lock(&print_mutex);
    printf("Interrupt monitoring started on GPIO%d\n", pin);
    pthread_mutex_unlock(&print_mutex);

    while (running) {
        ts.tv_sec = 1;
        ts.tv_nsec = 0;

        ret = gpiod_line_event_wait(line, &ts);
        if (ret < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) continue;

        ret = gpiod_line_event_read(line, &event);
        if (ret < 0) continue;

        get_timestamp(timestamp, sizeof(timestamp));

        pthread_mutex_lock(&print_mutex);
        printf("[%s] GPIO%d: %s edge detected\n",
               timestamp, pin,
               event.event_type == GPIOD_LINE_EVENT_RISING_EDGE ? "Rising" : "Falling");
        pthread_mutex_unlock(&print_mutex);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return NULL;
}

int gpio_bulk_example(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_bulk lines;
    struct gpiod_line *line_list[4];
    int values[4] = {1, 0, 1, 0};
    int ret;
    int pins[] = {GPIO_RELAY1, GPIO_RELAY2, GPIO_LED, GPIO_INPUT};

    printf("=== Bulk GPIO Control ===\n");

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) return -1;

    gpiod_line_bulk_init(&lines);

    for (int i = 0; i < 4; i++) {
        line_list[i] = gpiod_chip_get_line(chip, pins[i]);
        if (!line_list[i]) { ret = -1; goto cleanup; }
        gpiod_line_bulk_add(&lines, line_list[i]);
    }

    ret = gpiod_line_request_bulk_output(&lines, "gpio_bulk", values);
    if (ret < 0) goto cleanup;

    printf("Bulk values set: [%d, %d, %d, %d]\n",
           values[0], values[1], values[2], values[3]);

    int new_values[4] = {0, 1, 0, 1};
    gpiod_line_set_value_bulk(&lines, new_values);
    printf("Bulk values updated: [%d, %d, %d, %d]\n",
           new_values[0], new_values[1], new_values[2], new_values[3]);

    sleep(1);

cleanup:
    gpiod_line_release_bulk(&lines);
    gpiod_chip_close(chip);
    return ret;
}

int main(int argc, char *argv[])
{
    pthread_t int_thread;
    int int_pin = GPIO_INPUT;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Raspberry Pi GPIO Control Demo\n");
    printf("==============================\n\n");

    gpio_basic_example();
    gpio_bulk_example();

    pthread_create(&int_thread, NULL, gpio_interrupt_thread, &int_pin);

    printf("\nPress Ctrl+C to exit...\n");
    while (running) sleep(1);

    pthread_join(int_thread, NULL);

    printf("\nProgram exiting...\n");
    return 0;
}
```

---

## I2C设备访问

### 1. 用户空间I2C访问

```c
// i2c_userspace.c - 用户空间I2C访问
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#define I2C_BUS     "/dev/i2c-1"
#define MCP23017_ADDR   0x20
#define BME280_ADDR     0x76

int i2c_smbus_read_byte(int file, uint8_t reg, uint8_t *val)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    args.read_write = I2C_SMBUS_READ;
    args.command = reg;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    if (ioctl(file, I2C_SMBUS, &args) < 0) return -1;

    *val = data.byte;
    return 0;
}

int i2c_smbus_write_byte(int file, uint8_t reg, uint8_t val)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    data.byte = val;
    args.read_write = I2C_SMBUS_WRITE;
    args.command = reg;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    return ioctl(file, I2C_SMBUS, &args);
}

int mcp23017_test(int file)
{
    uint8_t val;
    int ret;

    printf("=== MCP23017 Test ===\n");

    if (ioctl(file, I2C_SLAVE, MCP23017_ADDR) < 0) {
        perror("Failed to set I2C address");
        return -1;
    }

    ret = i2c_smbus_read_byte(file, 0x00, &val);
    if (ret < 0) { perror("Failed to read IODIR"); return -1; }
    printf("IODIRA: 0x%02X\n", val);

    i2c_smbus_read_byte(file, 0x12, &val);
    printf("GPIOA: 0x%02X\n", val);

    i2c_smbus_read_byte(file, 0x13, &val);
    printf("GPIOB: 0x%02X\n", val);

    return 0;
}

int i2c_scan(int file)
{
    int addr;
    int found = 0;

    printf("=== I2C Bus Scan ===\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:          ");

    for (addr = 0x03; addr <= 0x77; addr++) {
        if (addr % 16 == 0) printf("\n%02x: ", addr);

        if (ioctl(file, I2C_SLAVE, addr) < 0) {
            printf("-- ");
            continue;
        }

        uint8_t buf;
        if (read(file, &buf, 1) == 1) {
            printf("%02X ", addr);
            found++;
        } else {
            printf("-- ");
        }
    }

    printf("\n\nFound %d device(s)\n", found);
    return found;
}

int main(int argc, char *argv[])
{
    int file;

    file = open(I2C_BUS, O_RDWR);
    if (file < 0) {
        perror("Failed to open I2C bus");
        printf("Try: sudo %s\n", argv[0]);
        return 1;
    }

    printf("I2C User Space Access Demo\n");
    printf("==========================\n\n");

    i2c_scan(file);
    printf("\n");
    mcp23017_test(file);

    close(file);
    return 0;
}
```

---

## SPI设备访问

### 1. 用户空间SPI访问

```c
// spi_userspace.c - 用户空间SPI访问
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include <gpiod.h>

#define SPI_DEVICE      "/dev/spidev0.0"
#define SPI_SPEED       1000000
#define GPIO_CS         25

/* ADS1118配置 */
#define ADS1118_CONFIG_OS       (1 << 15)
#define ADS1118_CONFIG_MUX_SHIFT 12
#define ADS1118_CONFIG_PGA_SHIFT 9
#define ADS1118_CONFIG_MODE     (1 << 8)
#define ADS1118_CONFIG_DR_SHIFT 5
#define ADS1118_CONFIG_PULL_UP  (1 << 3)

static uint16_t ads1118_make_config(uint8_t channel, uint8_t pga, uint8_t dr)
{
    uint16_t config = 0;
    config |= ADS1118_CONFIG_OS;
    config |= ((channel & 0x07) << ADS1118_CONFIG_MUX_SHIFT);
    config |= ((pga & 0x07) << ADS1118_CONFIG_PGA_SHIFT);
    config |= ADS1118_CONFIG_MODE;
    config |= ((dr & 0x07) << ADS1118_CONFIG_DR_SHIFT);
    config |= ADS1118_CONFIG_PULL_UP;
    config |= 0x0003;
    return config;
}

int spi_transfer(int fd, uint8_t *tx, uint8_t *rx, size_t len, int gpio_cs)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = SPI_SPEED,
        .delay_usecs = 0,
        .bits_per_word = 8,
    };

    if (gpio_cs >= 0) {
        struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
        if (chip) {
            struct gpiod_line *line = gpiod_chip_get_line(chip, gpio_cs);
            if (line) {
                gpiod_line_request_output(line, "spi_cs", 0);
                gpiod_line_set_value(line, 0);
                usleep(10);
            }
        }
    }

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (gpio_cs >= 0) {
        struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
        if (chip) {
            struct gpiod_line *line = gpiod_chip_get_line(chip, gpio_cs);
            if (line) {
                gpiod_line_set_value(line, 1);
            }
            gpiod_chip_close(chip);
        }
    }

    return ret;
}

int ads1118_read(int fd, uint8_t channel, int gpio_cs)
{
    uint16_t config = ads1118_make_config(channel, 0x01, 0x04);
    uint8_t tx_buf[4];
    uint8_t rx_buf[4];
    int16_t result;

    tx_buf[0] = (config >> 8) & 0xFF;
    tx_buf[1] = config & 0xFF;
    tx_buf[2] = tx_buf[0];
    tx_buf[3] = tx_buf[1];

    spi_transfer(fd, tx_buf, rx_buf, 4, gpio_cs);
    usleep(10000);

    tx_buf[0] = 0;
    tx_buf[1] = 0;
    tx_buf[2] = 0;
    tx_buf[3] = 0;

    spi_transfer(fd, tx_buf, rx_buf, 4, gpio_cs);

    result = ((int16_t)rx_buf[0] << 8) | rx_buf[1];
    return result;
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    uint8_t mode = SPI_MODE_1;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open SPI device");
        return 1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ret |= ioctl(fd, SPI_IOC_RD_MODE, &mode);
    ret |= ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ret |= ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ret |= ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret |= ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    if (ret < 0) {
        perror("Failed to configure SPI");
        close(fd);
        return 1;
    }

    printf("SPI User Space Access Demo\n");
    printf("==========================\n");
    printf("Mode: %d, Bits: %d, Speed: %d Hz\n\n", mode, bits, speed);

    printf("=== ADS1118 ADC Read ===\n");
    for (int i = 0; i < 4; i++) {
        int val = ads1118_read(fd, i, GPIO_CS);
        float voltage = (val * 4.096) / 32768.0;
        printf("Channel %d: %d (%.3f V)\n", i, val, voltage);
        usleep(1000);
    }

    close(fd);
    return 0;
}
```

---

## 实时补丁PREEMPT_RT

### 1. 实时补丁应用

```bash
#!/bin/bash
# apply_rt_patch.sh - 应用PREEMPT_RT补丁

KERNEL_VERSION="5.15.35"
RT_PATCH_VERSION="5.15.35-rt41"

# 下载内核和补丁
cd /usr/src
wget https://mirrors.edge.kernel.org/pub/linux/kernel/v5.x/linux-${KERNEL_VERSION}.tar.xz
wget https://mirrors.edge.kernel.org/pub/linux/kernel/projects/rt/5.15/patch-${RT_PATCH_VERSION}.patch.xz

# 解压
tar -xf linux-${KERNEL_VERSION}.tar.xz
cd linux-${KERNEL_VERSION}

# 应用补丁
xzcat ../patch-${RT_PATCH_VERSION}.patch.xz | patch -p1

# 配置内核
cp /boot/config-$(uname -r) .config
make oldconfig

# 启用PREEMPT_RT
cat >> .config << 'EOF'
CONFIG_PREEMPT_RT=y
CONFIG_PREEMPT=y
CONFIG_PREEMPT_RT_SHADOW=y
CONFIG_HZ_1000=y
CONFIG_HZ=1000
EOF

# 编译
make -j$(nproc)
make modules_install
make install

echo "RT kernel installed. Reboot to use."
```

### 2. 实时性测试程序

```c
// rt_latency_test.c - 实时性测试
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>

#define NSEC_PER_SEC    1000000000ULL
#define INTERVAL_NS     1000000ULL  /* 1ms周期 */

static volatile int running = 1;

static inline unsigned long long timespec_to_ns(struct timespec *ts)
{
    return (unsigned long long)ts->tv_sec * NSEC_PER_SEC + ts->tv_nsec;
}

static void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

int setup_realtime(int priority)
{
    struct sched_param param;
    cpu_set_t cpuset;

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -1;
    }

    /* 绑定到CPU0 */
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
        perror("sched_setaffinity failed");
        return -1;
    }

    /* 设置SCHED_FIFO调度 */
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed (try running as root)");
        return -1;
    }

    return 0;
}

void latency_test(void)
{
    struct timespec next, now, diff;
    unsigned long long min_latency = -1ULL, max_latency = 0;
    unsigned long long sum_latency = 0;
    unsigned long long latency;
    int iterations = 0;
    int max_iterations = 10000;

    printf("=== Real-time Latency Test ===\n");
    printf("Interval: %llu ns (%.2f kHz)\n", INTERVAL_NS,
           NSEC_PER_SEC / (double)INTERVAL_NS / 1000);
    printf("Running %d iterations...\n\n", max_iterations);

    clock_gettime(CLOCK_MONOTONIC, &next);

    while (running && iterations < max_iterations) {
        /* 计算下一个唤醒时间 */
        next.tv_nsec += INTERVAL_NS;
        if (next.tv_nsec >= NSEC_PER_SEC) {
            next.tv_sec++;
            next.tv_nsec -= NSEC_PER_SEC;
        }

        /* 等待 */
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);

        /* 测量延迟 */
        clock_gettime(CLOCK_MONOTONIC, &now);

        if (now.tv_sec > next.tv_sec ||
            (now.tv_sec == next.tv_sec && now.tv_nsec > next.tv_nsec)) {
            diff.tv_sec = now.tv_sec - next.tv_sec;
            diff.tv_nsec = now.tv_nsec - next.tv_nsec;
        } else {
            diff.tv_sec = 0;
            diff.tv_nsec = 0;
        }

        latency = timespec_to_ns(&diff);

        if (latency < min_latency) min_latency = latency;
        if (latency > max_latency) max_latency = latency;
        sum_latency += latency;
        iterations++;
    }

    printf("Results:\n");
    printf("  Iterations: %d\n", iterations);
    printf("  Min latency: %llu ns (%.2f us)\n", min_latency, min_latency / 1000.0);
    printf("  Max latency: %llu ns (%.2f us)\n", max_latency, max_latency / 1000.0);
    printf("  Avg latency: %llu ns (%.2f us)\n",
           sum_latency / iterations, (sum_latency / iterations) / 1000.0);
    printf("  Jitter: %llu ns (%.2f us)\n",
           max_latency - min_latency, (max_latency - min_latency) / 1000.0);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Setting up real-time scheduling...\n");
    if (setup_realtime(80) < 0) {
        printf("Continuing without real-time scheduling...\n");
    }

    latency_test();

    return 0;
}
```

---

## 系统启动优化

### 1. 启动优化配置

```bash
# /boot/cmdline.txt - 内核启动参数
console=serial0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 \\
    elevator=deadline fsck.repair=yes rootwait quiet \\
    init=/bin/systemd logo.nologo vt.global_cursor_default=0 \\
    fastboot noswap ro

# /boot/config.txt - 配置文件
dtparam=audio=off
dtoverlay=disable-bt
dtoverlay=disable-wifi
dtparam=i2c_arm=on
dtparam=spi=on
dtparam=uart0=on

# 禁用不必要的服务
start_file=start4cd.elf
fixup_file=fixup4cd.dat

# GPU内存最小化
gpu_mem=16

# 超频（谨慎使用）
arm_freq=1800
over_voltage=6
force_turbo=1
```

### 2. 系统服务优化脚本

```bash
#!/bin/bash
# optimize_boot.sh - 启动优化脚本

echo "=== Raspberry Pi Boot Optimizer ==="

# 1. 禁用不必要的服务
SYSTEMD_SERVICES="
    bluetooth
    triggerhappy
    man-db
    apt-daily
    apt-daily-upgrade
    dphys-swapfile
    fake-hwclock
    alsa-state
    alsa-restore
"

for service in $SYSTEMD_SERVICES; do
    echo "Disabling $service..."
    systemctl disable $service 2>/dev/null
    systemctl stop $service 2>/dev/null
done

# 2. 配置只读文件系统
cat > /etc/fstab << 'EOF'
proc            /proc           proc    defaults          0       0
/dev/mmcblk0p1  /boot           vfat    defaults,ro       0       2
/dev/mmcblk0p2  /               ext4    defaults,ro       0       1
tmpfs           /tmp            tmpfs   defaults,nosuid,nodev,size=100m 0 0
tmpfs           /var/log        tmpfs   defaults,nosuid,nodev,size=50m  0 0
tmpfs           /var/tmp        tmpfs   defaults,nosuid,nodev,size=10m  0 0
EOF

# 3. 配置RAM日志
cat > /etc/logrotate.d/ramlog << 'EOF'
/var/log/*.log {
    daily
    rotate 3
    compress
    delaycompress
    missingok
    notifempty
    create 644 root root
    sharedscripts
    postrotate
        /usr/lib/rsyslog/rsyslog-rotate > /dev/null 2>&1 || true
    endscript
}
EOF

# 4. 优化内核模块加载
cat > /etc/modprobe.d/blacklist.conf << 'EOF'
# Blacklist unnecessary modules
blacklist bcm2835_v4l2
blacklist snd_bcm2835
blacklist hci_uart
blacklist btbcm
blacklist bluetooth
EOF

# 5. 创建快速启动脚本
cat > /usr/local/bin/fastboot << 'EOF'
#!/bin/bash
# Fast boot configuration

# Mount boot partition read-write when needed
mount -o remount,rw /boot

# Mount root read-write
mount -o remount,rw /

# Sync on shutdown
mount -o remount,ro /
mount -o remount,ro /boot
EOF
chmod +x /usr/local/bin/fastboot

echo "Optimization complete. Reboot to apply changes."
```

---

## 实际项目：Modbus网关

### 1. Modbus网关主程序

```c
// modbus_gateway.c - Modbus RTU/TCP网关
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <modbus/modbus.h>
#include <modbus/modbus-tcp.h>
#include <modbus/modbus-rtu.h>
#include <gpiod.h>
#include <json-c/json.h>

#define VERSION         "2.0.0"
#define DEFAULT_PORT    502
#define DEFAULT_DEVICE  "/dev/ttyUSB0"
#define DEFAULT_BAUD    9600
#define MAX_CONNECTIONS 10
#define MAX_SLAVES      32
#define REG_COUNT       1000

/* 配置结构 */
struct gateway_config {
    char rtu_device[256];
    int rtu_baud;
    char rtu_parity;
    int rtu_data_bits;
    int rtu_stop_bits;
    int tcp_port;
    int tcp_timeout;
    int rs485_gpio;
    int debug_level;
    char mapping_file[256];
};

/* 网关状态 */
struct gateway_state {
    modbus_t *rtu_ctx;
    modbus_t *tcp_ctx;
    struct gpiod_chip *gpio_chip;
    struct gpiod_line *rs485_line;

    pthread_mutex_t rtu_lock;
    pthread_mutex_t data_lock;

    uint16_t holding_regs[REG_COUNT];
    uint16_t input_regs[REG_COUNT];
    uint8_t coils[REG_COUNT / 8];
    uint8_t discrete_inputs[REG_COUNT / 8];

    volatile int running;
    int client_count;
};

static struct gateway_state g_state = {0};
static struct gateway_config g_config = {
    .rtu_device = "/dev/ttyUSB0",
    .rtu_baud = 9600,
    .rtu_parity = 'N',
    .rtu_data_bits = 8,
    .rtu_stop_bits = 1,
    .tcp_port = 502,
    .tcp_timeout = 5,
    .rs485_gpio = -1,
    .debug_level = 1,
    .mapping_file = "/etc/modbus-gateway/mapping.json",
};

/* RS485方向控制 */
void rs485_set_tx(int tx_enable)
{
    if (g_state.rs485_line) {
        gpiod_line_set_value(g_state.rs485_line, tx_enable);
        usleep(tx_enable ? 1000 : 100);
    }
}

/* 信号处理 */
static void signal_handler(int sig)
{
    (void)sig;
    g_state.running = 0;
}

/* 读取配置文件 */
int load_config(const char *filename)
{
    FILE *fp;
    char buf[4096];
    struct json_object *root, *obj;

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Using default configuration\n");
        return 0;
    }

    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    buf[n] = '\0';
    fclose(fp);

    root = json_tokener_parse(buf);
    if (!root) {
        fprintf(stderr, "Failed to parse config file\n");
        return -1;
    }

    if (json_object_object_get_ex(root, "rtu", &obj)) {
        struct json_object *val;
        if (json_object_object_get_ex(obj, "device", &val))
            strncpy(g_config.rtu_device, json_object_get_string(val), 255);
        if (json_object_object_get_ex(obj, "baud", &val))
            g_config.rtu_baud = json_object_get_int(val);
        if (json_object_object_get_ex(obj, "rs485_gpio", &val))
            g_config.rs485_gpio = json_object_get_int(val);
    }

    if (json_object_object_get_ex(root, "tcp", &obj)) {
        struct json_object *val;
        if (json_object_object_get_ex(obj, "port", &val))
            g_config.tcp_port = json_object_get_int(val);
        if (json_object_object_get_ex(obj, "timeout", &val))
            g_config.tcp_timeout = json_object_get_int(val);
    }

    json_object_put(root);
    printf("Configuration loaded from %s\n", filename);
    return 0;
}

/* RTU通信线程 */
void *rtu_thread(void *arg)
{
    (void)arg;
    uint16_t buffer[125];
    int ret;
    int slave_ids[] = {1, 2, 3, 4, 5};
    int num_slaves = sizeof(slave_ids) / sizeof(slave_ids[0]);

    printf("[RTU] Thread started\n");

    while (g_state.running) {
        for (int i = 0; i < num_slaves && g_state.running; i++) {
            int slave_id = slave_ids[i];

            pthread_mutex_lock(&g_state.rtu_lock);
            modbus_set_slave(g_state.rtu_ctx, slave_id);

            /* 读取保持寄存器 */
            rs485_set_tx(0);
            ret = modbus_read_registers(g_state.rtu_ctx, 0, 10, buffer);

            if (ret == 10) {
                pthread_mutex_lock(&g_state.data_lock);
                memcpy(&g_state.holding_regs[slave_id * 100], buffer, ret * 2);
                pthread_mutex_unlock(&g_state.data_lock);

                if (g_config.debug_level >= 2) {
                    printf("[RTU] Slave %d: Read %d registers\n", slave_id, ret);
                }
            } else {
                if (g_config.debug_level >= 1) {
                    fprintf(stderr, "[RTU] Slave %d: Read failed: %s\n",
                            slave_id, modbus_strerror(errno));
                }
            }

            pthread_mutex_unlock(&g_state.rtu_lock);

            usleep(100000);  /* 100ms between slaves */
        }
    }

    printf("[RTU] Thread stopped\n");
    return NULL;
}

/* 处理TCP客户端 */
void *tcp_client_thread(void *arg)
{
    int client_sock = *(int *)arg;
    modbus_t *ctx;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;

    free(arg);

    ctx = modbus_new_tcp("127.0.0.1", 0);
    if (!ctx) {
        close(client_sock);
        return NULL;
    }

    modbus_set_socket(ctx, client_sock);

    __sync_add_and_fetch(&g_state.client_count, 1);
    printf("[TCP] Client connected (total: %d)\n", g_state.client_count);

    while (g_state.running) {
        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            int slave_id = query[6];
            int function = query[7];
            int addr = (query[8] << 8) | query[9];
            int count = (query[10] << 8) | query[11];

            if (g_config.debug_level >= 2) {
                printf("[TCP] Request: slave=%d, func=%d, addr=%d, count=%d\n",
                       slave_id, function, addr, count);
            }

            /* 处理本地数据 */
            pthread_mutex_lock(&g_state.data_lock);

            uint8_t response[MODBUS_TCP_MAX_ADU_LENGTH];
            int resp_len;

            response[0] = query[0];  /* Transaction ID */
            response[1] = query[1];
            response[2] = 0;         /* Protocol ID */
            response[3] = 0;
            response[4] = 0;         /* Length (fill later) */
            response[5] = 0;
            response[6] = slave_id;
            response[7] = function;

            switch (function) {
            case 0x03:  /* Read Holding Registers */
                if (addr + count <= REG_COUNT) {
                    response[8] = count * 2;
                    for (int i = 0; i < count; i++) {
                        response[9 + i * 2] = g_state.holding_regs[addr + i] >> 8;
                        response[10 + i * 2] = g_state.holding_regs[addr + i] & 0xFF;
                    }
                    resp_len = 9 + count * 2;
                } else {
                    response[7] = function | 0x80;
                    response[8] = 0x02;  /* Illegal data address */
                    resp_len = 9;
                }
                break;

            case 0x04:  /* Read Input Registers */
                if (addr + count <= REG_COUNT) {
                    response[8] = count * 2;
                    for (int i = 0; i < count; i++) {
                        response[9 + i * 2] = g_state.input_regs[addr + i] >> 8;
                        response[10 + i * 2] = g_state.input_regs[addr + i] & 0xFF;
                    }
                    resp_len = 9 + count * 2;
                } else {
                    response[7] = function | 0x80;
                    response[8] = 0x02;
                    resp_len = 9;
                }
                break;

            default:
                /* 转发到RTU */
                pthread_mutex_unlock(&g_state.data_lock);

                pthread_mutex_lock(&g_state.rtu_lock);
                rs485_set_tx(0);
                rc = modbus_send_raw_request(g_state.rtu_ctx, query + 6, rc - 6);
                if (rc >= 0) {
                    uint8_t rtu_resp[MODBUS_RTU_MAX_ADU_LENGTH];
                    rc = modbus_receive_confirmation(g_state.rtu_ctx, rtu_resp);
                    if (rc >= 0) {
                        /* 转换RTU到TCP */
                        response[4] = (rc + 1) >> 8;
                        response[5] = (rc + 1) & 0xFF;
                        memcpy(response + 8, rtu_resp + 1, rc - 1);
                        resp_len = 8 + rc - 1;
                        send(client_sock, response, resp_len, 0);
                    }
                }
                pthread_mutex_unlock(&g_state.rtu_lock);

                pthread_mutex_lock(&g_state.data_lock);
                goto skip_response;
            }

            /* 设置长度 */
            int data_len = resp_len - 6;
            response[4] = data_len >> 8;
            response[5] = data_len & 0xFF;

            send(client_sock, response, resp_len, 0);

        skip_response:
            pthread_mutex_unlock(&g_state.data_lock);

        } else if (rc == -1) {
            break;
        }
    }

    modbus_close(ctx);
    modbus_free(ctx);
    close(client_sock);

    __sync_sub_and_fetch(&g_state.client_count, 1);
    printf("[TCP] Client disconnected (total: %d)\n", g_state.client_count);

    return NULL;
}

/* TCP服务器线程 */
void *tcp_server_thread(void *arg)
{
    (void)arg;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    printf("[TCP] Server thread started\n");

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("[TCP] Socket creation failed");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(g_config.tcp_port);

    if (bind(server_sock, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("[TCP] Bind failed");
        close(server_sock);
        return NULL;
    }

    if (listen(server_sock, MAX_CONNECTIONS) < 0) {
        perror("[TCP] Listen failed");
        close(server_sock);
        return NULL;
    }

    printf("[TCP] Listening on port %d\n", g_config.tcp_port);

    while (g_state.running) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr,
                             &client_len);
        if (client_sock < 0) {
            if (errno == EINTR) continue;
            perror("[TCP] Accept failed");
            continue;
        }

        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = client_sock;

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, tcp_client_thread, sock_ptr);
        pthread_detach(client_thread);
    }

    close(server_sock);
    printf("[TCP] Server stopped\n");
    return NULL;
}

/* 初始化RTU */
int init_rtu(void)
{
    printf("[RTU] Initializing %s@%d\n", g_config.rtu_device, g_config.rtu_baud);

    g_state.rtu_ctx = modbus_new_rtu(g_config.rtu_device,
                                      g_config.rtu_baud,
                                      g_config.rtu_parity,
                                      g_config.rtu_data_bits,
                                      g_config.rtu_stop_bits);
    if (!g_state.rtu_ctx) {
        fprintf(stderr, "[RTU] Failed to create context\n");
        return -1;
    }

    modbus_set_error_recovery(g_state.rtu_ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);
    modbus_set_response_timeout(g_state.rtu_ctx, g_config.tcp_timeout, 0);

    /* 初始化RS485 GPIO */
    if (g_config.rs485_gpio >= 0) {
        g_state.gpio_chip = gpiod_chip_open_by_name("gpiochip0");
        if (g_state.gpio_chip) {
            g_state.rs485_line = gpiod_chip_get_line(g_state.gpio_chip,
                                                      g_config.rs485_gpio);
            if (g_state.rs485_line) {
                gpiod_line_request_output(g_state.rs485_line, "rs485_ctrl", 0);
                printf("[RTU] RS485 control on GPIO%d\n", g_config.rs485_gpio);
            }
        }
    }

    if (modbus_connect(g_state.rtu_ctx) == -1) {
        fprintf(stderr, "[RTU] Connection failed: %s\n",
                modbus_strerror(errno));
        modbus_free(g_state.rtu_ctx);
        return -1;
    }

    printf("[RTU] Connected\n");
    return 0;
}

/* 清理资源 */
void cleanup(void)
{
    g_state.running = 0;

    if (g_state.rs485_line) {
        gpiod_line_release(g_state.rs485_line);
    }
    if (g_state.gpio_chip) {
        gpiod_chip_close(g_state.gpio_chip);
    }
    if (g_state.rtu_ctx) {
        modbus_close(g_state.rtu_ctx);
        modbus_free(g_state.rtu_ctx);
    }
}

/* 主函数 */
int main(int argc, char *argv[])
{
    pthread_t rtu_tid, tcp_tid;
    int opt;

    printf("=== Modbus Gateway v%s ===\n\n", VERSION);

    /* 解析命令行 */
    while ((opt = getopt(argc, argv, "d:b:p:c:hv")) != -1) {
        switch (opt) {
        case 'd':
            strncpy(g_config.rtu_device, optarg, 255);
            break;
        case 'b':
            g_config.rtu_baud = atoi(optarg);
            break;
        case 'p':
            g_config.tcp_port = atoi(optarg);
            break;
        case 'c':
            strncpy(g_config.mapping_file, optarg, 255);
            break;
        case 'v':
            g_config.debug_level++;
            break;
        case 'h':
        default:
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  -d device   RTU device (default: %s)\n", g_config.rtu_device);
            printf("  -b baud     RTU baud rate (default: %d)\n", g_config.rtu_baud);
            printf("  -p port     TCP port (default: %d)\n", g_config.tcp_port);
            printf("  -c config   Config file\n");
            printf("  -v          Verbose (multiple for more)\n");
            printf("  -h          Show this help\n");
            return 0;
        }
    }

    /* 加载配置 */
    load_config(g_config.mapping_file);

    /* 初始化 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    pthread_mutex_init(&g_state.rtu_lock, NULL);
    pthread_mutex_init(&g_state.data_lock, NULL);

    g_state.running = 1;

    /* 初始化RTU */
    if (init_rtu() < 0) {
        return 1;
    }

    /* 创建线程 */
    pthread_create(&rtu_tid, NULL, rtu_thread, NULL);
    pthread_create(&tcp_tid, NULL, tcp_server_thread, NULL);

    /* 主循环 */
    printf("\nGateway running. Press Ctrl+C to stop.\n\n");

    while (g_state.running) {
        sleep(1);
    }

    printf("\nShutting down...\n");

    /* 等待线程 */
    pthread_join(rtu_tid, NULL);
    pthread_join(tcp_tid, NULL);

    cleanup();

    printf("Gateway stopped.\n");
    return 0;
}
```

### 2. Makefile

```makefile
# Makefile for Modbus Gateway
CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -D_GNU_SOURCE
LDFLAGS = -lmodbus -lgpiod -ljson-c -lpthread -lrt

TARGET = modbus_gateway

SRCS = modbus_gateway.c modbus_tcp.c modbus_rtu.c gpio_control.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SRCS)
 $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
 rm -f $(TARGET) *.o

install: $(TARGET)
 install -m 755 $(TARGET) /usr/local/bin/
 install -d /etc/modbus-gateway
 install -m 644 config.json /etc/modbus-gateway/
 install -m 644 modbus-gateway.service /etc/systemd/system/

# 交叉编译Raspberry Pi
rpi:
 aarch64-linux-gnu-gcc $(CFLAGS) -o $(TARGET)-arm64 $(SRCS) $(LDFLAGS)

deploy: rpi
 scp $(TARGET)-arm64 pi@raspberrypi:/usr/local/bin/modbus_gateway

.PHONY: all clean install rpi deploy
```

### 3. systemd服务

```ini
# /etc/systemd/system/modbus-gateway.service
[Unit]
Description=Modbus RTU/TCP Gateway
After=network.target

[Service]
Type=simple
User=root
Group=root
ExecStart=/usr/local/bin/modbus_gateway -c /etc/modbus-gateway/config.json
Restart=always
RestartSec=5

# 资源限制
LimitNOFILE=65536
LimitNPROC=4096

# 实时优先级
CPUSchedulingPolicy=fifo
CPUSchedulingPriority=50

[Install]
WantedBy=multi-user.target
```

### 4. 配置文件

```json
{
    "rtu": {
        "device": "/dev/ttyUSB0",
        "baud": 9600,
        "data_bits": 8,
        "parity": "N",
        "stop_bits": 1,
        "rs485_gpio": 23
    },
    "tcp": {
        "port": 502,
        "timeout": 5,
        "max_connections": 10
    },
    "slaves": [
        {"id": 1, "name": "Temperature Sensor"},
        {"id": 2, "name": "Pressure Sensor"},
        {"id": 3, "name": "Flow Meter"}
    ],
    "mapping": {
        "holding_registers": [
            {"local": 0, "remote": 0, "slave": 1, "count": 10},
            {"local": 100, "remote": 0, "slave": 2, "count": 10}
        ]
    },
    "debug": {
        "level": 1,
        "log_file": "/var/log/modbus-gateway.log"
    }
}
```

---

## 容器化部署

### 1. Dockerfile

```dockerfile
# Dockerfile for Modbus Gateway
FROM debian:bullseye-slim AS builder

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libmodbus-dev \
    libgpiod-dev \
    libjson-c-dev \
    && rm -rf /var/lib/apt/lists/*

# 复制源码
WORKDIR /build
COPY . .

# 构建
RUN make clean && make

# 运行时镜像
FROM debian:bullseye-slim

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libmodbus5 \
    libgpiod2 \
    libjson-c5 \
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# 创建设备访问组
RUN groupadd -r gpio && groupadd -r dialout

# 复制二进制文件
COPY --from=builder /build/modbus_gateway /usr/local/bin/

# 创建配置目录
RUN mkdir -p /etc/modbus-gateway
COPY config.json /etc/modbus-gateway/

# 创建用户
RUN useradd -r -s /bin/false -G gpio,dialout modbus

# 健康检查
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD pidof modbus_gateway || exit 1

EXPOSE 502

USER modbus

ENTRYPOINT ["/usr/local/bin/modbus_gateway"]
CMD ["-c", "/etc/modbus-gateway/config.json"]
```

### 2. Docker Compose

```yaml
# docker-compose.yml
version: '3.8'

services:
  modbus-gateway:
    build: .
    image: modbus-gateway:latest
    container_name: modbus-gateway
    restart: unless-stopped

    # 端口映射
    ports:
      - "502:502"

    # 设备访问
    devices:
      - "/dev/ttyUSB0:/dev/ttyUSB0"
      - "/dev/gpiomem:/dev/gpiomem"

    # 特权模式（用于GPIO访问）
    privileged: true

    # 或者使用capabilities
    # cap_add:
    #   - SYS_RAWIO

    # 卷挂载
    volumes:
      - ./config:/etc/modbus-gateway:ro
      - ./logs:/var/log

    # 网络
    networks:
      - modbus-net

    # 资源限制
    deploy:
      resources:
        limits:
          cpus: '0.5'
          memory: 128M
        reservations:
          cpus: '0.1'
          memory: 64M

    # 环境变量
    environment:
      - LOG_LEVEL=info
      - TZ=Asia/Shanghai

  # 监控服务
  prometheus:
    image: prom/prometheus:latest
    container_name: prometheus
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml:ro
      - prometheus-data:/prometheus
    ports:
      - "9090:9090"
    networks:
      - modbus-net

  grafana:
    image: grafana/grafana:latest
    container_name: grafana
    volumes:
      - grafana-data:/var/lib/grafana
      - ./grafana/dashboards:/etc/grafana/provisioning/dashboards:ro
    ports:
      - "3000:3000"
    networks:
      - modbus-net

networks:
  modbus-net:
    driver: bridge

volumes:
  prometheus-data:
  grafana-data:
```

### 3. Kubernetes部署

```yaml
# k8s-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: modbus-gateway
  namespace: industrial
spec:
  replicas: 1
  selector:
    matchLabels:
      app: modbus-gateway
  template:
    metadata:
      labels:
        app: modbus-gateway
    spec:
      hostNetwork: true  # 用于直接访问串口
      nodeSelector:
        hardware-type: rpi4
      containers:
      - name: gateway
        image: registry.local/modbus-gateway:latest
        imagePullPolicy: IfNotPresent
        securityContext:
          privileged: true
        resources:
          limits:
            cpu: "500m"
            memory: "128Mi"
          requests:
            cpu: "100m"
            memory: "64Mi"
        ports:
        - containerPort: 502
          name: modbus-tcp
        volumeMounts:
        - name: device-tty
          mountPath: /dev/ttyUSB0
        - name: config
          mountPath: /etc/modbus-gateway
          readOnly: true
        livenessProbe:
          exec:
            command:
            - pidof
            - modbus_gateway
          initialDelaySeconds: 10
          periodSeconds: 30
        readinessProbe:
          tcpSocket:
            port: 502
          initialDelaySeconds: 5
          periodSeconds: 10
      volumes:
      - name: device-tty
        hostPath:
          path: /dev/ttyUSB0
          type: CharDevice
      - name: config
        configMap:
          name: modbus-gateway-config
---
apiVersion: v1
kind: Service
metadata:
  name: modbus-gateway
  namespace: industrial
spec:
  selector:
    app: modbus-gateway
  ports:
  - port: 502
    targetPort: 502
    name: modbus-tcp
  type: LoadBalancer
---
apiVersion: v1
kind: ConfigMap
metadata:
  name: modbus-gateway-config
  namespace: industrial
data:
  config.json: |
    {
        "rtu": {
            "device": "/dev/ttyUSB0",
            "baud": 9600
        },
        "tcp": {
            "port": 502,
            "timeout": 5
        },
        "debug": {
            "level": 1
        }
    }
```

---

## 项目目录结构

```
RaspberryPi_Embedded_Linux/
├── kernel_drivers/
│   ├── Makefile
│   ├── hello_module.c
│   ├── char_device.c
│   ├── gpio_driver.c
│   ├── i2c_device.c
│   └── spi_device.c
├── userspace/
│   ├── gpio_control.c
│   ├── i2c_userspace.c
│   ├── spi_userspace.c
│   └── Makefile
├── modbus_gateway/
│   ├── modbus_gateway.c
│   ├── Makefile
│   ├── config.json
│   ├── modbus-gateway.service
│   └── Dockerfile
├── buildroot/
│   ├── configs/
│   │   └── rpi4_modbus_defconfig
│   └── package/
│       └── modbus-gateway/
│           ├── Config.in
│           └── modbus-gateway.mk
├── yocto/
│   └── meta-custom/
│       └── recipes-gateway/
│           └── modbus-gateway/
├── device_tree/
│   ├── bcm2711-rpi-4-b-custom.dts
│   └── rpi-custom-overlay.dts
├── scripts/
│   ├── apply_rt_patch.sh
│   └── optimize_boot.sh
├── k8s/
│   └── deployment.yaml
├── docker-compose.yml
└── README.md
```

---

## 总结

本文档涵盖了Raspberry Pi嵌入式Linux开发的完整技术栈：

1. **系统构建**：使用Buildroot和Yocto创建定制Linux系统
2. **设备树**：硬件配置和内核接口描述
3. **驱动开发**：内核模块、字符设备、GPIO/I2C/SPI驱动
4. **用户空间**：libgpiod、I2C/SPI设备访问
5. **实时性**：PREEMPT_RT补丁和实时编程
6. **系统优化**：启动时间和资源优化
7. **完整项目**：Modbus网关实现
8. **容器化**：Docker和Kubernetes部署

---

## 参考资源

- [Raspberry Pi Documentation](https://www.raspberrypi.org/documentation/)
- [Linux Device Drivers, 3rd Edition](https://lwn.net/Kernel/LDD3/)
- [Buildroot Manual](https://buildroot.org/downloads/manual/manual.html)
- [Yocto Project Documentation](https://docs.yoctoproject.org/)
- [PREEMPT_RT Wiki](https://wiki.linuxfoundation.org/realtime/start)
- [libgpiod Documentation](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/about/)


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
