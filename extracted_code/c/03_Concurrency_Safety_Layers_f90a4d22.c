/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Concurrency_Safety_Layers.md
 * Line: 372
 * Language: c
 * Block ID: f90a4d22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    _Atomic(void *) buffer[QUEUE_SIZE];
    atomic_int head;
    atomic_int tail;
} MPSCQueue;

// 多生产者入队
bool enqueue(MPSCQueue *q, void *item) {
    int tail = atomic_fetch_add(&q->tail, 1);
    if (tail - atomic_load(&q->head) >= QUEUE_SIZE) {
        // 队列满
        atomic_fetch_sub(&q->tail, 1);
        return false;
    }
    atomic_store_explicit(&q->buffer[tail % QUEUE_SIZE],
                         item, memory_order_release);
    return true;
}

// 单消费者出队
void* dequeue(MPSCQueue *q) {
    int head = atomic_load(&q->head);
    if (head == atomic_load(&q->tail)) {
        return NULL;  // 空队列
    }
    void *item = atomic_load_explicit(
        &q->buffer[head % QUEUE_SIZE], memory_order_acquire);
    atomic_fetch_add(&q->head, 1);
    return item;
}
