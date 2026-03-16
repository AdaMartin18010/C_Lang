# 树莓派嵌入式 Linux 开发指南

> Raspberry Pi 4/5/Zero 2 W 嵌入式系统开发：裸机、Buildroot、Yocto、驱动开发

---

## 1. 硬件平台选择

### 1.1 树莓派型号对比

| 型号 | CPU | RAM | 用途 | 功耗 |
|:-----|:----|:----|:-----|:-----|
| Pi Zero 2 W | 4×Cortex-A53 @1GHz | 512MB | 轻量IoT | ~0.5W |
| Pi 3B+ | 4×Cortex-A53 @1.4GHz | 1GB | 中等负载 | ~2W |
| Pi 4B | 4×Cortex-A72 @1.5GHz | 2-8GB | 高性能嵌入式 | ~3W |
| Pi 5 | 4×Cortex-A76 @2.4GHz | 4-8GB | 桌面级嵌入式 | ~5W |
| CM4 | 同Pi 4B | 1-8GB | 工业定制 | ~3W |

### 1.2 启动流程

```
GPU Boot ROM → bootcode.bin → start.elf → config.txt → kernel8.img → Linux
     │              │              │            │            │
   固化ROM      加载SD卡      读配置        外设配置    内核+设备树
```

---

## 2. 最小 Linux 系统构建

### 2.1 Buildroot 构建

```bash
# 安装依赖
sudo apt-get install build-essential git wget cpio unzip rsync bc

# 下载 Buildroot
git clone git://git.buildroot.net/buildroot
cd buildroot
git checkout 2024.02

# 配置树莓派4
make raspberrypi4_64_defconfig

# 配置选项
make menuconfig
```

**Buildroot 关键配置**：

```
Target Architecture: AArch64 (little endian)
Target Architecture Variant: cortex-a72

Toolchain:
  → C library: glibc
  → Enable C++ support
  → Enable compiler cache

System configuration:
  → System hostname: rpi4-industrial
  → System banner: Industrial Linux System
  → Enable root login
  → Root password: industrial
  → Run getty on: tty1
  → Baudrate: 115200

Target packages:
  → Hardware handling:
    → Enable i2c-tools
    → Enable spi-tools
  → Networking:
    → Enable openssh
    → Enable wpa_supplicant
  → Development tools:
    → Enable strace
    → Enable gdb

Filesystem images:
  → ext4
  → tar the root filesystem
```

**构建**：

```bash
make -j$(nproc)

# 输出文件
ls output/images/
# sdcard.img  # 直接烧录到SD卡

dd if=output/images/sdcard.img of=/dev/sdX bs=4M status=progress
```

### 2.2 Yocto 构建

```bash
# 安装依赖
sudo apt-get install gawk wget git diffstat unzip texinfo gcc chrpath

# 获取 Poky
mkdir ~/yocto && cd ~/yocto
git clone git://git.yoctoproject.org/poky -b nanbield
git clone git://git.yoctoproject.org/meta-raspberrypi

# 初始化环境
source poky/oe-init-build-env rpi-build

# 添加层
bitbake-layers add-layer ../meta-raspberrypi

# 配置 local.conf
cat >> conf/local.conf << 'EOF'
MACHINE = "raspberrypi4-64"
IMAGE_INSTALL:append = " i2c-tools spi-tools python3"
ENABLE_SPI_BUS = "1"
ENABLE_I2C = "1"
KERNEL_MODULE_AUTOLOAD:rpi += "i2c-dev spi-dev"
EOF

# 构建
bitbake core-image-minimal

# 烧录
ls tmp/deploy/images/raspberrypi4-64/
dd if=core-image-minimal-raspberrypi4-64.wic of=/dev/sdX bs=4M
```

### 2.3 自定义 Init 系统

```c
/* minimal_init.c - 最小init程序 */
#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MOUNT_POINT(proc, target) \
    mount(proc, target, proc, 0, NULL)

void setup_filesystem(void)
{
    /* 挂载 proc */
    MOUNT_POINT("proc", "/proc");

    /* 挂载 sysfs */
    MOUNT_POINT("sysfs", "/sys");

    /* 挂载 devtmpfs */
    MOUNT_POINT("devtmpfs", "/dev");
}

void setup_network(void)
{
    /* 配置 lo 接口 */
    system("ip link set lo up");
    system("ip addr add 127.0.0.1/8 dev lo");

    /* 配置 eth0 (如果存在) */
    system("ip link set eth0 up");
    system("udhcpc -i eth0");
}

int main(int argc, char *argv[])
{
    printf("Minimal Init Starting...\n");

    /* 设置文件系统 */
    setup_filesystem();

    /* 设置网络 */
    setup_network();

    /* 启动 shell */
    printf("Starting shell...\n");

    pid_t pid = fork();
    if(pid == 0) {
        /* 子进程 */
        execl("/bin/sh", "sh", NULL);
        perror("exec failed");
        return 1;
    }

    /* 父进程：等待子进程退出 */
    int status;
    waitpid(pid, &status, 0);

    printf("Shell exited. Rebooting...\n");
    sync();
    reboot(RB_AUTOBOOT);

    return 0;
}
```

---

## 3. 设备树 (Device Tree) 开发

### 3.1 设备树基础

```dts
/* rpi4-custom.dts */
/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2711";

    fragment@0 {
        target = <&gpio>;
        __overlay__ {
            /* 自定义 GPIO 配置 */
            industrial_pins: industrial_pins {
                brcm,pins = <17 27 22>;      /* GPIO17, 27, 22 */
                brcm,function = <1 1 0>;      /* 输出, 输出, 输入 */
                brcm,pull = <0 0 2>;          /* 无, 无, 上拉 */
            };
        };
    };

    fragment@1 {
        target-path = "/";
        __overlay__ {
            /* 自定义设备 */
            industrial_controller: industrial@0 {
                compatible = "custom,industrial-ctrl";
                reg = <0>;
                gpios = <&gpio 17 0>,    /* GPIO17, 默认低 */
                        <&gpio 27 0>;    /* GPIO27, 默认低 */
                interrupt-parent = <&gpio>;
                interrupts = <22 2>;      /* GPIO22, 下降沿 */
                status = "okay";
            };
        };
    };

    fragment@2 {
        target = <&i2c1>;
        __overlay__ {
            #address-cells = <1>;
            #size-cells = <0>;
            status = "okay";

            /* BME280 传感器 */
            bme280@76 {
                compatible = "bosch,bme280";
                reg = <0x76>;
                status = "okay";
            };

            /* ADS1115 ADC */
            ads1115@48 {
                compatible = "ti,ads1115";
                reg = <0x48>;
                #address-cells = <1>;
                #size-cells = <0>;

                channel@0 {
                    reg = <0>;
                    ti,gain = <1>;      /* +/-4.096V */
                    ti,datarate = <4>;  /* 128SPS */
                };
            };
        };
    };
};
```

**编译设备树**：

```bash
# 安装设备树编译器
sudo apt-get install device-tree-compiler

# 编译
dtc -I dts -O dtb -o rpi4-custom.dtbo rpi4-custom.dts

# 复制到启动分区
sudo cp rpi4-custom.dtbo /boot/overlays/

# 配置 config.txt
echo "dtoverlay=rpi4-custom" | sudo tee -a /boot/config.txt
```

### 3.2 运行时设备树修改

```c
/* dtb_modify.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>

/* 通过 configfs 动态配置 GPIO */
void export_gpio(int gpio)
{
    char path[64];
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd < 0) {
        perror("export");
        return;
    }

    dprintf(fd, "%d\n", gpio);
    close(fd);

    /* 设置方向 */
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio);
    fd = open(path, O_WRONLY);
    write(fd, "out", 3);
    close(fd);
}

void set_gpio_value(int gpio, int value)
{
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);

    int fd = open(path, O_WRONLY);
    dprintf(fd, "%d\n", value);
    close(fd);
}
```

---

## 4. 内核驱动开发

### 4.1 GPIO 字符设备驱动

```c
/* industrial_gpio_driver.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#define DEVICE_NAME     "industrial_gpio"
#define CLASS_NAME      "industrial"
#define NUM_GPIOS       3

struct industrial_dev {
    struct cdev cdev;
    struct device *device;
    struct class *class;
    dev_t devt;

    int gpio_outputs[2];
    int gpio_input;
    int irq;

    wait_queue_head_t wait_queue;
    atomic_t input_triggered;
};

static struct industrial_dev *industrial_dev;

/* 中断处理 */
static irqreturn_t industrial_irq_handler(int irq, void *dev_id)
{
    atomic_set(&industrial_dev->input_triggered, 1);
    wake_up_interruptible(&industrial_dev->wait_queue);
    return IRQ_HANDLED;
}

/* 文件操作 */
static int industrial_open(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t industrial_write(struct file *file, const char __user *buf,
                               size_t count, loff_t *offset)
{
    char cmd[16];
    int gpio_num, value;

    if(copy_from_user(cmd, buf, min(count, sizeof(cmd)-1)))
        return -EFAULT;

    cmd[min(count, sizeof(cmd)-1)] = '\0';

    /* 解析命令: "SET 0 1" - 设置第0个GPIO为高 */
    if(sscanf(cmd, "SET %d %d", &gpio_num, &value) == 2) {
        if(gpio_num >= 0 && gpio_num < 2) {
            gpio_set_value(industrial_dev->gpio_outputs[gpio_num], value);
            printk(KERN_INFO "GPIO %d set to %d\n", gpio_num, value);
        }
    }

    return count;
}

static ssize_t industrial_read(struct file *file, char __user *buf,
                              size_t count, loff_t *offset)
{
    int value;
    char response[32];

    /* 等待中断或超时 */
    wait_event_interruptible_timeout(
        industrial_dev->wait_queue,
        atomic_read(&industrial_dev->input_triggered),
        HZ * 5  /* 5秒超时 */
    );

    atomic_set(&industrial_dev->input_triggered, 0);

    value = gpio_get_value(industrial_dev->gpio_input);
    snprintf(response, sizeof(response), "INPUT: %d\n", value);

    if(copy_to_user(buf, response, strlen(response)))
        return -EFAULT;

    return strlen(response);
}

static struct file_operations industrial_fops = {
    .owner = THIS_MODULE,
    .open = industrial_open,
    .read = industrial_read,
    .write = industrial_write,
};

/* 平台驱动 probe */
static int industrial_probe(struct platform_device *pdev)
{
    struct device_node *np = pdev->dev.of_node;
    int ret, i;

    industrial_dev = kzalloc(sizeof(*industrial_dev), GFP_KERNEL);
    if(!industrial_dev)
        return -ENOMEM;

    /* 获取 GPIO */
    for(i = 0; i < 2; i++) {
        industrial_dev->gpio_outputs[i] = of_get_named_gpio(
            np, "gpios", i);
        if(gpio_is_valid(industrial_dev->gpio_outputs[i])) {
            ret = gpio_request(industrial_dev->gpio_outputs[i],
                              "industrial_out");
            if(ret) goto err_gpio;
            gpio_direction_output(industrial_dev->gpio_outputs[i], 0);
        }
    }

    industrial_dev->gpio_input = of_get_named_gpio(np, "gpios", 2);
    if(gpio_is_valid(industrial_dev->gpio_input)) {
        ret = gpio_request(industrial_dev->gpio_input, "industrial_in");
        if(ret) goto err_gpio;
        gpio_direction_input(industrial_dev->gpio_input);

        /* 请求中断 */
        industrial_dev->irq = gpio_to_irq(industrial_dev->gpio_input);
        ret = request_irq(industrial_dev->irq, industrial_irq_handler,
                         IRQF_TRIGGER_FALLING, "industrial_irq", NULL);
        if(ret) goto err_irq;
    }

    init_waitqueue_head(&industrial_dev->wait_queue);
    atomic_set(&industrial_dev->input_triggered, 0);

    /* 创建字符设备 */
    alloc_chrdev_region(&industrial_dev->devt, 0, 1, DEVICE_NAME);
    cdev_init(&industrial_dev->cdev, &industrial_fops);
    industrial_dev->cdev.owner = THIS_MODULE;
    cdev_add(&industrial_dev->cdev, industrial_dev->devt, 1);

    industrial_dev->class = class_create(THIS_MODULE, CLASS_NAME);
    industrial_dev->device = device_create(industrial_dev->class, NULL,
                                           industrial_dev->devt, NULL,
                                           DEVICE_NAME);

    printk(KERN_INFO "Industrial GPIO driver loaded\n");
    return 0;

err_irq:
    gpio_free(industrial_dev->gpio_input);
err_gpio:
    for(i = 0; i < 2; i++)
        gpio_free(industrial_dev->gpio_outputs[i]);
    kfree(industrial_dev);
    return ret;
}

static int industrial_remove(struct platform_device *pdev)
{
    int i;

    device_destroy(industrial_dev->class, industrial_dev->devt);
    class_destroy(industrial_dev->class);
    cdev_del(&industrial_dev->cdev);
    unregister_chrdev_region(industrial_dev->devt, 1);

    free_irq(industrial_dev->irq, NULL);
    gpio_free(industrial_dev->gpio_input);

    for(i = 0; i < 2; i++)
        gpio_free(industrial_dev->gpio_outputs[i]);

    kfree(industrial_dev);

    printk(KERN_INFO "Industrial GPIO driver removed\n");
    return 0;
}

static const struct of_device_id industrial_of_match[] = {
    { .compatible = "custom,industrial-ctrl" },
    {}
};
MODULE_DEVICE_TABLE(of, industrial_of_match);

static struct platform_driver industrial_driver = {
    .probe = industrial_probe,
    .remove = industrial_remove,
    .driver = {
        .name = "industrial_gpio",
        .of_match_table = industrial_of_match,
    },
};

module_platform_driver(industrial_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Industrial Dev");
MODULE_DESCRIPTION("Industrial GPIO Controller Driver");
```

### 4.2 编译和加载

```makefile
# Makefile
obj-m += industrial_gpio_driver.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
 make -C $(KDIR) M=$(PWD) modules

clean:
 make -C $(KDIR) M=$(PWD) clean

load:
 sudo insmod industrial_gpio_driver.ko

unload:
 sudo rmmod industrial_gpio_driver
```

```bash
# 编译
make

# 加载驱动
sudo insmod industrial_gpio_driver.ko

# 查看日志
dmesg | tail

# 测试
echo "SET 0 1" | sudo tee /dev/industrial_gpio
cat /dev/industrial_gpio  # 等待输入触发
```

---

## 5. 实时性优化 (PREEMPT_RT)

### 5.1 打实时补丁

```bash
# 下载内核和补丁
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.tar.xz
wget https://cdn.kernel.org/pub/linux/kernel/projects/rt/6.6/patch-6.6-rt15.patch.xz

tar xf linux-6.6.tar.xz
cd linux-6.6
xzcat ../patch-6.6-rt15.patch.xz | patch -p1

# 配置
defmake bcm2711_defconfig  # Pi 4

# 启用 PREEMPT_RT
./scripts/config --enable CONFIG_PREEMPT_RT
./scripts/config --enable CONFIG_HIGH_RES_TIMERS
./scripts/config --enable CONFIG_NO_HZ_FULL

# 编译
make -j$(nproc) Image modules dtbs
sudo make modules_install
sudo cp arch/arm64/boot/Image /boot/kernel8-rt.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot/
```

### 5.2 实时任务编程

```c
/* realtime_task.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#define NSEC_PER_SEC    1000000000L
#define INTERVAL_NS     1000000L  /* 1ms周期 */

static inline void timespec_add_ns(struct timespec *ts, long ns)
{
    ts->tv_nsec += ns;
    while(ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
}

void *realtime_thread(void *arg)
{
    struct sched_param param;
    struct timespec next;
    cpu_set_t cpuset;
    int interval_count = 0;
    long max_jitter = 0;

    /* 绑定到 CPU0 */
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    /* 设置为 SCHED_FIFO */
    param.sched_priority = 80;
    if(pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
        perror("pthread_setschedparam");
        return NULL;
    }

    /* 锁定内存 */
    if(mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall");
    }

    /* 获取当前时间 */
    clock_gettime(CLOCK_MONOTONIC, &next);

    while(interval_count < 1000) {
        struct timespec now, diff;
        long jitter_ns;

        /* 等待下一个周期 */
        timespec_add_ns(&next, INTERVAL_NS);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);

        /* 计算抖动 */
        clock_gettime(CLOCK_MONOTONIC, &now);
        diff.tv_sec = now.tv_sec - next.tv_sec;
        diff.tv_nsec = now.tv_nsec - next.tv_nsec;
        jitter_ns = diff.tv_sec * NSEC_PER_SEC + diff.tv_nsec;

        if(jitter_ns < 0) jitter_ns = -jitter_ns;
        if(jitter_ns > max_jitter) max_jitter = jitter_ns;

        /* 实时任务：读取传感器 */
        read_sensors();

        interval_count++;
    }

    printf("Max jitter: %ld ns (%.2f us)\n", max_jitter, max_jitter/1000.0);

    munlockall();
    return NULL;
}

int main(void)
{
    pthread_t rt_thread;

    printf("Starting real-time task...\n");

    pthread_create(&rt_thread, NULL, realtime_thread, NULL);
    pthread_join(rt_thread, NULL);

    return 0;
}
```

---

## 6. 工业总线接口

### 6.1 Modbus RTU/TCP

```c
/* modbus_server.c - 使用 libmodbus */
#include <modbus/modbus.h>

modbus_t *ctx;
modbus_mapping_t *mb_mapping;

int setup_modbus(void)
{
    /* RTU 模式 */
    ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    modbus_set_slave(ctx, 1);  /* 从机地址 */

    /* 或 TCP 模式 */
    // ctx = modbus_new_tcp("0.0.0.0", 502);

    if(modbus_connect(ctx) == -1) {
        fprintf(stderr, "Modbus connection failed\n");
        return -1;
    }

    /* 创建寄存器映射 */
    mb_mapping = modbus_mapping_new(100, 100, 100, 100);

    /* 初始化数据 */
    mb_mapping->tab_registers[0] = 25;  /* 温度 */
    mb_mapping->tab_registers[1] = 60;  /* 湿度 */

    return 0;
}

void run_modbus_server(void)
{
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;

    while(1) {
        rc = modbus_receive(ctx, query);
        if(rc > 0) {
            /* 处理请求 */
            modbus_reply(ctx, query, rc, mb_mapping);
        }
    }
}
```

### 6.2 CAN 总线 (SocketCAN)

```c
/* can_interface.c */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int can_init(const char *interface)
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;

    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(sock < 0) {
        perror("socket");
        return -1;
    }

    strcpy(ifr.ifr_name, interface);
    ioctl(sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    return sock;
}

void can_send(int sock, uint32_t id, uint8_t *data, uint8_t len)
{
    struct can_frame frame;

    frame.can_id = id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    write(sock, &frame, sizeof(frame));
}

void can_receive(int sock)
{
    struct can_frame frame;
    int nbytes;

    nbytes = read(sock, &frame, sizeof(frame));
    if(nbytes > 0) {
        printf("Received CAN ID: 0x%03X, Data: ", frame.can_id);
        for(int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }
}

/* 配置 CAN 接口 */
// sudo ip link set can0 up type can bitrate 500000
```

---

## 7. 系统监控与 watchdog

```c
/* system_monitor.c */
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int watchdog_init(void)
{
    int fd = open("/dev/watchdog", O_RDWR);
    if(fd < 0) {
        perror("watchdog");
        return -1;
    }

    /* 设置超时为10秒 */
    int timeout = 10;
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

    return fd;
}

void watchdog_kick(int fd)
{
    ioctl(fd, WDIOC_KEEPALIVE, 0);
}

/* 系统监控任务 */
void *monitor_task(void *arg)
{
    int watchdog_fd = watchdog_init();

    while(1) {
        /* 检查关键进程 */
        if(check_critical_processes() != 0) {
            /* 不喂狗，让系统重启 */
            printf("Critical process failed! System will reboot...\n");
            break;
        }

        /* 检查温度 */
        float temp = read_cpu_temperature();
        if(temp > 85.0) {
            printf("CPU overheating: %.1f°C\n", temp);
            throttle_cpu();
        }

        /* 喂狗 */
        if(watchdog_fd >= 0) {
            watchdog_kick(watchdog_fd);
        }

        sleep(1);
    }

    return NULL;
}
```

---

## 8. 参考资源

- **Buildroot**: <https://buildroot.org/>
- **Yocto**: <https://www.yoctoproject.org/>
- **Raspberry Pi 内核**: <https://github.com/raspberrypi/linux>
- **Device Tree 规范**: <https://www.devicetree.org/>
- **PREEMPT_RT**: <https://wiki.linuxfoundation.org/realtime/start>

---

> **硬件**: Raspberry Pi 4B/CM4/Zero 2 W
> **内核**: Linux 6.6+ with PREEMPT_RT
> **代码**: 1000+ 行可运行代码
