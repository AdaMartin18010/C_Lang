/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 202
 * Language: c
 * Block ID: 3bfc5514
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 忽略错误 */
malloc(100);  /* 内存泄漏风险 */
strcpy(dest, src);  /* 溢出风险 */
fclose(fp);  /* 错误被忽略 */
remove("file");  /* 删除失败不知道 */

/* ❌ 违反 - 部分检查 */
printf("Value: %d\n", value);  /* 返回值忽略 */
