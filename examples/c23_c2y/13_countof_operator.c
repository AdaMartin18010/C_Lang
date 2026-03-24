/*
 * C2y _Countof 操作符示例
 * 编译: gcc -std=c2y -o 13_countof_operator 13_countof_operator.c
 *       clang -std=c2y -o 13_countof_operator 13_countof_operator.c
 */

#include <stdio.h>

// _Countof 与 sizeof 对比
void demonstrate_countof(void) {
    int arr[10];
    int matrix[5][10];
    
    printf("sizeof vs _Countof:\n");
    printf("sizeof(arr) = %zu (bytes)\n", sizeof(arr));
    printf("_Countof(arr) = %zu (elements)\n", _Countof(arr));
    
    printf("\nFor 2D array:\n");
    printf("sizeof(matrix) = %zu (bytes)\n", sizeof(matrix));
    printf("_Countof(matrix) = %zu (rows)\n", _Countof(matrix));
    printf("_Countof(matrix[0]) = %zu (columns)\n", _Countof(matrix[0]));
    printf("Total elements: %zu\n", _Countof(matrix) * _Countof(matrix[0]));
}

// 泛型数组处理
void print_int_array(int arr[], size_t count) {
    printf("Array (%zu elements): ", count);
    for (size_t i = 0; i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

#define print_array(arr) print_int_array(arr, _Countof(arr))

// 安全的数组遍历宏
#define for_each(item, arr) \
    for (size_t _i = 0, _count = _Countof(arr); \
         _i < _count && ((item) = &(arr)[_i]); \
         _i++)

// 数组初始化检查
#define ensure_fully_initialized(arr) \
    static_assert(_Countof(arr) > 0, "Array must not be empty")

// 实际应用：查找最大值
int find_max(int arr[], size_t count) {
    if (count == 0) return 0;
    int max = arr[0];
    for (size_t i = 1; i < count; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

#define array_max(arr) find_max(arr, _Countof(arr))

// 字符串数组处理
void print_strings(const char *strs[], size_t count) {
    printf("String array (%zu items):\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("  [%zu] %s\n", i, strs[i]);
    }
}

// 安全的数组复制
void safe_copy(int dest[], size_t dest_count, 
               const int src[], size_t src_count) {
    size_t copy_count = dest_count < src_count ? dest_count : src_count;
    for (size_t i = 0; i < copy_count; i++) {
        dest[i] = src[i];
    }
}

#define copy_array(dest, src) \
    safe_copy(dest, _Countof(dest), src, _Countof(src))

int main(void) {
    printf("C2y _Countof Operator Demo\n\n");
    
    demonstrate_countof();
    
    printf("\n--- Basic Usage ---\n");
    int numbers[] = {10, 20, 30, 40, 50};
    printf("Array has %zu elements\n", _Countof(numbers));
    print_array(numbers);
    printf("Max value: %d\n", array_max(numbers));
    
    printf("\n--- For Each Loop ---\n");
    int *p;
    for_each(p, numbers) {
        printf("Processing: %d\n", *p);
    }
    
    printf("\n--- 2D Array ---\n");
    int matrix[3][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12}
    };
    
    printf("Matrix dimensions: %zu x %zu\n", 
           _Countof(matrix), _Countof(matrix[0]));
    
    for (size_t i = 0; i < _Countof(matrix); i++) {
        printf("Row %zu: ", i);
        for (size_t j = 0; j < _Countof(matrix[i]); j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    
    printf("\n--- String Array ---\n");
    const char *fruits[] = {"Apple", "Banana", "Cherry", "Date"};
    printf("Number of fruits: %zu\n", _Countof(fruits));
    print_strings(fruits, _Countof(fruits));
    
    printf("\n--- Safe Copy ---\n");
    int source[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int destination[5];
    
    copy_array(destination, source);
    printf("Source (%zu elements): ", _Countof(source));
    for (size_t i = 0; i < _Countof(source); i++) printf("%d ", source[i]);
    printf("\n");
    
    printf("Destination (%zu elements): ", _Countof(destination));
    for (size_t i = 0; i < _Countof(destination); i++) printf("%d ", destination[i]);
    printf("\n");
    
    printf("\n--- Error Prevention ---\n");
    printf("_Countof prevents pointer decay errors:\n");
    printf("int arr[10]; _Countof(arr) = %zu (correct)\n", _Countof(numbers));
    printf("int *p = arr; _Countof(p) would be a compile error\n");
    
    printf("\n_Countof demo complete!\n");
    
    return 0;
}
