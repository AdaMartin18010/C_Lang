/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 840
 * Language: c
 * Block ID: 21c49232
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线程池任务并行
#include <pthread.h>

typedef struct {
    void (*func)(void *);
    void *arg;
} Task;

void parallel_process_tree(Node *root) {
    if (root == NULL) return;

    // 大任务并行化
    if (root->size > THRESHOLD) {
        pthread_t left_thread;
        pthread_create(&left_thread, NULL,
                       (void*)parallel_process_tree, root->left);

        parallel_process_tree(root->right);

        pthread_join(left_thread, NULL);
    } else {
        // 小任务串行
        parallel_process_tree(root->left);
        parallel_process_tree(root->right);
    }

    process_node(root);
}
