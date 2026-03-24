# 垃圾收集算法：形式化理论与工程实现

> **层级**: L6+
> **引用**: Jones & Lins (1996), Dijkstra (1978), Yuasa (1990)

---

## 【思维导图·GC理论体系】

```
垃圾收集
├── 标记-清除
│   ├── 三色标记
│   ├── 增量标记
│   └── 并发标记
├── 复制收集
│   ├── 半区复制
│   ├── 分代收集
│   └── Appel式
├── 引用计数
│   ├── 简单RC
│   ├── 周期检测
│   └── 延迟RC
└── 高级技术
    ├── 写屏障
    ├── 读屏障
    └── 栈扫描
```

---

## 【科学·GC的形式化】

### 1. 可达性语义

**定义**: 堆对象o是可达的如果存在从根到o的路径。

**形式化**:

```
Roots ::= StackVars ∪ GlobalVars ∪ Registers

Reachable(Roots, Heap) =
  { o ∈ Heap | ∃r ∈ Roots. r →* o }
```

**安全条件**:

```
∀o ∈ Freed. o ∉ Reachable(Roots, Heap)
```

### 2. 三色标记算法

**状态机**:

```
White: 未访问
Grey: 已访问，子节点未访问
Black: 已访问，子节点已访问
```

**不变式**:

```
∀b ∈ Black. ∀o ∈ children(b). o ∈ Black ∪ Grey
```

**算法**:

```
1. 初始化: 所有对象为White，根为Grey
2. while Grey ≠ ∅:
   o = pop(Grey)
   for child in children(o):
     if child is White:
       mark Grey
   mark o Black
3. 所有White对象可回收
```

### 3. 分代收集理论

**弱分代假设**:

```
P(对象存活 | 年龄=t) 随 t 增加而增加
```

**代区划分**:

```
Heap = Nursery ∪ OldGen
Nursery: 新分配，频繁GC
OldGen: 长期存活，少量GC
```

**晋升策略**:

```
if obj.survive_count > threshold:
  promote_to_oldgen(obj)
```

---

## 【工程·GC实现】

### 1. 写屏障

**目的**: 维护标记不变式

**Dijkstra写屏障**:

```c
void write_barrier(Object* obj, Object* ref) {
  obj->field = ref;
  if (is_black(obj) && is_white(ref)) {
    shade(ref);  // 将ref标记为Grey
  }
}
```

**Yuasa读屏障**:

```c
Object* read_barrier(Object* obj, int field) {
  Object* ref = obj->field;
  if (is_grey(obj)) {
    shade(ref);
  }
  return ref;
}
```

### 2. Go的并发GC

**设计**:

- 并发标记（与用户程序并行）
- 写屏障保护
- 栈扫描

**流程**:

```
1. STW: 准备（< 100μs）
2. 并发标记
3. STW: 重新扫描（< 100μs）
4. 并发清除
```

### 3. Lean 4的引用计数

**优化**: 借用检查减少RC操作

**代码**:

```c
void lean_inc(lean_object* o) {
  if (! lean_is_scalar(o)) {
    atomic_fetch_add(&o->m_rc, 1);
  }
}

void lean_dec(lean_object* o) {
  if (! lean_is_scalar(o)) {
    if (atomic_fetch_sub(&o->m_rc, 1) == 1) {
      lean_free(o);
    }
  }
}
```

---

## 【多维矩阵·GC算法对比】

```
算法          吞吐量    延迟      内存碎片    移动对象    复杂度
─────────────────────────────────────────────────────────────────
标记-清除      高       中       高         否         低
复制          中       低       无         是         中
标记-整理      中       中       低         是         高
引用计数       高       极低      无         否         低
分代          极高      低       中         是         高
并发标记       中       极低      中         部分       极高
```

---

## 【定理·GC安全性】

**定理**: 正确实现的GC不会回收可达对象。

**证明**:

```
假设GC回收了可达对象o。
则o ∈ Reachable(Roots, Heap)。
由可达性定义，存在根r使得r →* o。
由标记算法，所有可达对象最终变为Black。
White对象（可回收）与Black对象不相交。
矛盾！∎
```

---

## 参考

- Jones, R. & Lins, R. "Garbage Collection" (1996)
- Dijkstra, E.W. "On-the-Fly Garbage Collection" (1978)
- Yuasa, T. "Real-Time Garbage Collection" (1990)
