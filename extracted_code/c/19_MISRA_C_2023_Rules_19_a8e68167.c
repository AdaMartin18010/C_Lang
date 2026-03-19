/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\19_MISRA_C_2023_Rules_19.md
 * Line: 27
 * Language: c
 * Block ID: a8e68167
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 系统头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ✅ 合规 - 项目头文件 */
#include "config.h"
#include "utils.h"
#include "module/driver.h"

/* ✅ 合规 - 相对路径 */
#include "../include/common.h"
#include "./local.h"

/* ❌ 违反 - 宏扩展 */
#define HEADER "file.h"
#include HEADER  /* 违反 */

/* ❌ 违反 - 拼接 */
#include FILENAME  /* 宏扩展 */
