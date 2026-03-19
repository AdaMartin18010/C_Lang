/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 411
 * Language: c
 * Block ID: e67f80be
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 条件变量使用模式 */
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static struct queue work_queue;

void producer(struct work_item *item)
{
    pthread_mutex_lock(&queue_mutex);
    queue_push(&work_queue, item);
    pthread_cond_signal(&queue_cond);  /* 通知消费者 */
    pthread_mutex_unlock(&queue_mutex);
}

void consumer(void)
{
    pthread_mutex_lock(&queue_mutex);
    while (queue_empty(&work_queue)) {
        pthread_cond_wait(&queue_cond, &queue_mutex);  /* 原子释放锁并等待 */
    }
    struct work_item *item = queue_pop(&work_queue);
    pthread_mutex_unlock(&queue_mutex);

    process(item);
}
