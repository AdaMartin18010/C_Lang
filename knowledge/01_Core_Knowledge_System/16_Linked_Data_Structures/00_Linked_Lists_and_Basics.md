# C 语言链表与基础数据结构

> **难度**: L2-L4
> **核心目标**: 掌握手动内存管理下的数据结构实现
> **关键能力**: 指针操作、内存分配、边界条件处理

---

## 1. 单向链表 (Singly Linked List)

### 1.1 基础实现

```c
#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// 头插法：O(1)
Node *list_prepend(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return head;  // 内存不足，保持原状
    new_node->data = value;
    new_node->next = head;
    return new_node;
}

// 尾插法：O(n)
Node *list_append(Node *head, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return head;
    new_node->data = value;
    new_node->next = NULL;

    if (!head) return new_node;

    Node *curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = new_node;
    return head;
}

// 删除第一个匹配节点
Node *list_remove(Node *head, int value) {
    Node *curr = head, *prev = NULL;

    while (curr) {
        if (curr->data == value) {
            if (prev) {
                prev->next = curr->next;
            } else {
                head = curr->next;  // 删除头节点
            }
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    return head;  // 未找到
}

// 完全释放
void list_destroy(Node *head) {
    while (head) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
}
```

### 1.2 使用尾指针优化

```c
typedef struct {
    Node *head;
    Node *tail;   // 尾指针使尾插变为 O(1)
    size_t size;
} List;

void list_init(List *list) {
    list->head = list->tail = NULL;
    list->size = 0;
}

int list_push_back(List *list, int value) {
    Node *node = malloc(sizeof(Node));
    if (!node) return -1;
    node->data = value;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->size++;
    return 0;
}
```

---

## 2. 双向链表 (Doubly Linked List)

```c
typedef struct DNode {
    struct DNode *prev;
    struct DNode *next;
    int data;
} DNode;

typedef struct {
    DNode *head;
    DNode *tail;
} DList;

// O(1) 删除已知节点（无需遍历找前驱）
void dlist_remove(DList *list, DNode *node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    free(node);
}

// O(1) 在已知节点前插入
void dlist_insert_before(DList *list, DNode *target, int value) {
    DNode *node = malloc(sizeof(DNode));
    node->data = value;
    node->next = target;
    node->prev = target->prev;

    if (target->prev) {
        target->prev->next = node;
    } else {
        list->head = node;
    }
    target->prev = node;
}
```

---

## 3. 链表经典算法

### 3.1 检测环（Floyd 判圈算法）

```c
// 快慢指针检测环
int has_cycle(Node *head) {
    Node *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;       // 走一步
        fast = fast->next->next;  // 走两步
        if (slow == fast) return 1;  // 相遇则有环
    }
    return 0;
}

// 找到环的入口
Node *find_cycle_entry(Node *head) {
    Node *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    if (!fast || !fast->next) return NULL;  // 无环

    // 慢指针回到头，同速前进，相遇点即入口
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;
}
```

### 3.2 反转链表（迭代）

```c
Node *list_reverse(Node *head) {
    Node *prev = NULL, *curr = head, *next = NULL;

    while (curr) {
        next = curr->next;   // 保存下一个
        curr->next = prev;   // 反转指向
        prev = curr;         // 前移 prev
        curr = next;         // 前移 curr
    }
    return prev;  // 新头节点
}
```

### 3.3 找中点

```c
Node *list_find_middle(Node *head) {
    Node *slow = head, *fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;  // 慢指针指向中点
}
```

### 3.4 合并两个有序链表

```c
Node *list_merge_sorted(Node *a, Node *b) {
    Node dummy = {0, NULL};  // 哑节点简化边界处理
    Node *tail = &dummy;

    while (a && b) {
        if (a->data <= b->data) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }

    tail->next = a ? a : b;  // 连接剩余部分
    return dummy.next;
}
```

---

## 4. 泛型链表（宏模板）

```c
// intrusive_list.h — 侵入式链表宏

#define LIST_NODE(type) \
    struct { type *next; }

#define LIST_HEAD(name, type) \
    struct name { type *first; }

#define LIST_INIT(head) do { (head)->first = NULL; } while(0)

#define LIST_INSERT_HEAD(head, elem, field) do { \
    (elem)->field.next = (head)->first; \
    (head)->first = (elem); \
} while(0)

#define LIST_FOREACH(var, head, field) \
    for ((var) = (head)->first; (var); (var) = (var)->field.next)

// 使用
typedef struct Task {
    LIST_NODE(struct Task) link;
    int priority;
    void (*run)(void);
} Task;

LIST_HEAD(TaskList, Task);

TaskList ready_queue;
LIST_INIT(&ready_queue);

Task t1 = { .priority = 1, .run = task1_run };
LIST_INSERT_HEAD(&ready_queue, &t1, link);

Task *t;
LIST_FOREACH(t, &ready_queue, link) {
    printf("priority: %d\n", t->priority);
}
```

> Linux Kernel 的 `list.h` 是侵入式链表的工业级实现。

---

## 5. 与数组的对比

| 操作 | 数组 | 链表 |
|:-----|:-----|:-----|
| 随机访问 | O(1) | O(n) |
| 头插 | O(n) | O(1) |
| 尾插 | O(1) amortized | O(n) / O(1)* |
| 中间插入/删除 | O(n) | O(n) 查找 + O(1) 修改 |
| 内存占用 | 连续，可能浪费 | 分散，有指针开销 |
| 缓存友好 | ✅ 高 | ❌ 低（指针跳转） |

*使用尾指针优化后

---

## 6. 常见错误

```c
// ❌ 使用已释放的节点
Node *tmp = head;
free(tmp);
head = tmp->next;  // UAF！

// ✅ 先保存 next 再 free
Node *tmp = head;
head = head->next;
free(tmp);

// ❌ 丢失链表尾节点（内存泄漏）
Node *p = head;
while (p->next) {  // 如果 head 为 NULL，崩溃
    p = p->next;
}

// ✅ 总是检查空指针
if (!head) return;
```

---

> **最后更新**: 2026-05-11
> **参考**: Linux Kernel list.h, CSAPP Ch.3, K&R Ch.6
