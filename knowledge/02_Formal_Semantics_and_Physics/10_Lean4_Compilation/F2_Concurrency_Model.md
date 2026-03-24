# Lean 4并发模型：任务并行与并行运行时

> **层级**: L6 (元理论层)
> **引用**: Blumofe et al. (1995), Arora et al. (1998), Muller et al. (2020)
> **课程对标**: MIT 6.824, CMU 15-618

---

## 一、并发理论基础

### 1.1 并发vs并行

**定义 1.1** (并发与并行)

- **并发 (Concurrency)**：多个任务的交替执行（逻辑同时）
- **并行 (Parallelism)**：多个任务的真正同时执行（物理同时）

**关系**：

```
并行 ⇒ 并发
并发 ⇏ 并行
```

### 1.2 进程代数

**CSP (Communicating Sequential Processes)**：

- 进程：独立执行单元
- 通道：进程间通信
- 事件：同步点

**Pi-演算**：

- 移动性：通道可以作为数据传递
- 名称：通信端点的标识

---

## 二、Lean 4任务并行模型

### 2.1 Task类型

**定义 2.1** (Task)

```lean
Task α : Type
```

表示一个将在未来完成的计算，类型为α。

**操作**：

```lean
-- 创建任务
Task.spawn (f : Unit → α) : Task α

-- 获取结果（阻塞）
Task.get (t : Task α) : α

-- 检查完成
Task.isFinished (t : Task α) : Bool
```

### 2.2 工作窃取调度

**算法** (Blumofe & Leiserson, 1999)：

每个工作线程有双端队列 (deque)：

- **本地操作**：push/pop从队列尾部
- **窃取操作**：其他线程从队列头部窃取

**定理 2.2** (工作窃取效率)

对于完全严格(fully strict)的计算，
工作窃取调度器的期望时间：

```
T_p ≤ T_1/P + O(T_∞)
```

其中：

- T_1：串行时间
- T_p：P处理器并行时间
- T_∞：关键路径长度

**Lean 4实现**：

```c
typedef struct {
    Task** deque;
    atomic_size_t top, bottom;
    pthread_mutex_t lock;
} WorkerDeque;

void worker_loop(WorkerDeque* deque) {
    while (true) {
        Task* task = pop_local(deque);
        if (task == NULL) {
            task = steal_from_other(deque);
        }
        if (task != NULL) {
            execute(task);
        }
    }
}
```

---

## 三、并行集合操作

### 3.1 Parallel For

```lean
def parallelFor (n : Nat) (f : Fin n → IO Unit) : IO Unit := do
  let tasks ← (List.range n).mapM fun i =>
    IO.asTask (f ⟨i, by simp⟩)
  tasks.forM IO.wait
```

**调度策略**：

- 小n：顺序执行（避免开销）
- 大n：分治递归，阈值后并行

### 3.2 并行归约

```lean
def parallelFold (f : α → α → α) (neutral : α)
                 (xs : Array α) : Task α := do
  if xs.size < threshold then
    return Task.pure (xs.foldl f neutral)
  else
    let mid := xs.size / 2
    let left ← parallelFold f neutral xs[:mid]
    let right ← parallelFold f neutral xs[mid:]
    return Task.map2 f left right
```

---

## 四、原子操作与内存模型

### 4.1 C11内存序

**内存序级别**：

- **relaxed**：无同步保证
- **consume**：依赖同步（少用）
- **acquire**：获取语义（读）
- **release**：释放语义（写）
- **acq_rel**：获取+释放
- **seq_cst**：顺序一致性

### 4.2 Lean 4原子操作

```lean
structure Atomic α where
  private mk ::
  private ref : IO.Ref α

def Atomic.get (a : Atomic α) : IO α :=
  a.ref.get

def Atomic.set (a : Atomic α) (v : α) : IO Unit :=
  a.ref.set v

def Atomic.compareAndSet (a : Atomic α) (expected new : α) : IO Bool :=
  a.ref.compareAndSet expected new
```

**实现**：

```c
bool atomic_compare_exchange(
    atomic_int* obj,
    int* expected,
    int desired
) {
    return atomic_compare_exchange_explicit(
        obj, expected, desired,
        memory_order_acq_rel,
        memory_order_acquire
    );
}
```

---

## 五、生成垃圾收集

### 5.1 分代GC理论

**弱分代假设**：

- 大部分对象很快死亡
- 存活久的对象会继续存活

**代区划分**：

- **新生代**：新分配的对象
- **老年代**：经过多次GC仍存活的对象

### 5.2 Lean 4 GC架构

**设计决策**：

- 增量标记-清除
- 并行标记
- 并发标记（计划中）

**算法**：

```
1. 根集合标记
2. 并行标记（工作窃取）
3. 清扫阶段
4. 内存归还
```

---

**参考文献**：

- Blumofe, R.D. et al. "Cilk: An Efficient Multithreaded Runtime System" (1995)
- Arora, N.S. et al. "Thread Scheduling for Multiprogrammed Multiprocessors" (1998)
- Muller, S. et al. "Compiling a Functional Language with Work-Stealing" (2020)
