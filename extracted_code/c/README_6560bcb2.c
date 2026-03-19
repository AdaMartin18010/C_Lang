/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 793
 * Language: c
 * Block ID: 6560bcb2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 排序策略接口
typedef struct {
    void (*sort)(int* arr, int n);
    const char* name;
} SortStrategy;

// 具体策略: 快速排序
static void quick_sort_impl(int* arr, int left, int right) {
    if (left >= right) return;
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            i++; j--;
        }
    }
    quick_sort_impl(arr, left, j);
    quick_sort_impl(arr, i, right);
}

static void quick_sort(int* arr, int n) {
    quick_sort_impl(arr, 0, n - 1);
}

static const SortStrategy quick_sort_strategy = {
    .sort = quick_sort,
    .name = "QuickSort"
};

// 具体策略: 归并排序
static void merge_sort_impl(int* arr, int* temp, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_impl(arr, temp, left, mid);
    merge_sort_impl(arr, temp, mid + 1, right);

    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        temp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (i = left; i <= right; i++) arr[i] = temp[i];
}

static void merge_sort(int* arr, int n) {
    int* temp = malloc(n * sizeof(int));
    merge_sort_impl(arr, temp, 0, n - 1);
    free(temp);
}

static const SortStrategy merge_sort_strategy = {
    .sort = merge_sort,
    .name = "MergeSort"
};

// 上下文: 排序器
typedef struct {
    const SortStrategy* strategy;
} Sorter;

void sorter_set_strategy(Sorter* sorter, const SortStrategy* strategy) {
    sorter->strategy = strategy;
}

void sorter_execute(Sorter* sorter, int* arr, int n) {
    printf("Using %s...\n", sorter->strategy->name);
    sorter->strategy->sort(arr, n);
}

// 使用
void strategy_example(void) {
    Sorter sorter = { &quick_sort_strategy };
    int data[] = {5, 2, 8, 1, 9};

    // 使用快速排序
    sorter_execute(&sorter, data, 5);

    // 切换为归并排序
    sorter_set_strategy(&sorter, &merge_sort_strategy);
    sorter_execute(&sorter, data, 5);
}
