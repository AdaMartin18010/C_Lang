/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 6863
 * Language: c
 * Block ID: 9a10eb72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: gpio_matrix_keyboard.c
 * Description: 完整GPIO矩阵键盘驱动实现
 * 支持：矩阵扫描、消抖、按键映射、组合键、输入子系统
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GPIO Keyboard Developer");
MODULE_DESCRIPTION("GPIO Matrix Keyboard Driver");
MODULE_VERSION("1.0.0");

#define DRIVER_NAME     "gpio_keyboard"
#define DEVICE_NAME     "gpio_keyboard"
#define MAX_MATRIX_SIZE 16
#define MAX_KEYS        (MAX_MATRIX_SIZE * MAX_MATRIX_SIZE)
#define DEFAULT_POLL_MS 20
#define DEFAULT_DEBOUNCE_MS 5

/* 按键状态 */
enum key_state {
    KEY_STATE_RELEASED = 0,
    KEY_STATE_PRESSED,
    KEY_STATE_DEBOUNCE_PRESS,
    KEY_STATE_DEBOUNCE_RELEASE,
};

/* 按键数据结构 */
struct gpio_key {
    unsigned int code;          /* 按键码 */
    enum key_state state;       /* 当前状态 */
    enum key_state last_state;  /* 上次状态 */
    unsigned long debounce_time;/* 消抖时间 */
    bool changed;               /* 状态是否变化 */
    u32 press_count;            /* 按下计数 */
    u32 release_count;          /* 释放计数 */
};

/* 矩阵配置 */
struct matrix_config {
    unsigned int rows;
    unsigned int cols;
    struct gpio_desc **row_gpios;
    struct gpio_desc **col_gpios;
    unsigned int *keycodes;     /* 按键码矩阵 */
    bool active_low;
};

/* 设备私有数据 */
struct gpio_keyboard {
    struct platform_device *pdev;
    struct input_dev *input;

    /* 矩阵配置 */
    struct matrix_config matrix;
    struct gpio_key keys[MAX_KEYS];
    unsigned int num_keys;

    /* 扫描方式 */
    bool use_polling;           /* true=轮询, false=中断 */
    struct timer_list poll_timer;
    unsigned int poll_interval; /* ms */

    /* 中断方式 */
    unsigned int *row_irqs;
    struct work_struct irq_work;

    /* 消抖 */
    unsigned int debounce_ms;
    struct timer_list debounce_timer;
    bool debounce_active;

    /* 同步 */
    struct mutex lock;
    spinlock_t irq_lock;

    /* 组合键 */
    unsigned int combo_keys[4];
    unsigned int num_combo;
    unsigned int combo_timeout_ms;
    struct timer_list combo_timer;

    /* 调试 */
    bool debug;
    u64 scan_count;
    u64 key_events;
    u64 bounce_filtered;
};

/*
 * 扫描矩阵键盘 - 核心算法
 */
static void scan_matrix(struct gpio_keyboard *kbd)
{
    int row, col;
    int idx;
    int val;
    unsigned long flags;

    spin_lock_irqsave(&kbd->irq_lock, flags);

    for (row = 0; row < kbd->matrix.rows; row++) {
        /* 设置当前行为输出低电平 */
        gpiod_direction_output(kbd->matrix.row_gpios[row], 0);
        udelay(10);  /* 等待稳定 */

        for (col = 0; col < kbd->matrix.cols; col++) {
            idx = row * kbd->matrix.cols + col;

            /* 读取列状态 */
            val = gpiod_get_value(kbd->matrix.col_gpios[col]);

            /* 根据active_low转换 */
            if (kbd->matrix.active_low)
                val = !val;

            /* 状态机 */
            switch (kbd->keys[idx].state) {
            case KEY_STATE_RELEASED:
                if (val) {
                    /* 检测到按下，开始消抖 */
                    kbd->keys[idx].state = KEY_STATE_DEBOUNCE_PRESS;
                    kbd->keys[idx].debounce_time = jiffies +
                        msecs_to_jiffies(kbd->debounce_ms);
                }
                break;

            case KEY_STATE_DEBOUNCE_PRESS:
                if (val) {
                    /* 消抖时间到，确认按下 */
                    if (time_after(jiffies, kbd->keys[idx].debounce_time)) {
                        kbd->keys[idx].state = KEY_STATE_PRESSED;
                        kbd->keys[idx].changed = true;
                        kbd->keys[idx].press_count++;
                        kbd->key_events++;
                    }
                } else {
                    /* 消抖期间释放，认为是抖动 */
                    kbd->keys[idx].state = KEY_STATE_RELEASED;
                    kbd->bounce_filtered++;
                }
                break;

            case KEY_STATE_PRESSED:
                if (!val) {
                    /* 检测到释放，开始消抖 */
                    kbd->keys[idx].state = KEY_STATE_DEBOUNCE_RELEASE;
                    kbd->keys[idx].debounce_time = jiffies +
                        msecs_to_jiffies(kbd->debounce_ms);
                }
                break;

            case KEY_STATE_DEBOUNCE_RELEASE:
                if (!val) {
                    /* 消抖时间到，确认释放 */
                    if (time_after(jiffies, kbd->keys[idx].debounce_time)) {
                        kbd->keys[idx].state = KEY_STATE_RELEASED;
                        kbd->keys[idx].changed = true;
                        kbd->keys[idx].release_count++;
                    }
                } else {
                    /* 消抖期间按下，认为是抖动 */
                    kbd->keys[idx].state = KEY_STATE_PRESSED;
                    kbd->bounce_filtered++;
                }
                break;
            }
        }

        /* 恢复行为输入高阻态 */
        gpiod_direction_input(kbd->matrix.row_gpios[row]);
    }

    kbd->scan_count++;
    spin_unlock_irqrestore(&kbd->irq_lock, flags);
}

/*
 * 向输入子系统报告按键事件
 */
static void report_keys(struct gpio_keyboard *kbd)
{
    int i;
    bool sync_needed = false;

    for (i = 0; i < kbd->num_keys; i++) {
        if (kbd->keys[i].changed) {
            unsigned int code = kbd->matrix.keycodes[i];
            int value = (kbd->keys[i].state == KEY_STATE_PRESSED) ? 1 : 0;

            if (kbd->debug)
                dev_info(&kbd->pdev->dev, "Key %d (code %d) %s\n",
                         i, code, value ? "pressed" : "released");

            input_report_key(kbd->input, code, value);
            kbd->keys[i].changed = false;
            kbd->keys[i].last_state = kbd->keys[i].state;
            sync_needed = true;
        }
    }

    if (sync_needed)
        input_sync(kbd->input);
}

/*
 * 轮询定时器回调
 */
static void poll_timer_callback(struct timer_list *t)
{
    struct gpio_keyboard *kbd = from_timer(kbd, t, poll_timer);

    scan_matrix(kbd);
    report_keys(kbd);

    /* 重新设置定时器 */
    if (kbd->use_polling)
        mod_timer(&kbd->poll_timer, jiffies + msecs_to_jiffies(kbd->poll_interval));
}

/*
 * 中断处理 - 行变化触发
 */
static irqreturn_t row_irq_handler(int irq, void *dev_id)
{
    struct gpio_keyboard *kbd = dev_id;

    /* 禁用中断，调度工作队列 */
    disable_irq_nosync(irq);
    schedule_work(&kbd->irq_work);

    return IRQ_HANDLED;
}

/*
 * 中断工作队列处理
 */
static void irq_work_handler(struct work_struct *work)
{
    struct gpio_keyboard *kbd = container_of(work, struct gpio_keyboard, irq_work);
    int row;

    /* 扫描矩阵 */
    scan_matrix(kbd);
    report_keys(kbd);

    /* 重新使能中断 */
    for (row = 0; row < kbd->matrix.rows; row++) {
        enable_irq(kbd->row_irqs[row]);
    }
}

/*
 * 从Device Tree解析矩阵配置
 */
static int parse_matrix_from_dt(struct gpio_keyboard *kbd)
{
    struct device *dev = &kbd->pdev->dev;
    struct device_node *np = dev->of_node;
    struct device_node *keymap_np;
    int nrows, ncols;
    int i, j;
    int ret;

    /* 获取行列数 */
    ret = of_property_read_u32(np, "keyboard-rows", &nrows);
    if (ret) {
        dev_err(dev, "Missing keyboard-rows property\n");
        return ret;
    }

    ret = of_property_read_u32(np, "keyboard-columns", &ncols);
    if (ret) {
        dev_err(dev, "Missing keyboard-columns property\n");
        return ret;
    }

    if (nrows > MAX_MATRIX_SIZE || ncols > MAX_MATRIX_SIZE) {
        dev_err(dev, "Matrix size exceeds maximum\n");
        return -EINVAL;
    }

    kbd->matrix.rows = nrows;
    kbd->matrix.cols = ncols;
    kbd->num_keys = nrows * ncols;

    /* 分配GPIO数组 */
    kbd->matrix.row_gpios = devm_kcalloc(dev, nrows, sizeof(*kbd->matrix.row_gpios),
                                          GFP_KERNEL);
    kbd->matrix.col_gpios = devm_kcalloc(dev, ncols, sizeof(*kbd->matrix.col_gpios),
                                          GFP_KERNEL);
    kbd->matrix.keycodes = devm_kcalloc(dev, kbd->num_keys, sizeof(*kbd->matrix.keycodes),
                                         GFP_KERNEL);
    if (!kbd->matrix.row_gpios || !kbd->matrix.col_gpios || !kbd->matrix.keycodes)
        return -ENOMEM;

    /* 获取行GPIO */
    for (i = 0; i < nrows; i++) {
        kbd->matrix.row_gpios[i] = devm_gpiod_get_index(dev, "row", i, GPIOD_IN);
        if (IS_ERR(kbd->matrix.row_gpios[i])) {
            ret = PTR_ERR(kbd->matrix.row_gpios[i]);
            dev_err(dev, "Failed to get row GPIO %d: %d\n", i, ret);
            return ret;
        }
    }

    /* 获取列GPIO */
    for (i = 0; i < ncols; i++) {
        kbd->matrix.col_gpios[i] = devm_gpiod_get_index(dev, "col", i, GPIOD_IN);
        if (IS_ERR(kbd->matrix.col_gpios[i])) {
            ret = PTR_ERR(kbd->matrix.col_gpios[i]);
            dev_err(dev, "Failed to get col GPIO %d: %d\n", i, ret);
            return ret;
        }
    }

    /* 获取按键码 */
    keymap_np = of_get_child_by_name(np, "keymap");
    if (!keymap_np) {
        dev_warn(dev, "No keymap node, using default\n");
        for (i = 0; i < kbd->num_keys; i++)
            kbd->matrix.keycodes[i] = KEY_RESERVED;
    } else {
        for (i = 0; i < nrows; i++) {
            for (j = 0; j < ncols; j++) {
                char prop_name[32];
                u32 code;
                snprintf(prop_name, sizeof(prop_name), "row%d-col%d", i, j);
                ret = of_property_read_u32(keymap_np, prop_name, &code);
                if (ret)
                    code = KEY_RESERVED;
                kbd->matrix.keycodes[i * ncols + j] = code;
            }
        }
        of_node_put(keymap_np);
    }

    /* 获取配置 */
    kbd->matrix.active_low = of_property_read_bool(np, "active-low");
    of_property_read_u32(np, "poll-interval-ms", &kbd->poll_interval);
    if (kbd->poll_interval == 0)
        kbd->poll_interval = DEFAULT_POLL_MS;
    of_property_read_u32(np, "debounce-ms", &kbd->debounce_ms);
    if (kbd->debounce_ms == 0)
        kbd->debounce_ms = DEFAULT_DEBOUNCE_MS;

    kbd->use_polling = of_property_read_bool(np, "use-polling");

    dev_info(dev, "Matrix: %dx%d, polling=%s, interval=%dms, debounce=%dms\n",
             nrows, ncols, kbd->use_polling ? "yes" : "no",
             kbd->poll_interval, kbd->debounce_ms);

    return 0;
}

/*
 * 注册输入设备
 */
static int register_input_device(struct gpio_keyboard *kbd)
{
    struct input_dev *input;
    int i;
    int ret;

    input = devm_input_allocate_device(&kbd->pdev->dev);
    if (!input)
        return -ENOMEM;

    input->name = DRIVER_NAME;
    input->phys = "gpio-keys/input0";
    input->id.bustype = BUS_HOST;
    input->id.vendor = 0x0001;
    input->id.product = 0x0001;
    input->id.version = 0x0100;

    /* 设置支持的按键 */
    __set_bit(EV_KEY, input->evbit);
    for (i = 0; i < kbd->num_keys; i++) {
        if (kbd->matrix.keycodes[i] != KEY_RESERVED)
            __set_bit(kbd->matrix.keycodes[i], input->keybit);
    }

    /* 注册输入设备 */
    ret = input_register_device(input);
    if (ret) {
        dev_err(&kbd->pdev->dev, "Failed to register input device: %d\n", ret);
        return ret;
    }

    kbd->input = input;

    return 0;
}

/*
 * sysfs属性 - 统计信息
 */
static ssize_t stats_show(struct device *dev, struct device_attribute *attr,
                           char *buf)
{
    struct gpio_keyboard *kbd = dev_get_drvdata(dev);
    ssize_t count = 0;
    int i;

    count += sprintf(buf + count, "=== GPIO Keyboard Statistics ===\n");
    count += sprintf(buf + count, "Scan count: %llu\n", kbd->scan_count);
    count += sprintf(buf + count, "Key events: %llu\n", kbd->key_events);
    count += sprintf(buf + count, "Bounce filtered: %llu\n", kbd->bounce_filtered);
    count += sprintf(buf + count, "\nKey press counts:\n");

    for (i = 0; i < kbd->num_keys && count < PAGE_SIZE - 64; i++) {
        if (kbd->keys[i].press_count > 0) {
            count += sprintf(buf + count, "  Key %d (code %d): %u/%u\n",
                             i, kbd->matrix.keycodes[i],
                             kbd->keys[i].press_count, kbd->keys[i].release_count);
        }
    }

    return count;
}
static DEVICE_ATTR_RO(stats);

static ssize_t debug_show(struct device *dev, struct device_attribute *attr,
                           char *buf)
{
    struct gpio_keyboard *kbd = dev_get_drvdata(dev);
    return sprintf(buf, "%d\n", kbd->debug);
}

static ssize_t debug_store(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count)
{
    struct gpio_keyboard *kbd = dev_get_drvdata(dev);
    unsigned long val;

    if (kstrtoul(buf, 0, &val))
        return -EINVAL;

    kbd->debug = !!val;

    return count;
}
static DEVICE_ATTR_RW(debug);

static struct attribute *gpio_keyboard_attrs[] = {
    &dev_attr_stats.attr,
    &dev_attr_debug.attr,
    NULL,
};

static struct attribute_group gpio_keyboard_attr_group = {
    .attrs = gpio_keyboard_attrs,
};

/*
 * 平台探测函数
 */
static int gpio_keyboard_probe(struct platform_device *pdev)
{
    struct gpio_keyboard *kbd;
    int ret;
    int i;

    dev_info(&pdev->dev, "Probing GPIO Keyboard\n");

    kbd = devm_kzalloc(&pdev->dev, sizeof(*kbd), GFP_KERNEL);
    if (!kbd)
        return -ENOMEM;

    kbd->pdev = pdev;
    platform_set_drvdata(pdev, kbd);
    mutex_init(&kbd->lock);
    spin_lock_init(&kbd->irq_lock);

    /* 解析Device Tree */
    ret = parse_matrix_from_dt(kbd);
    if (ret)
        return ret;

    /* 初始化按键状态 */
    for (i = 0; i < kbd->num_keys; i++) {
        kbd->keys[i].code = kbd->matrix.keycodes[i];
        kbd->keys[i].state = KEY_STATE_RELEASED;
        kbd->keys[i].changed = false;
    }

    /* 注册输入设备 */
    ret = register_input_device(kbd);
    if (ret)
        return ret;

    /* 设置扫描方式 */
    if (kbd->use_polling) {
        /* 初始化轮询定时器 */
        timer_setup(&kbd->poll_timer, poll_timer_callback, 0);
        mod_timer(&kbd->poll_timer, jiffies + msecs_to_jiffies(kbd->poll_interval));
        dev_info(&pdev->dev, "Using polling mode\n");
    } else {
        /* 中断方式 */
        kbd->row_irqs = devm_kcalloc(&pdev->dev, kbd->matrix.rows,
                                      sizeof(*kbd->row_irqs), GFP_KERNEL);
        if (!kbd->row_irqs)
            return -ENOMEM;

        INIT_WORK(&kbd->irq_work, irq_work_handler);

        for (i = 0; i < kbd->matrix.rows; i++) {
            kbd->row_irqs[i] = gpiod_to_irq(kbd->matrix.row_gpios[i]);
            if (kbd->row_irqs[i] < 0) {
                dev_err(&pdev->dev, "Failed to get IRQ for row %d\n", i);
                return kbd->row_irqs[i];
            }

            ret = devm_request_irq(&pdev->dev, kbd->row_irqs[i], row_irq_handler,
                                   IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
                                   "gpio_keyboard", kbd);
            if (ret) {
                dev_err(&pdev->dev, "Failed to request IRQ %d: %d\n",
                        kbd->row_irqs[i], ret);
                return ret;
            }
        }

        dev_info(&pdev->dev, "Using interrupt mode\n");
    }

    /* 创建sysfs属性 */
    ret = sysfs_create_group(&pdev->dev.kobj, &gpio_keyboard_attr_group);
    if (ret) {
        dev_warn(&pdev->dev, "Failed to create sysfs group: %d\n", ret);
    }

    dev_info(&pdev->dev, "GPIO Keyboard driver loaded\n");

    return 0;
}

/*
 * 平台移除函数
 */
static int gpio_keyboard_remove(struct platform_device *pdev)
{
    struct gpio_keyboard *kbd = platform_get_drvdata(pdev);

    sysfs_remove_group(&pdev->dev.kobj, &gpio_keyboard_attr_group);

    if (kbd->use_polling) {
        del_timer_sync(&kbd->poll_timer);
    } else {
        cancel_work_sync(&kbd->irq_work);
    }

    input_unregister_device(kbd->input);

    dev_info(&pdev->dev, "GPIO Keyboard driver removed\n");

    return 0;
}

/* 电源管理 */
static int __maybe_unused gpio_keyboard_suspend(struct device *dev)
{
    struct gpio_keyboard *kbd = dev_get_drvdata(dev);

    if (kbd->use_polling)
        del_timer_sync(&kbd->poll_timer);

    return 0;
}

static int __maybe_unused gpio_keyboard_resume(struct device *dev)
{
    struct gpio_keyboard *kbd = dev_get_drvdata(dev);

    if (kbd->use_polling)
        mod_timer(&kbd->poll_timer, jiffies + msecs_to_jiffies(kbd->poll_interval));

    return 0;
}

static SIMPLE_DEV_PM_OPS(gpio_keyboard_pm_ops,
                         gpio_keyboard_suspend, gpio_keyboard_resume);

static const struct of_device_id gpio_keyboard_of_match[] = {
    { .compatible = "gpio-matrix-keyboard", },
    { }
};
MODULE_DEVICE_TABLE(of, gpio_keyboard_of_match);

static struct platform_driver gpio_keyboard_driver = {
    .probe  = gpio_keyboard_probe,
    .remove = gpio_keyboard_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = of_match_ptr(gpio_keyboard_of_match),
        .pm = &gpio_keyboard_pm_ops,
    },
};

module_platform_driver(gpio_keyboard_driver);
