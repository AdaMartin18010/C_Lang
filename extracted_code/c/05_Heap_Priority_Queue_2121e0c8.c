/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\05_Heap_Priority_Queue.md
 * Line: 495
 * Language: c
 * Block ID: 2121e0c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 堆排序 - 升序 (使用最大堆)
 * 时间复杂度: O(n log n)
 * 空间复杂度: O(1) (原地排序)
 */
void heap_sort_ascending(int* arr, size_t n) {
    if (n <= 1) return;

    // 1. 建最大堆 (从最后一个非叶子节点开始)
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        // 下沉操作
        size_t parent = (size_t)i;
        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t largest = parent;

            if (left < n && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < n && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest == parent) break;

            // 交换
            int temp = arr[parent];
            arr[parent] = arr[largest];
            arr[largest] = temp;

            parent = largest;
        }
    }

    // 2. 排序
    for (int i = (int)(n - 1); i > 0; i--) {
        // 将堆顶(最大值)移到末尾
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // 对减小后的堆进行调整
        size_t parent = 0;
        size_t heap_size = (size_t)i;

        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t largest = parent;

            if (left < heap_size && arr[left] > arr[largest]) {
                largest = left;
            }
            if (right < heap_size && arr[right] > arr[largest]) {
                largest = right;
            }

            if (largest == parent) break;

            temp = arr[parent];
            arr[parent] = arr[largest];
            arr[largest] = temp;

            parent = largest;
        }
    }
}

/**
 * 堆排序 - 降序 (使用最小堆)
 */
void heap_sort_descending(int* arr, size_t n) {
    if (n <= 1) return;

    // 建最小堆
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        size_t parent = (size_t)i;
        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t smallest = parent;

            if (left < n && arr[left] < arr[smallest]) {
                smallest = left;
            }
            if (right < n && arr[right] < arr[smallest]) {
                smallest = right;
            }

            if (smallest == parent) break;

            int temp = arr[parent];
            arr[parent] = arr[smallest];
            arr[smallest] = temp;

            parent = smallest;
        }
    }

    // 排序
    for (int i = (int)(n - 1); i > 0; i--) {
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        size_t parent = 0;
        size_t heap_size = (size_t)i;

        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t smallest = parent;

            if (left < heap_size && arr[left] < arr[smallest]) {
                smallest = left;
            }
            if (right < heap_size && arr[right] < arr[smallest]) {
                smallest = right;
            }

            if (smallest == parent) break;

            temp = arr[parent];
            arr[parent] = arr[smallest];
            arr[smallest] = temp;

            parent = smallest;
        }
    }
}
