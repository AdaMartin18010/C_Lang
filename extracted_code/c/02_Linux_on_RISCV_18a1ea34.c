/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\02_Linux_on_RISCV.md
 * Line: 379
 * Language: c
 * Block ID: 18a1ea34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内核中操作设备树的API

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

// 获取设备节点
struct device_node *np = of_find_node_by_path("/soc/my-accelerator@20000000");
if (!np) {
    dev_err(dev, "Failed to find device node\n");
    return -ENODEV;
}

// 读取属性
u32 ring_size;
of_property_read_u32(np, "acme,ring-size", &ring_size);

// 获取内存资源
struct resource res;
of_address_to_resource(np, 0, &res);
void __iomem *base = ioremap(res.start, resource_size(&res));

// 获取中断号
int irq = of_irq_get(np, 0);
if (irq < 0) {
    dev_err(dev, "Failed to get IRQ: %d\n", irq);
    return irq;
}

// 获取时钟
struct clk *clk = of_clk_get(np, 0);
if (IS_ERR(clk)) {
    return PTR_ERR(clk);
}
clk_prepare_enable(clk);

// 释放资源
of_node_put(np);
