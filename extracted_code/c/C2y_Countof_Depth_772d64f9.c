/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 394
 * Language: c
 * Block ID: 772d64f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int *data;
    size_t count;
} IntArrayView;

void print_array_view(IntArrayView view) {
    for (size_t i = 0; i < view.count; i++) {
        printf("%d ", view.data[i]);
    }
}

// 调用
int data[10];
print_array_view((IntArrayView){data, _Countof(data)});
