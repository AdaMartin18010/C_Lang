/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 39
 * Language: c
 * Block ID: d096ec8f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不安全
char buf[256];
strcpy(buf, user_input);  // 溢出风险
sprintf(buf, "Hello %s", name);  // 溢出+格式字符串漏洞

// 安全
SafeString *s = safestr_create(256);
safestr_append(s, user_input);  // 自动扩容
safestr_printf(s, "Hello %s", name);  // 安全检查
