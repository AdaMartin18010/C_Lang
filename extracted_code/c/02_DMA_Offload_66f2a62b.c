/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\02_DMA_Offload.md
 * Line: 2345
 * Language: c
 * Block ID: 66f2a62b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 标准C库 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdalign.h>

/* POSIX */
#include <time.h>
#include <signal.h>
#include <unistd.h>

/* Linux内核 (内核态代码) */
#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/completion.h>
#endif
