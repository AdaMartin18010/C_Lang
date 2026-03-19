/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\01_Syntax_Elements.md
 * Line: 613
 * Language: c
 * Block ID: 71d4b7e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 未定义行为？不，是未指定行为
const char *s = "Hello" " " "World";  // OK，翻译阶段6拼接

// ✅ 明确意图
const char *s1 = "Hello World";  // 直接写完整

// 注意：不同编码前缀不能拼接
// const char *err = u8"Hello" u"World";  // 错误！
