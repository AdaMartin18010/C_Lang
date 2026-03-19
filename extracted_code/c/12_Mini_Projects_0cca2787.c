/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 399
 * Language: c
 * Block ID: 0cca2787
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

JSONValue* json_parse(const char *text);
JSONValue* json_parse_file(const char *path);

// 访问
int json_get_int(const JSONValue *val, const char *path);
const char* json_get_string(const JSONValue *val, const char *path);

// 序列化
char* json_stringify(const JSONValue *val, bool pretty);
