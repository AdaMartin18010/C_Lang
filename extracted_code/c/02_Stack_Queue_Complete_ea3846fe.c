/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 767
 * Language: c
 * Block ID: ea3846fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 链表队列节点
typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;    // 队首指针
    QueueNode* rear;     // 队尾指针
    size_t size;
} LinkedQueue;

// 初始化
void linked_queue_init(LinkedQueue* queue) {
    queue->front = nullptr;
    queue->rear = nullptr;
    queue->size = 0;
}

// 销毁
void linked_queue_destroy(LinkedQueue* queue) {
    QueueNode* current = queue->front;
    while (current != nullptr) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    queue->front = nullptr;
    queue->rear = nullptr;
    queue->size = 0;
}

// 入队 - 链表尾部插入
bool linked_queue_enqueue(LinkedQueue* queue, int value) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (new_node == nullptr) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return false;
    }
    new_node->data = value;
    new_node->next = nullptr;

    if (queue->rear == nullptr) {
        // 空队列
        queue->front = queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
    return true;
}

// 出队 - 链表头部删除
bool linked_queue_dequeue(LinkedQueue* queue, int* out_value) {
    if (queue->front == nullptr) {
        fprintf(stderr, "Error: Queue is empty\n");
        return false;
    }

    QueueNode* temp = queue->front;
    *out_value = temp->data;
    queue->front = temp->next;

    if (queue->front == nullptr) {
        queue->rear = nullptr;  // 队列变空
    }

    free(temp);
    queue->size--;
    return true;
}

// 查看队首
bool linked_queue_front(const LinkedQueue* queue, int* out_value) {
    if (queue->front == nullptr) {
        fprintf(stderr, "Error: Queue is empty\n");
        return false;
    }
    *out_value = queue->front->data;
    return true;
}

// 打印队列
void linked_queue_print(const LinkedQueue* queue) {
    printf("LinkedQueue [size=%zu]: front -> ", queue->size);
    QueueNode* current = queue->front;
    while (current != nullptr) {
        printf("[%d]", current->data);
        if (current->next != nullptr) printf(" -> ");
        current = current->next;
    }
    printf(" <- rear\n");
}
