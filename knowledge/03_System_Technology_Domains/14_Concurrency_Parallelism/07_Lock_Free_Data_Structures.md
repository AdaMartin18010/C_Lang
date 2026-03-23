# 无锁数据结构

> **层级定位**: 03 System Technology Domains / 14 Concurrency Parallelism
> **难度级别**: L5 专家
> **预估学习时间**: 10-12 小时

---

## 1. 无锁编程基础

### 1.1 CAS操作

```c
#include <stdatomic.h>

// 比较并交换
_Bool atomic_compare_exchange_strong(
    atomic_int *obj,
    int *expected,
    int desired);

// 使用示例
atomic_int counter = ATOMIC_VAR_INIT(0);

int expected = atomic_load(&counter);
int desired = expected + 1;

while (!atomic_compare_exchange_strong(&counter, &expected, desired)) {
    // 失败，expected被更新为当前值
    desired = expected + 1;
}
```

### 1.2 ABA问题

```c
// ABA问题示例
// 线程1: 读取A → 被抢占
// 线程2: A→B→A（修改后恢复）
// 线程1: CAS成功，但链表结构已变

// 解决方案：带标签的指针
struct tagged_ptr {
    void *ptr;
    uint64_t tag;  // 版本号
};

_Bool cas_tagged(struct tagged_ptr *loc,
                 struct tagged_ptr expected,
                 struct tagged_ptr desired) {
    // 使用128位CAS (cmpxchg16b on x86_64)
}
```

---

## 2. 无锁队列

### 2.1 Michael-Scott队列

```c
#include <stdatomic.h>
#include <stdlib.h>

typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

void queue_init(LockFreeQueue *q) {
    Node *dummy = malloc(sizeof(Node));
    dummy->next = NULL;
    atomic_store(&q->head, dummy);
    atomic_store(&q->tail, dummy);
}

void enqueue(LockFreeQueue *q, void *data) {
    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;

    Node *tail;
    Node *next;

    while (1) {
        tail = atomic_load(&q->tail);
        next = atomic_load(&tail->next);

        // 检查tail是否仍是tail
        if (tail == atomic_load(&q->tail)) {
            if (next == NULL) {
                // 尝试链接新节点
                if (atomic_compare_exchange_strong(
                        &tail->next, &next, node)) {
                    // 成功，尝试更新tail
                    atomic_compare_exchange_strong(
                        &q->tail, &tail, node);
                    return;
                }
            } else {
                // tail落后，尝试帮助更新
                atomic_compare_exchange_strong(
                    &q->tail, &tail, next);
            }
        }
    }
}

void* dequeue(LockFreeQueue *q) {
    Node *head;
    Node *tail;
    Node *next;

    while (1) {
        head = atomic_load(&q->head);
        tail = atomic_load(&q->tail);
        next = atomic_load(&head->next);

        if (head == atomic_load(&q->head)) {
            if (head == tail) {
                // 可能为空
                if (next == NULL) return NULL;
                // tail落后
                atomic_compare_exchange_strong(
                    &q->tail, &tail, next);
            } else {
                void *data = next->data;
                // 尝试更新head
                if (atomic_compare_exchange_strong(
                        &q->head, &head, next)) {
                    // 注意：此时head节点逻辑上已删除
                    // 实际释放需要Hazard Pointer或RCU
                    return data;
                }
            }
        }
    }
}
```

---

## 3. 无锁栈

### 3.1 Treiber栈

```c
typedef struct Node {
    void *data;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) top;
} LockFreeStack;

void stack_init(LockFreeStack *s) {
    atomic_store(&s->top, NULL);
}

void push(LockFreeStack *s, void *data) {
    Node *node = malloc(sizeof(Node));
    node->data = data;

    Node *old_top;
    do {
        old_top = atomic_load(&s->top);
        node->next = old_top;
    } while (!atomic_compare_exchange_strong(
        &s->top, &old_top, node));
}

void* pop(LockFreeStack *s) {
    Node *old_top;
    Node *new_top;

    do {
        old_top = atomic_load(&s->top);
        if (old_top == NULL) return NULL;
        new_top = atomic_load(&old_top->next);
    } while (!atomic_compare_exchange_strong(
        &s->top, &old_top, new_top));

    void *data = old_top->data;
    // 需要延迟释放
    return data;
}
```

---

## 4. 内存回收

### 4.1 Hazard Pointers

```c
#define MAX_THREADS 64
#define HP_PER_THREAD 3

_Atomic(Node*) hazard_pointers[MAX_THREADS][HP_PER_THREAD];
_Atomic(Node*) retired_list;

void hp_record(int thread_id, int hp_index, Node *ptr) {
    atomic_store(&hazard_pointers[thread_id][hp_index], ptr);
    atomic_thread_fence(memory_order_seq_cst);
}

void hp_clear(int thread_id, int hp_index) {
    atomic_store(&hazard_pointers[thread_id][hp_index], NULL);
}

void retire_node(Node *node) {
    // 添加到退休列表
    Node *old;
    do {
        old = atomic_load(&retired_list);
        node->next = old;
    } while (!atomic_compare_exchange_strong(
        &retired_list, &old, node));

    // 尝试扫描回收
    scan_retired();
}

void scan_retired() {
    // 收集所有hazard指针
    Node *hp_set[MAX_THREADS * HP_PER_THREAD];
    int hp_count = 0;

    for (int i = 0; i < MAX_THREADS; i++) {
        for (int j = 0; j < HP_PER_THREAD; j++) {
            Node *hp = atomic_load(&hazard_pointers[i][j]);
            if (hp != NULL) hp_set[hp_count++] = hp;
        }
    }

    // 检查退休列表
    // 不在hp_set中的可以安全释放
}
```

---

## 5. 无锁哈希表

### 5.1 分割有序哈希表

```c
#define NUM_BUCKETS 1024

typedef struct HashEntry {
    uint64_t key;
    void *value;
    _Atomic(struct HashEntry*) next;
} HashEntry;

typedef struct {
    _Atomic(HashEntry*) buckets[NUM_BUCKETS];
} LockFreeHashTable;

uint64_t hash(uint64_t key) {
    return key % NUM_BUCKETS;
}

void* lookup(LockFreeHashTable *ht, uint64_t key) {
    uint64_t h = hash(key);
    HashEntry *entry = atomic_load(&ht->buckets[h]);

    while (entry != NULL) {
        if (entry->key == key) return entry->value;
        entry = atomic_load(&entry->next);
    }
    return NULL;
}
```

---

## 关联导航

### 前置知识

- [C11内存模型](../../02_Formal_Semantics_and_Physics/01_Game_Semantics/02_C11_Memory_Model.md)
- [原子操作](./03_Atomic_Operations.md)
- [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)

### 后续延伸

- [RCU机制](./08_RCU_Mechanism.md)
- [并发数据结构](https://github.com/concurrencykit/ck)

### 参考实现

- [Concurrency Kit](https://github.com/concurrencykit/ck)
- [liblfds](http://www.liblfds.org/)
