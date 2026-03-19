/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\05_Thread_Pool_Patterns.md
 * Line: 276
 * Language: c
 * Block ID: a29bcbe2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct fj_task {
    void (*compute)(struct fj_task*);
    void (*fork)(struct fj_task*);
    void* (*join)(struct fj_task*);

    atomic_int status;  /* 0=pending, 1=running, 2=done */
    struct fj_task* parent;
    struct fj_task* subtasks[2];
    atomic_int completed_subtasks;

    void* result;
} fj_task_t;

void fj_fork(fj_task_t* task) {
    atomic_store(&task->status, 1);
    /* 提交到线程池 */
    submit_to_pool(task);
}

void* fj_join(fj_task_t* task) {
    /* 如果任务未完成，尝试窃取其他工作 */
    while (atomic_load(&task->status) != 2) {
        work_item_t stolen;
        if (try_steal(&stolen)) {
            /* 执行窃取的任务 */
            stolen.fn(stolen.data);
        } else {
            thrd_yield();
        }
    }
    return task->result;
}

void fj_complete(fj_task_t* task) {
    atomic_store(&task->status, 2);

    if (task->parent) {
        int completed = atomic_fetch_add(&task->parent->completed_subtasks, 1);
        if (completed == 1) {  /* 两个子任务都完成 */
            /* 调度父任务 */
            fj_fork(task->parent);
        }
    }
}

/* 示例: 并行快速排序 */
void quicksort_fj(fj_task_t* task) {
    int* arr = task->data;
    int n = task->n;

    if (n < 1000) {
        /* 小数组直接排序 */
        qsort(arr, n, sizeof(int), compare_int);
        fj_complete(task);
        return;
    }

    /* 分区 */
    int pivot = partition(arr, n);

    /* 创建子任务 */
    fj_task_t* left = create_fj_task(quicksort_fj, arr, pivot);
    fj_task_t* right = create_fj_task(quicksort_fj, arr + pivot + 1, n - pivot - 1);

    left->parent = task;
    right->parent = task;
    task->subtasks[0] = left;
    task->subtasks[1] = right;

    /* Fork */
    fj_fork(left);
    fj_fork(right);
}
