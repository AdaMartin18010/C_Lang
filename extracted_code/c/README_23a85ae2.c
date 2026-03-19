/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 157
 * Language: c
 * Block ID: 23a85ae2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 良好的生命周期设计
// 创建-使用-销毁 模式

database_t* db = database_open("connection_string");
if (!db) {
    handle_error();
}

// 使用...
database_query(db, "SELECT * FROM table");

// 必须销毁
database_close(db);  // 即使出错也要调用
