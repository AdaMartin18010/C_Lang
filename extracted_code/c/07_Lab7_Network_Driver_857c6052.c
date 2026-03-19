/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 183
 * Language: c
 * Block ID: 857c6052
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.c

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "e1000.h"

// MMIO基地址
volatile uint32 *e1000_regs;

// 发送描述符环
struct tx_desc tx_ring[NTXDESC] __attribute__((aligned(16)));
char tx_bufs[NTXDESC][2048];  // 发送缓冲区
int tx_mbuf[NTXDESC];         // 保存mbuf索引

// 接收描述符环
struct rx_desc rx_ring[NRXDESC] __attribute__((aligned(16)));
char rx_bufs[NRXDESC][2048];  // 接收缓冲区

// 锁
struct spinlock e1000_lock;

// MAC地址
uint8 mac_addr[6];
