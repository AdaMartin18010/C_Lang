/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 235
 * Language: c
 * Block ID: 2f64bc86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* FreeRTOSConfig.h - 内存配置 */
#define configTOTAL_HEAP_SIZE           ((size_t)(32 * 1024))
#define configUSE_HEAP_SCHEME           4

/* 五种堆分配方案 */
/* Heap_1: 仅分配，不释放 - 最简单 */
/* Heap_2: 最佳匹配算法 - 已废弃 */
/* Heap_3: 使用标准库malloc/free - 线程安全 */
/* Heap_4: 首次适配算法 - 支持合并 */
/* Heap_5: Heap_4 + 跨多个内存区域 */
