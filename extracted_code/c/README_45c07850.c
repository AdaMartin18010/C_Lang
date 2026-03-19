/*
 * Auto-generated from: 01_Core_Knowledge_System\11_Internationalization\README.md
 * Line: 333
 * Language: c
 * Block ID: 45c07850
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* i18n 设计原则:
 * 1. 所有用户可见字符串外部化
 * 2. 避免字符串拼接
 * 3. 支持占位符和参数化
 * 4. 考虑文本扩展 (英语→德语 +30%)
 */

// ❌ 错误做法 - 字符串拼接
char msg[256];
strcpy(msg, "You have ");
strcat(msg, count_str);
strcat(msg, " messages.");

// ✅ 正确做法 - 完整句子
printf(ngettext(
    "You have %d message in %s.",
    "You have %d messages in %s.",
    count
), count, folder_name);

// ❌ 错误做法 - 假设单词顺序
printf(_("Copying "));
printf(filename);
printf(_(" to "));
printf(destination);

// ✅ 正确做法 - 完整句子模板
printf(_("Copying %(source)s to %(dest)s"),
       "source", filename,
       "dest", destination, NULL);
