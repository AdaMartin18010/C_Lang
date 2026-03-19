/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 573
 * Language: c
 * Block ID: 4b6a2b2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 外部链接 */
/* common.h */
extern int g_shared_counter;

/* common.c */
int g_shared_counter = 0;

/* file1.c */
#include "common.h"
void increment(void) { g_shared_counter++; }

/* file2.c */
#include "common.h"
int read(void) { return g_shared_counter; }

/* ✅ 合规 - 内部链接 */
/* module.c */
static int s_module_state = 0;  /* 只在module.c中可见 */

void module_init(void) { s_module_state = 1; }

/* ✅ 合规 - 明确分离 */
/* private.h（只在模块内部包含）*/
extern int s_module_state;  /* 模块内部使用 */
