/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1233
 * Language: c
 * Block ID: 7bff3f18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============ 从C23迁移到C2y ============

// 1. 资源管理现代化
// Before:
void old_style(void) {
    FILE* f = fopen("file", "r");
    if (!f) return;
    // ... 多处return需要小心处理fclose
    if (error) { fclose(f); return; }
    // ...
    fclose(f);
}

// After:
void new_style(void) {
    if (FILE* f = fopen("file", "r"); f != nullptr) {
        defer fclose(f);
        // 放心return，自动清理
        if (error) return;
    }
}

// 2. 数组处理现代化
// Before:
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void process(int arr[], size_t n);  // 需要传递大小

// After:
void process_c2y(int arr[_Countof(arr)]);  // 如果可能
// 或保持显式参数但使用_Countof调用方

// 3. 循环控制现代化
// Before:
for (int i = 0; i < N && !found; i++) {
    for (int j = 0; j < M && !found; j++) {
        if (match(i, j)) { found = 1; result_i = i; result_j = j; }
    }
}

// After:
search: for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
        if (match(i, j)) {
            result_i = i;
            result_j = j;
            break search;
        }
    }
}

// 4. 默认值处理
// Before:
const char* name = user_input ? user_input : "default";

// After:
const char* name = user_input ?: "default";
