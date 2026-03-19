/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 170
 * Language: c
 * Block ID: 5b45c00c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 支持空间导航的代码结构

// 使用区域标记
// ============ LIFECYCLE ============
Database *db_create(const char *path);
void db_open(Database *db);
void db_close(Database *db);
void db_destroy(Database *db);

// ============ QUERY ============
Result *db_query(Database *db, const char *sql);
void result_free(Result *r);

// ============ TRANSACTION ============
void db_begin(Database *db);
void db_commit(Database *db);
void db_rollback(Database *db);

// 这种结构帮助建立"代码地图"的心智模型
