/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\README.md
 * Line: 315
 * Language: c
 * Block ID: 03f2b4e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// hello_rpi.c - 最简单的内核模块
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void) {
    printk(KERN_INFO "[HelloRPI] Module loaded!\n");
    return 0;
}

static void __exit hello_exit(void) {
    printk(KERN_INFO "[HelloRPI] Module unloaded!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Embedded Linux Developer");
MODULE_DESCRIPTION("Raspberry Pi Hello World Module");
