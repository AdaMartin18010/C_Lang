/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 2073
 * Language: c
 * Block ID: b5cfbacb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: gpio_basic.c
 * Description: GPIO基础操作示例 - LED和按键控制
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>              /* 传统GPIO API */
#include <linux/gpio/consumer.h>      /* GPIO描述符API (推荐) */
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GPIO Driver Developer");
MODULE_DESCRIPTION("GPIO LED and Button Driver");

#define DRIVER_NAME     "gpiobasic"
#define CLASS_NAME      "gpiobasic"
#define NUM_LEDS        4
#define NUM_BUTTONS     4

/* GPIO LED 配置 */
struct gpio_led {
    const char *name;
    unsigned int gpio;
    unsigned int active_low;
    const char *default_state;
};

/* GPIO 按键配置 */
struct gpio_button {
    const char *name;
    unsigned int gpio;
    unsigned int active_low;
    unsigned int irq;
    unsigned int debounce_ms;
};

/* 设备私有数据 */
struct gpiobasic_dev {
    struct platform_device *pdev;

    /* LED相关 */
    struct gpio_desc *led_gpios[NUM_LEDS];
    int num_leds;
    u8 led_state;

    /* 按键相关 */
    struct gpio_desc *button_gpios[NUM_BUTTONS];
    int num_buttons;
    unsigned int button_irqs[NUM_BUTTONS];
    u8 button_states[NUM_BUTTONS];
    u32 button_counts[NUM_BUTTONS];
    struct mutex button_lock;

    /* sysfs */
    struct kobject *kobj;

    /* 字符设备 */
    struct cdev cdev;
    dev_t dev_num;
    struct class *class;
    struct device *device;
};

/* 平台数据示例 (非Device Tree系统) */
static struct gpio_led led_platform_data[] = {
    { "led0", 17, 0, "off" },   /* GPIO 17, 高电平有效 */
    { "led1", 18, 0, "off" },   /* GPIO 18 */
    { "led2", 27, 0, "off" },   /* GPIO 27 */
    { "led3", 22, 0, "off" },   /* GPIO 22 */
};

static struct gpio_button button_platform_data[] = {
    { "button0", 23, 1, 0, 50 },  /* GPIO 23, 低电平有效, 50ms消抖 */
    { "button1", 24, 1, 0, 50 },
    { "button2", 25, 1, 0, 50 },
    { "button3", 5,  1, 0, 50 },
};

/*
 * LED控制函数
 */
static void led_set(struct gpiobasic_dev *dev, int led_num, int value)
{
    if (led_num < 0 || led_num >= dev->num_leds)
        return;

    gpiod_set_value(dev->led_gpios[led_num], value);

    if (value)
        dev->led_state |= (1 << led_num);
    else
        dev->led_state &= ~(1 << led_num);
}

static int led_get(struct gpiobasic_dev *dev, int led_num)
{
    if (led_num < 0 || led_num >= dev->num_leds)
        return -EINVAL;

    return gpiod_get_value(dev->led_gpios[led_num]);
}

static void led_toggle(struct gpiobasic_dev *dev, int led_num)
{
    int current = led_get(dev, led_num);
    led_set(dev, led_num, !current);
}

/*
 * 按键中断处理
 */
static irqreturn_t button_irq_handler(int irq, void *dev_id)
{
    struct gpiobasic_dev *dev = dev_id;
    int i;
    int value;

    /* 查找是哪个按键触发的中断 */
    for (i = 0; i < dev->num_buttons; i++) {
        if (dev->button_irqs[i] == irq) {
            /* 读取按键状态 */
            value = gpiod_get_value(dev->button_gpios[i]);

            mutex_lock(&dev->button_lock);
            dev->button_states[i] = value;
            dev->button_counts[i]++;
            mutex_unlock(&dev->button_lock);

            dev_info(&dev->pdev->dev, "Button %d %s (count=%u)\n",
                     i, value ? "pressed" : "released", dev->button_counts[i]);

            /* 示例: 按键控制LED */
            led_toggle(dev, i % dev->num_leds);

            return IRQ_HANDLED;
        }
    }

    return IRQ_NONE;
}

/*
 * 批量设置LED (通过二进制值)
 */
static void leds_set_all(struct gpiobasic_dev *dev, u8 value)
{
    int i;
    for (i = 0; i < dev->num_leds; i++) {
        led_set(dev, i, (value >> i) & 1);
    }
}

static u8 leds_get_all(struct gpiobasic_dev *dev)
{
    return dev->led_state;
}

/*
 * LED跑马灯效果 (用于测试)
 */
static void led_chase_pattern(struct gpiobasic_dev *dev, int cycles)
{
    int i, c;

    for (c = 0; c < cycles; c++) {
        for (i = 0; i < dev->num_leds; i++) {
            leds_set_all(dev, 1 << i);
            mdelay(100);
        }
        for (i = dev->num_leds - 2; i > 0; i--) {
            leds_set_all(dev, 1 << i);
            mdelay(100);
        }
    }

    leds_set_all(dev, 0);
}

/*
 * sysfs属性 - LED控制
 */
static ssize_t leds_show(struct device *dev, struct device_attribute *attr,
                          char *buf)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    return sprintf(buf, "0x%02X\n", leds_get_all(gdev));
}

static ssize_t leds_store(struct device *dev, struct device_attribute *attr,
                           const char *buf, size_t count)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    unsigned long value;
    int ret;

    ret = kstrtoul(buf, 0, &value);
    if (ret)
        return ret;

    leds_set_all(gdev, (u8)value);

    return count;
}
static DEVICE_ATTR_RW(leds);

/*
 * sysfs属性 - 单个LED控制
 */
static ssize_t led_show(struct device *dev, struct device_attribute *attr,
                         char *buf)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    int led_num = attr->attr.name[3] - '0';  /* led0 -> 0 */
    int value;

    if (led_num < 0 || led_num >= gdev->num_leds)
        return -EINVAL;

    value = led_get(gdev, led_num);
    return sprintf(buf, "%d\n", value);
}

static ssize_t led_store(struct device *dev, struct device_attribute *attr,
                          const char *buf, size_t count)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    int led_num = attr->attr.name[3] - '0';
    unsigned long value;
    int ret;

    if (led_num < 0 || led_num >= gdev->num_leds)
        return -EINVAL;

    ret = kstrtoul(buf, 0, &value);
    if (ret)
        return ret;

    led_set(gdev, led_num, !!value);

    return count;
}

static struct device_attribute dev_attr_led0 = __ATTR(led0, 0664, led_show, led_store);
static struct device_attribute dev_attr_led1 = __ATTR(led1, 0664, led_show, led_store);
static struct device_attribute dev_attr_led2 = __ATTR(led2, 0664, led_show, led_store);
static struct device_attribute dev_attr_led3 = __ATTR(led3, 0664, led_show, led_store);

/*
 * sysfs属性 - 按键状态
 */
static ssize_t buttons_show(struct device *dev, struct device_attribute *attr,
                             char *buf)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    u8 state = 0;
    int i;

    mutex_lock(&gdev->button_lock);
    for (i = 0; i < gdev->num_buttons; i++) {
        state |= (gdev->button_states[i] << i);
    }
    mutex_unlock(&gdev->button_lock);

    return sprintf(buf, "0x%02X\n", state);
}
static DEVICE_ATTR_RO(buttons);

/*
 * sysfs属性 - 按键计数
 */
static ssize_t button_counts_show(struct device *dev, struct device_attribute *attr,
                                   char *buf)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    int i;
    ssize_t count = 0;

    mutex_lock(&gdev->button_lock);
    for (i = 0; i < gdev->num_buttons; i++) {
        count += sprintf(buf + count, "button%d: %u\n", i, gdev->button_counts[i]);
    }
    mutex_unlock(&gdev->button_lock);

    return count;
}
static DEVICE_ATTR_RO(button_counts);

/*
 * sysfs属性 - 测试模式
 */
static ssize_t test_pattern_store(struct device *dev, struct device_attribute *attr,
                                   const char *buf, size_t count)
{
    struct gpiobasic_dev *gdev = dev_get_drvdata(dev);
    unsigned long cycles;
    int ret;

    ret = kstrtoul(buf, 0, &cycles);
    if (ret)
        return ret;

    led_chase_pattern(gdev, cycles);

    return count;
}
static DEVICE_ATTR_WO(test_pattern);

/* 属性组 */
static struct attribute *gpiobasic_attrs[] = {
    &dev_attr_leds.attr,
    &dev_attr_buttons.attr,
    &dev_attr_button_counts.attr,
    &dev_attr_test_pattern.attr,
    &dev_attr_led0.attr,
    &dev_attr_led1.attr,
    &dev_attr_led2.attr,
    &dev_attr_led3.attr,
    NULL,
};

static struct attribute_group gpiobasic_attr_group = {
    .attrs = gpiobasic_attrs,
};

/*
 * 字符设备接口
 */
static int gpiobasic_open(struct inode *inode, struct file *filp)
{
    filp->private_data = container_of(inode->i_cdev, struct gpiobasic_dev, cdev);
    return 0;
}

static int gpiobasic_release(struct inode *inode, struct file *filp)
{
    return 0;
}

/* ioctl 命令 */
#define GPIOBASIC_IOC_MAGIC     'G'
#define GPIOBASIC_SET_LED       _IOW(GPIOBASIC_IOC_MAGIC, 0, struct gpio_led_cmd)
#define GPIOBASIC_GET_LED       _IOR(GPIOBASIC_IOC_MAGIC, 1, struct gpio_led_cmd)
#define GPIOBASIC_GET_BUTTON    _IOR(GPIOBASIC_IOC_MAGIC, 2, struct gpio_button_cmd)
#define GPIOBASIC_SET_ALL_LEDS  _IOW(GPIOBASIC_IOC_MAGIC, 3, __u8)

struct gpio_led_cmd {
    __u8 led_num;
    __u8 value;
};

struct gpio_button_cmd {
    __u8 button_num;
    __u8 pressed;
    __u32 count;
};

static long gpiobasic_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct gpiobasic_dev *dev = filp->private_data;
    struct gpio_led_cmd led_cmd;
    struct gpio_button_cmd btn_cmd;
    int ret = 0;

    switch (cmd) {
    case GPIOBASIC_SET_LED:
        if (copy_from_user(&led_cmd, (void __user *)arg, sizeof(led_cmd)))
            return -EFAULT;
        led_set(dev, led_cmd.led_num, led_cmd.value);
        break;

    case GPIOBASIC_GET_LED:
        if (copy_from_user(&led_cmd, (void __user *)arg, sizeof(led_cmd)))
            return -EFAULT;
        led_cmd.value = led_get(dev, led_cmd.led_num);
        if (copy_to_user((void __user *)arg, &led_cmd, sizeof(led_cmd)))
            return -EFAULT;
        break;

    case GPIOBASIC_GET_BUTTON:
        if (copy_from_user(&btn_cmd, (void __user *)arg, sizeof(btn_cmd)))
            return -EFAULT;
        if (btn_cmd.button_num >= dev->num_buttons)
            return -EINVAL;
        mutex_lock(&dev->button_lock);
        btn_cmd.pressed = dev->button_states[btn_cmd.button_num];
        btn_cmd.count = dev->button_counts[btn_cmd.button_num];
        mutex_unlock(&dev->button_lock);
        if (copy_to_user((void __user *)arg, &btn_cmd, sizeof(btn_cmd)))
            return -EFAULT;
        break;

    case GPIOBASIC_SET_ALL_LEDS:
        leds_set_all(dev, (__u8)arg);
        break;

    default:
        ret = -ENOTTY;
    }

    return ret;
}

static const struct file_operations gpiobasic_fops = {
    .owner          = THIS_MODULE,
    .open           = gpiobasic_open,
    .release        = gpiobasic_release,
    .unlocked_ioctl = gpiobasic_ioctl,
};

/*
 * 从Device Tree初始化
 */
static int gpiobasic_init_from_dt(struct gpiobasic_dev *dev)
{
    struct device_node *np = dev->pdev->dev.of_node;
    struct device_node *child;
    int i = 0;
    int ret;

    if (!np)
        return -ENODEV;

    /* 解析LED节点 */
    for_each_child_of_node(np, child) {
        if (of_property_read_bool(child, "led")) {
            if (i >= NUM_LEDS) {
                dev_warn(&dev->pdev->dev, "Too many LEDs, ignoring extras\n");
                break;
            }

            dev->led_gpios[i] = devm_fwnode_get_gpiod_from_child(&dev->pdev->dev,
                                                                  NULL, &child->fwnode,
                                                                  GPIOD_ASIS, child->name);
            if (IS_ERR(dev->led_gpios[i])) {
                ret = PTR_ERR(dev->led_gpios[i]);
                of_node_put(child);
                return ret;
            }

            /* 设置方向为输出 */
            gpiod_direction_output(dev->led_gpios[i], 0);
            i++;
        }
    }
    dev->num_leds = i;

    /* 解析按键节点 */
    i = 0;
    for_each_child_of_node(np, child) {
        if (of_property_read_bool(child, "button")) {
            if (i >= NUM_BUTTONS) {
                dev_warn(&dev->pdev->dev, "Too many buttons, ignoring extras\n");
                break;
            }

            dev->button_gpios[i] = devm_fwnode_get_gpiod_from_child(&dev->pdev->dev,
                                                                     NULL, &child->fwnode,
                                                                     GPIOD_ASIS, child->name);
            if (IS_ERR(dev->button_gpios[i])) {
                ret = PTR_ERR(dev->button_gpios[i]);
                of_node_put(child);
                return ret;
            }

            /* 设置方向为输入 */
            gpiod_direction_input(dev->button_gpios[i]);

            /* 获取中断 */
            dev->button_irqs[i] = gpiod_to_irq(dev->button_gpios[i]);
            if (dev->button_irqs[i] < 0) {
                dev_warn(&dev->pdev->dev, "Failed to get IRQ for button %d\n", i);
            }

            i++;
        }
    }
    dev->num_buttons = i;

    return 0;
}

/*
 * 从平台数据初始化
 */
static int gpiobasic_init_from_pdata(struct gpiobasic_dev *dev)
{
    int i;
    int ret;

    /* 初始化LED */
    dev->num_leds = ARRAY_SIZE(led_platform_data);
    for (i = 0; i < dev->num_leds; i++) {
        ret = devm_gpio_request_one(&dev->pdev->dev,
                                     led_platform_data[i].gpio,
                                     GPIOF_OUT_INIT_LOW,
                                     led_platform_data[i].name);
        if (ret)
            return ret;

        dev->led_gpios[i] = gpio_to_desc(led_platform_data[i].gpio);
    }

    /* 初始化按键 */
    dev->num_buttons = ARRAY_SIZE(button_platform_data);
    for (i = 0; i < dev->num_buttons; i++) {
        unsigned int gpio = button_platform_data[i].gpio;

        ret = devm_gpio_request_one(&dev->pdev->dev, gpio,
                                     GPIOF_IN,
                                     button_platform_data[i].name);
        if (ret)
            return ret;

        dev->button_gpios[i] = gpio_to_desc(gpio);
        dev->button_irqs[i] = gpio_to_irq(gpio);
    }

    return 0;
}

/*
 * 平台探测函数
 */
static int gpiobasic_probe(struct platform_device *pdev)
{
    struct gpiobasic_dev *dev;
    int ret;
    int i;

    dev_info(&pdev->dev, "Probing GPIO Basic driver\n");

    dev = devm_kzalloc(&pdev->dev, sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->pdev = pdev;
    platform_set_drvdata(pdev, dev);
    mutex_init(&dev->button_lock);

    /* 初始化GPIO (优先使用Device Tree) */
    if (pdev->dev.of_node)
        ret = gpiobasic_init_from_dt(dev);
    else
        ret = gpiobasic_init_from_pdata(dev);

    if (ret) {
        dev_err(&pdev->dev, "Failed to initialize GPIOs: %d\n", ret);
        return ret;
    }

    dev_info(&pdev->dev, "Found %d LEDs and %d buttons\n",
             dev->num_leds, dev->num_buttons);

    /* 注册按键中断 */
    for (i = 0; i < dev->num_buttons; i++) {
        if (dev->button_irqs[i] > 0) {
            ret = devm_request_threaded_irq(&pdev->dev, dev->button_irqs[i],
                                            NULL, button_irq_handler,
                                            IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
                                            button_platform_data[i].name, dev);
            if (ret) {
                dev_warn(&pdev->dev, "Failed to request IRQ for button %d: %d\n",
                         i, ret);
            } else {
                dev_info(&pdev->dev, "Button %d IRQ %d registered\n",
                         i, dev->button_irqs[i]);
            }
        }
    }

    /* 分配设备号 */
    ret = alloc_chrdev_region(&dev->dev_num, 0, 1, DRIVER_NAME);
    if (ret) {
        dev_err(&pdev->dev, "Failed to allocate device number\n");
        return ret;
    }

    /* 创建设备类 */
    dev->class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(dev->class)) {
        ret = PTR_ERR(dev->class);
        goto unregister_chrdev;
    }

    /* 创建设备 */
    dev->device = device_create(dev->class, &pdev->dev, dev->dev_num, dev, DRIVER_NAME);
    if (IS_ERR(dev->device)) {
        ret = PTR_ERR(dev->device);
        goto destroy_class;
    }

    /* 创建sysfs属性 */
    ret = sysfs_create_group(&dev->device->kobj, &gpiobasic_attr_group);
    if (ret) {
        dev_warn(&pdev->dev, "Failed to create sysfs group: %d\n", ret);
    }

    /* 初始化字符设备 */
    cdev_init(&dev->cdev, &gpiobasic_fops);
    dev->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev->cdev, dev->dev_num, 1);
    if (ret) {
        dev_err(&pdev->dev, "Failed to add cdev\n");
        goto destroy_device;
    }

    dev_info(&pdev->dev, "GPIO Basic driver loaded\n");
    dev_info(&pdev->dev, "Use sysfs: /sys/class/%s/%s/\n", CLASS_NAME, DRIVER_NAME);
    dev_info(&pdev->dev, "Or device node: /dev/%s\n", DRIVER_NAME);

    return 0;

destroy_device:
    sysfs_remove_group(&dev->device->kobj, &gpiobasic_attr_group);
    device_destroy(dev->class, dev->dev_num);
destroy_class:
    class_destroy(dev->class);
unregister_chrdev:
    unregister_chrdev_region(dev->dev_num, 1);
    return ret;
}

/*
 * 平台移除函数
 */
static int gpiobasic_remove(struct platform_device *pdev)
{
    struct gpiobasic_dev *dev = platform_get_drvdata(pdev);

    cdev_del(&dev->cdev);
    sysfs_remove_group(&dev->device->kobj, &gpiobasic_attr_group);
    device_destroy(dev->class, dev->dev_num);
    class_destroy(dev->class);
    unregister_chrdev_region(dev->dev_num, 1);

    /* 关闭所有LED */
    leds_set_all(dev, 0);

    dev_info(&pdev->dev, "GPIO Basic driver removed\n");

    return 0;
}

/* Device Tree匹配 */
static const struct of_device_id gpiobasic_of_match[] = {
    { .compatible = "myvendor,gpiobasic", },
    { }
};
MODULE_DEVICE_TABLE(of, gpiobasic_of_match);

static struct platform_driver gpiobasic_driver = {
    .probe  = gpiobasic_probe,
    .remove = gpiobasic_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(gpiobasic_of_match),
    },
};

module_platform_driver(gpiobasic_driver);
