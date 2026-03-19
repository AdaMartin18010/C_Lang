/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 145
 * Language: c
 * Block ID: 7cba9b31
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 使用函数指针实现回调
typedef void (*progress_callback_t)(int percent, void* user_data);

void process_data(data_t* data, progress_callback_t callback, void* user_data) {
    for (int i = 0; i < data->count; i++) {
        process_item(&data->items[i]);
        if (callback) {
            callback((i * 100) / data->count, user_data);
        }
    }
}

// 使用示例
void my_progress(int percent, void* user_data) {
    printf("Progress: %d%%\n", percent);
}

process_data(data, my_progress, NULL);
