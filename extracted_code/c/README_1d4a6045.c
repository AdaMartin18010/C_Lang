/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 72
 * Language: c
 * Block ID: 1d4a6045
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux 内核的驱动模型设计 - 展示松耦合架构
// include/linux/device.h

struct device_driver {
    const char      *name;
    struct bus_type *bus;

    // 操作函数表 - 实现多态性
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
};

struct device {
    struct device_driver *driver;
    void                 *platform_data;
    void                 *driver_data;

    // 设备树节点引用
    struct device_node   *of_node;
};
