/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 278
 * Language: c
 * Block ID: 917e5d85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 带缓冲区的异步IPC */

#define MSG_QUEUE_SIZE 256

typedef struct {
    message_t messages[MSG_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    spinlock_t lock;
    wait_queue_t waiters;
} msg_queue_t;

int async_send(int dest, message_t* msg) {
    thread_t* dest_thread = lookup_thread(dest);
    if (!dest_thread) return -EINVAL;

    msg_queue_t* queue = &dest_thread->msg_queue;

    spin_lock(&queue->lock);

    if (queue->count >= MSG_QUEUE_SIZE) {
        spin_unlock(&queue->lock);
        return -EAGAIN;  /* 队列满 */
    }

    /* 入队 */
    queue->messages[queue->tail] = *msg;
    queue->tail = (queue->tail + 1) % MSG_QUEUE_SIZE;
    queue->count++;

    /* 唤醒等待的接收者 */
    if (!wait_queue_empty(&queue->waiters)) {
        thread_t* waiter = dequeue_waiter(&queue->waiters);
        wakeup_thread(waiter);
    }

    spin_unlock(&queue->lock);
    return 0;
}

int async_receive(message_t* msg, int* sender) {
    thread_t* current = get_current_thread();
    msg_queue_t* queue = &current->msg_queue;

    spin_lock(&queue->lock);

    while (queue->count == 0) {
        /* 队列为空，阻塞等待 */
        current->state = THREAD_WAITING;
        enqueue_waiter(&queue->waiters, current);
        spin_unlock(&queue->lock);
        schedule();
        spin_lock(&queue->lock);
    }

    /* 出队 */
    *msg = queue->messages[queue->head];
    queue->head = (queue->head + 1) % MSG_QUEUE_SIZE;
    queue->count--;

    if (sender) *sender = msg->source;

    spin_unlock(&queue->lock);
    return 0;
}
