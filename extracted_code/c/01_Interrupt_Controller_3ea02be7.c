/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 1309
 * Language: c
 * Block ID: 3ea02be7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核 IRQ描述符核心数据结构
 * include/linux/irqdesc.h
 */

struct irq_desc {
    struct irq_common_data  irq_common_data;
    struct irq_data         irq_data;
    unsigned int __percpu   *kstat_irqs;

    // 中断处理入口函数
    irq_flow_handler_t      handle_irq;

    // 驱动注册的处理函数链表
    struct irqaction        *action;

    // 中断统计信息
    unsigned int            status_use_accessors;
    atomic_t                threads_active;

    // 中断线程化支持
    struct task_struct      *threads;
    struct irqaction        *threads_oneshot;

    // 亲和性和唤醒设置
    cpumask_var_t           affinity;
    cpumask_var_t           effective_affinity;

    // 电源管理
#ifdef CONFIG_PM_SLEEP
    unsigned int            nr_actions;
    unsigned int            no_suspend_depth;
    unsigned int            cond_suspend_depth;
    unsigned int            force_resume_depth;
#endif

#ifdef CONFIG_PROC_FS
    struct proc_dir_entry   *dir;
#endif

    // 核心锁
    struct mutex            request_mutex;
};

// irqaction - 驱动注册的处理函数
struct irqaction {
    irq_handler_t           handler;        // 硬中断处理函数
    void                    *dev_id;        // 设备标识
    void __percpu           *percpu_dev_id;
    struct irqaction        *next;          // 链表下一个 (共享中断)
    irq_handler_t           thread_fn;      // 线程处理函数
    struct task_struct      *thread;
    struct irqaction        *secondary;

    // 标志和名称
    unsigned int            irq;
    unsigned int            flags;
    unsigned long           thread_flags;
    unsigned long           thread_mask;
    const char              *name;
    struct proc_dir_entry   *dir;
};

// irq_chip - 中断控制器操作接口
struct irq_chip {
    const char              *name;
    unsigned int            (*irq_startup)(struct irq_data *data);
    void                    (*irq_shutdown)(struct irq_data *data);
    void                    (*irq_enable)(struct irq_data *data);
    void                    (*irq_disable)(struct irq_data *data);

    // 核心操作
    void                    (*irq_ack)(struct irq_data *data);
    void                    (*irq_mask)(struct irq_data *data);
    void                    (*irq_mask_ack)(struct irq_data *data);
    void                    (*irq_unmask)(struct irq_data *data);
    void                    (*irq_eoi)(struct irq_data *data);

    // 高级特性
    int                     (*irq_set_affinity)(struct irq_data *data,
                                                 const struct cpumask *dest,
                                                 bool force);
    int                     (*irq_retrigger)(struct irq_data *data);
    int                     (*irq_set_type)(struct irq_data *data, unsigned int flow_type);
    int                     (*irq_set_wake)(struct irq_data *data, unsigned int on);

    // ... 更多回调
};
