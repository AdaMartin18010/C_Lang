/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 726
 * Language: c
 * Block ID: aeafa920
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Linux内核PMU驱动示例 - x86 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/msr.h>

#define PMU_DEVICE_NAME "pmu_driver"
#define PMU_CLASS_NAME  "pmu_class"

/* Intel PMU MSR定义 */
#define IA32_PERFEVTSEL0    0x186
#define IA32_PERFEVTSEL1    0x187
#define IA32_PMC0           0xC1
#define IA32_PMC1           0xC2
#define IA32_PERF_GLOBAL_CTRL   0x38F
#define IA32_PERF_GLOBAL_STATUS 0x38E
#define IA32_PERF_GLOBAL_OVF_CTRL 0x390

/* 事件配置结构体 */
struct pmu_event_config {
    uint64_t event_select;
    uint64_t umask;
    int os;
    int usr;
    int edge;
};

static struct class *pmu_class = NULL;
static struct device *pmu_device = NULL;
static int major;

/* 读取MSR */
static inline uint64_t native_read_msr_safe(unsigned int msr, int *err) {
    uint64_t low, high;
    *err = 0;
    asm volatile("1: rdmsr\n"
                 "2:\n"
                 ".section .fixup,\"ax\"\n"
                 "3: mov %4, %0\n"
                 "   jmp 2b\n"
                 ".previous\n"
                 ".section __ex_table,\"a\"\n"
                 "   .align 8\n"
                 "   .quad 1b, 3b\n"
                 ".previous"
                 : "=&r" (*err), "=a" (low), "=d" (high)
                 : "c" (msr), "i" (-EFAULT), "0" (0));
    return low | ((uint64_t)high << 32);
}

/* 写入MSR */
static inline int native_write_msr_safe(unsigned int msr, uint64_t val) {
    int err;
    asm volatile("1: wrmsr\n"
                 "2:\n"
                 ".section .fixup,\"ax\"\n"
                 "3: mov %4, %0\n"
                 "   jmp 2b\n"
                 ".previous\n"
                 ".section __ex_table,\"a\"\n"
                 "   .align 8\n"
                 "   .quad 1b, 3b\n"
                 ".previous"
                 : "=&r" (err)
                 : "c" (msr), "a" ((uint32_t)val), "d" ((uint32_t)(val >> 32)),
                   "i" (-EFAULT), "0" (0));
    return err;
}

/* 配置性能计数器 */
static int pmu_config_counter(int counter_id, struct pmu_event_config *config) {
    uint64_t evt_sel;
    unsigned int sel_reg, cnt_reg;

    if (counter_id < 0 || counter_id > 1) {
        return -EINVAL;
    }

    sel_reg = IA32_PERFEVTSEL0 + counter_id;
    cnt_reg = IA32_PMC0 + counter_id;

    /* 先禁用计数器 */
    native_write_msr_safe(sel_reg, 0);

    /* 清零计数器 */
    native_write_msr_safe(cnt_reg, 0);

    /* 配置事件选择寄存器 */
    evt_sel = config->event_select & 0xFF;
    evt_sel |= (config->umask & 0xFF) << 8;
    evt_sel |= (config->usr ? 1ULL : 0ULL) << 16;
    evt_sel |= (config->os ? 1ULL : 0ULL) << 17;
    evt_sel |= (config->edge ? 1ULL : 0ULL) << 18;
    evt_sel |= 1ULL << 22;  /* 使能位 */

    native_write_msr_safe(sel_reg, evt_sel);

    printk(KERN_INFO "PMU: Configured counter %d, event=0x%llx, umask=0x%llx\n",
           counter_id, config->event_select, config->umask);

    return 0;
}

/* 启动/停止计数器 */
static void pmu_control_all(int enable) {
    uint64_t global_ctrl = enable ? 0x3 : 0;  /* 使能PMC0和PMC1 */
    native_write_msr_safe(IA32_PERF_GLOBAL_CTRL, global_ctrl);
    printk(KERN_INFO "PMU: Global control = 0x%llx\n", global_ctrl);
}

/* 读取计数器 */
static uint64_t pmu_read_counter(int counter_id) {
    int err;
    unsigned int cnt_reg = IA32_PMC0 + counter_id;
    return native_read_msr_safe(cnt_reg, &err);
}

/* 设备打开 */
static int pmu_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "PMU: Device opened\n");
    return 0;
}

/* 设备释放 */
static int pmu_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "PMU: Device released\n");
    return 0;
}

/* IOCTL处理 */
static long pmu_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct pmu_event_config config;
    uint64_t value;
    int counter_id;

    switch (cmd) {
    case PMU_IOC_CONFIG:
        if (copy_from_user(&config, (void __user *)arg, sizeof(config)))
            return -EFAULT;
        if (copy_from_user(&counter_id, (void __user *)(arg + sizeof(config)),
                          sizeof(int)))
            return -EFAULT;
        return pmu_config_counter(counter_id, &config);

    case PMU_IOC_START:
        pmu_control_all(1);
        return 0;

    case PMU_IOC_STOP:
        pmu_control_all(0);
        return 0;

    case PMU_IOC_READ:
        if (copy_from_user(&counter_id, (void __user *)arg, sizeof(int)))
            return -EFAULT;
        value = pmu_read_counter(counter_id);
        if (copy_to_user((void __user *)(arg + sizeof(int)), &value, sizeof(value)))
            return -EFAULT;
        return 0;

    default:
        return -EINVAL;
    }
}

static struct file_operations pmu_fops = {
    .owner = THIS_MODULE,
    .open = pmu_open,
    .release = pmu_release,
    .unlocked_ioctl = pmu_ioctl,
};

static int __init pmu_init(void) {
    major = register_chrdev(0, PMU_DEVICE_NAME, &pmu_fops);
    if (major < 0) {
        printk(KERN_ALERT "PMU: Failed to register device\n");
        return major;
    }

    pmu_class = class_create(THIS_MODULE, PMU_CLASS_NAME);
    if (IS_ERR(pmu_class)) {
        unregister_chrdev(major, PMU_DEVICE_NAME);
        return PTR_ERR(pmu_class);
    }

    pmu_device = device_create(pmu_class, NULL, MKDEV(major, 0),
                               NULL, PMU_DEVICE_NAME);
    if (IS_ERR(pmu_device)) {
        class_destroy(pmu_class);
        unregister_chrdev(major, PMU_DEVICE_NAME);
        return PTR_ERR(pmu_device);
    }

    printk(KERN_INFO "PMU: Driver loaded, major=%d\n", major);
    return 0;
}

static void __exit pmu_exit(void) {
    device_destroy(pmu_class, MKDEV(major, 0));
    class_destroy(pmu_class);
    unregister_chrdev(major, PMU_DEVICE_NAME);
    printk(KERN_INFO "PMU: Driver unloaded\n");
}

module_init(pmu_init);
module_exit(pmu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Performance Analysis Team");
MODULE_DESCRIPTION("x86 PMU Kernel Driver");
