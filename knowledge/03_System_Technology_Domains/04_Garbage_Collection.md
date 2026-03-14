# C语言垃圾回收机制

## 1. 手动内存管理 vs 垃圾回收

### 1.1 手动管理的挑战

```c
int* ptr = (int*)malloc(sizeof(int) * 100);
// 使用内存...
free(ptr);      // 必须手动释放
```

| 问题 | 后果 |
|------|------|
| 内存泄漏 | 内存耗尽 |
| 悬空指针 | 未定义行为 |
| 双重释放 | 堆损坏 |

### 1.2 垃圾回收优势

```c
int* ptr = (int*)GC_malloc(sizeof(int) * 100);
// 使用内存，无需释放
```

**GC核心优势**：消除内存泄漏、减少管理代码、降低崩溃率。

### 1.3 对比

| 特性 | 手动管理 | 垃圾回收 |
|------|---------|---------|
| 运行时开销 | 低 | 中等 |
| 开发复杂度 | 高 | 低 |
| 实时性 | 适合 | 可能引入暂停 |

---

## 2. 保守式垃圾回收

### 2.1 Boehm GC

**Boehm-Demers-Weiser GC** 是C/C++最著名的保守式GC。

**工作原理**：扫描根→识别指针→标记可达→回收不可达。

**特点**：可能误判整数为指针；支持增量/并发GC；支持分代收集。

### 2.2 使用示例

```bash
sudo apt-get install libgc-dev    # Linux
brew install bdw-gc               # macOS
```

```c
#include <gc.h>

int main() {
    GC_INIT();
    int* arr = (int*)GC_malloc(100 * sizeof(int));
    // 使用... 无需free
    return 0;
}
```

**编译**：`gcc program.c -o program -lgc`

---

## 3. GC算法对比

### 3.1 标记-清除（Mark-Sweep）

标记可达对象，清除未标记对象。

**特点**：实现简单，产生内存碎片，暂停时间长。

### 3.2 复制（Copying）

将存活对象复制到另一半空间，交换角色。

**特点**：无碎片，分配快；内存利用率仅50%。

### 3.3 标记-整理（Mark-Compact）

标记后将存活对象压缩到内存一端。

**特点**：无碎片，空间利用率高；整理开销大。

### 3.4 分代收集（Generational）

基于"弱分代假说"——大多数对象很快死亡。

```
Heap {
    YoungGen { Eden, Survivor0, Survivor1 }  // 复制算法
    OldGen                                    // 标记-整理
}
```

- **Minor GC**：频繁收集新生代
- **Major GC**：较少频率收集老年代

### 3.5 算法对比

| 算法 | 空间开销 | 暂停时间 | 碎片 |
|------|---------|---------|------|
| 标记-清除 | 低 | 长 | 有 |
| 复制 | 50% | 短 | 无 |
| 标记-整理 | 低 | 较长 | 无 |
| 分代 | 中 | 平均短 | 可控 |

---

## 4. 在C中集成GC

### 4.1 链表示例

```c
#include <gc.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* create_node(int data) {
    Node* node = (Node*)GC_malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    return node;
}

void append(Node** head, int data) {
    Node* new_node = create_node(data);
    if (*head == NULL) { *head = new_node; return; }
    Node* cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = new_node;
}

int main() {
    GC_INIT();
    Node* list = NULL;
    for (int i = 0; i < 100; i++) append(&list, i);

    // 断开前50个节点引用
    Node* temp = list;
    for (int i = 0; i < 50; i++) temp = temp->next;
    list = temp;

    GC_gcollect();  // 触发GC回收
    return 0;
}
```

### 4.2 混合策略

```c
void* gc_alloc(size_t size) { return GC_malloc(size); }
void* manual_alloc(size_t size) { return malloc(size); }

int main() {
    GC_INIT();
    char* temp = (char*)gc_alloc(1024);      // GC管理
    void* critical = manual_alloc(1024);      // 手动管理
    free(critical);   // 必须释放
    return 0;
}
```

### 4.3 GC配置

```c
void configure_gc() {
    GC_set_max_heap_size(100 * 1024 * 1024);  // 最大100MB
    GC_enable_incremental();                   // 启用增量GC
}
```

---

## 5. 性能与可靠性权衡

### 5.1 性能开销

| 开销类型 | 典型影响 |
|---------|---------|
| 分配开销 | 比malloc慢5-20% |
| 暂停时间 | 毫秒到秒级 |
| 内存开销 | 额外10-50% |

### 5.2 场景选择

| 场景 | 推荐方案 |
|------|---------|
| 实时/嵌入式系统 | 手动管理 |
| 桌面应用 | Boehm GC |
| 高性能计算 | 手动管理 |

### 5.3 最佳实践

- **对象池化**：频繁分配的对象使用对象池
- **大对象处理**：手动管理避免GC压力
- **及时断引用**：避免内存滞留

---

## 6. 总结

- **保守式GC**（Boehm GC）是C项目引入GC的最佳选择
- **算法选择**：复制适合新生代，标记-整理适合老年代
- **混合策略**：关键路径手动管理，非关键路径使用GC
- **性能敏感场景**仍建议手动管理

---

## 参考资源

- [Boehm GC官方文档](https://www.hboehm.info/gc/)
- 《垃圾回收的算法与实现》- 中村成洋
- [GC Handbook](http://gchandbook.org/)

---

> **状态**: ✅ 已完成
