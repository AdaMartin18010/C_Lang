/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\05_Lab5_Cow.md
 * Line: 266
 * Language: c
 * Block ID: 069bacbc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/riscv.h

// PTE标志位 (使用RSW保留位)
#define PTE_COW (1L << 8)  // Copy-on-Write标志
