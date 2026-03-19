/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 191
 * Language: c
 * Block ID: c752efc5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单接口（常见情况）
ErrorCode file_copy(const char *src, const char *dst);

// 完整接口（高级情况）
ErrorCode file_copy_ex(const char *src, const char *dst,
                        CopyOptions options,
                        ProgressCallback on_progress,
                        void *user_data);

// 使用示例
// 简单使用
file_copy("source.txt", "dest.txt");

// 高级使用
CopyOptions opts = {
    .buffer_size = 64 * 1024,
    .overwrite = true,
    .preserve_timestamps = true
};
file_copy_ex("bigfile.zip", "backup/bigfile.zip",
              opts, show_progress, NULL);
