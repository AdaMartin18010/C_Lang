/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\README.md
 * Line: 139
 * Language: c
 * Block ID: 28d3d9fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 新增：安全内存清除（防止优化消除）
void memset_explicit(void *s, int c, size_t n);

// 用法：安全擦除敏感数据
char password[64];
// ... 使用密码 ...
memset_explicit(password, 0, sizeof(password));  // 保证被擦除
