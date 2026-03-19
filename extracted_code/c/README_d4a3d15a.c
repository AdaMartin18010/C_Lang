/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\README.md
 * Line: 151
 * Language: c
 * Block ID: d4a3d15a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 新增（已广泛作为扩展支持）
char *strdup(const char *s);      // 分配并复制字符串
char *strndup(const char *s, size_t n);  // 分配并复制n个字符

// 内存拷贝扩展
void *memccpy(void *dest, const void *src, int c, size_t n);
