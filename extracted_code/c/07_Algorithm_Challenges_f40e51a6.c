/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 249
 * Language: c
 * Block ID: f40e51a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct Regex Regex;

// 编译正则表达式
Regex* regex_compile(const char *pattern);

// 匹配整个字符串
bool regex_match(const Regex *re, const char *text);

// 搜索子串
bool regex_search(const Regex *re, const char *text,
                  int *match_start, int *match_end);

// 释放资源
void regex_free(Regex *re);
