/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\01_Function_Knowledge_Graph.md
 * Line: 180
 * Language: c
 * Block ID: cea77048
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 不好的设计：一个函数做太多事情
void process_data_bad(data_t* data) {
    read_file(data);
    validate(data);
    transform(data);
    save_to_db(data);
    send_notification(data);
}

// ✅ 好的设计：每个函数只做一件事
void read_data(const char* path, data_t* out);
bool validate_data(const data_t* data);
void transform_data(data_t* data);
bool save_data(const data_t* data);
void notify(const data_t* data);
