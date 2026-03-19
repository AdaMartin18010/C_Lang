/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1661
 * Language: c
 * Block ID: d12d9f32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 改进1：使用size_t并添加注释
void process_array(int *arr, size_t count);

// 调用
int data[100];
process_array(data, _Countof(data));  // 清晰明了

// 改进2：使用数组引用（如果大小固定）
void process_array_fixed(int arr[100]);

// 改进3：使用包装结构
typedef struct {
    int *data;
    size_t count;
} IntArrayView;

void process_array_view(IntArrayView arr);
process_array_view((IntArrayView){data, _Countof(data)});
