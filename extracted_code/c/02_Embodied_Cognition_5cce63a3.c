/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 461
 * Language: c
 * Block ID: 5cce63a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：批量操作后才反馈
void process_all_files_bad(FileList *files) {
    for (int i = 0; i < files->count; i++) {
        process_file(files->items[i]);  // 长时间无反馈
    }
    printf("All done\n");  // 太晚了！
}

// 正确：渐进式反馈
void process_all_files_good(FileList *files) {
    for (int i = 0; i < files->count; i++) {
        printf("Processing %d/%d: %s\n",
               i + 1, files->count, files->items[i]->name);
        process_file(files->items[i]);
        printf("  ✓ Done\n");
    }
}
