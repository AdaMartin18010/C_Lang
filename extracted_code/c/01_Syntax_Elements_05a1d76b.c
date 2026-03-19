/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 347
 * Language: c
 * Block ID: 05a1d76b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 普通字符
char c1 = 'A';
char c2 = '\n';  // 转义序列

// 转义序列
char newline = '\n';
char tab = '\t';
char backslash = '\\';
char single_quote = '\'';
char null_char = '\0';

// 八进制转义
char oct_esc = '\101';  // 'A' (65 in octal)

// 十六进制转义
char hex_esc = '\x41';  // 'A' (65 in hex)

// Unicode字符(C11)
char16_t c16 = u'中';   // UTF-16
char32_t c32 = U'中';   // UTF-32

// 宽字符
wchar_t wc = L'中';     // 平台相关
