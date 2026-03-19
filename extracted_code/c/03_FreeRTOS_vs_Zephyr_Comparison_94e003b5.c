/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 4963
 * Language: c
 * Block ID: 94e003b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Zephyr MPU配置示例 */
/* prj.conf */
CONFIG_ARM_MPU=y
CONFIG_USERSPACE=y
CONFIG_MEMORY_PROTECTION=y

/* 运行时内存域配置 */
K_MEM_PARTITION_DEFINE(my_partition, start, size, K_MEM_PARTITION_P_RW_U_RW);
