/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 325
 * Language: c
 * Block ID: 09ea0afe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

Config* config_load(const char *path, ConfigFormat format);
int config_get_int(Config *cfg, const char *key, int default_val);
const char* config_get_string(Config *cfg, const char *key);
