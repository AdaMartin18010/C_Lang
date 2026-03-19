/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 437
 * Language: c
 * Block ID: 15df45f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 抑制特定行的警告
/*lint -e{9024} */  // 抑制Rule 11.4
void *ptr = (void *)0x1000;

// 使用偏离记录注释
/* Deviation Record: DEV001
 * Rule: 11.4
 * Justification: Memory-mapped I/O address
 * Safety Impact: None, verified by manual review
 */
#define MMIO_ADDR ((void *)0x1000)
