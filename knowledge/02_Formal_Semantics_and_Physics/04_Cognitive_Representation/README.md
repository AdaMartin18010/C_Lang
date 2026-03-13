# 认知表示与知识表示

> **核心命题**: 人类认知与机器智能的交汇点在于"表示"。
> 理解知识如何在心智中表示、如何在代码中体现，是设计直观系统、编写可维护代码、创造真正智能的基础。

---

## 目录

- [认知表示与知识表示](#认知表示与知识表示)
  - [目录](#目录)
  - [1. 知识表示基础](#1-知识表示基础)
    - [1.1 知识表示的必要性](#11-知识表示的必要性)
    - [1.2 语义网络](#12-语义网络)
    - [1.3 框架表示](#13-框架表示)
    - [1.4 脚本表示](#14-脚本表示)
  - [2. 概念图与本体论](#2-概念图与本体论)
    - [2.1 概念图 (Conceptual Graphs)](#21-概念图-conceptual-graphs)
    - [2.2 本体论工程](#22-本体论工程)
  - [3. 心智模型理论](#3-心智模型理论)
    - [3.1 心智模型是什么](#31-心智模型是什么)
    - [3.2 程序员的心智模型](#32-程序员的心智模型)
  - [4. 编程中的心智模型](#4-编程中的心智模型)
    - [4.1 执行模型](#41-执行模型)
    - [4.2 并发心智模型](#42-并发心智模型)
    - [4.3 类型系统心智模型](#43-类型系统心智模型)
  - [5. 代码认知复杂度](#5-代码认知复杂度)
    - [5.1 圈复杂度 (Cyclomatic Complexity)](#51-圈复杂度-cyclomatic-complexity)
    - [5.2 认知复杂度 (Cognitive Complexity)](#52-认知复杂度-cognitive-complexity)
    - [5.3 代码可读性原则](#53-代码可读性原则)
  - [6. 编程语言设计的认知维度](#6-编程语言设计的认知维度)
    - [6.1 认知维度框架](#61-认知维度框架)
    - [6.2 语言特性与心智模型](#62-语言特性与心智模型)
    - [6.3 语法设计原则](#63-语法设计原则)
  - [7. 参考资料](#7-参考资料)
    - [经典文献](#经典文献)
    - [在线资源](#在线资源)
    - [工具](#工具)

---

## 1. 知识表示基础

### 1.1 知识表示的必要性

```
┌─────────────────────────────────────────────────────────────┐
│                  知识表示的核心问题                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   世界 ──→ 感知 ──→ 表示 ──→ 推理 ──→ 行动 ──→ 世界         │
│         (认知)    (符号)   (计算)   (物理)                   │
│                                                             │
│   关键问题:                                                 │
│   1. 什么值得表示? (本体承诺)                                │
│   2. 如何有效表示? (表达力 vs 效率)                          │
│   3. 如何有效推理? (计算复杂性)                              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**知识表示方案对比:**

| 表示方法 | 优点 | 缺点 | 适用场景 |
|:---------|:-----|:-----|:---------|
| 语义网络 | 直观, 关系显式 | 推理复杂 | 概念关联 |
| 框架 | 结构化, 继承 | 刚性结构 | 典型情境 |
| 脚本 | 时间序列 | 过于具体 | 事件序列 |
| 规则系统 | 可解释 | 组合爆炸 | 专家系统 |
| 逻辑 | 严密, 可证明 | 计算昂贵 | 形式验证 |

### 1.2 语义网络

语义网络用**节点**表示概念，用**边**表示关系。

```
                    ┌───────────┐
                    │   动物    │
                    └─────┬─────┘
                          │ is-a
           ┌──────────────┼──────────────┐
           ↓              ↓              ↓
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │   鸟    │    │   鱼    │    │   狗    │
      └────┬────┘    └────┬────┘    └────┬────┘
             │ can        │ can          │ can
             ↓            ↓              ↓
      ┌─────────┐    ┌─────────┐    ┌─────────┐
      │   飞    │    │   游    │    │   跑    │
      └─────────┘    └─────────┘    └─────────┘
             ↑            ↑              ↑
             └────────────┴──────────────┘
                          │ can
                    ┌─────────┐
                    │  运动   │
                    └─────────┘
```

```python
# Python 中的语义网络实现
class SemanticNetwork:
    def __init__(self):
        self.nodes = {}      # 概念节点
        self.edges = []      # 关系边

    def add_concept(self, name, properties=None):
        self.nodes[name] = {
            'properties': properties or {},
            'relations': []
        }

    def add_relation(self, from_node, relation, to_node):
        edge = (from_node, relation, to_node)
        self.edges.append(edge)
        self.nodes[from_node]['relations'].append(edge)

    def query(self, start, relation_type=None, depth=3):
        """关系查询与推理"""
        results = []
        visited = set()

        def traverse(node, current_depth):
            if current_depth > depth or node in visited:
                return
            visited.add(node)

            for edge in self.nodes[node]['relations']:
                from_n, rel, to_n = edge
                if relation_type is None or rel == relation_type:
                    results.append(edge)
                traverse(to_n, current_depth + 1)

        traverse(start, 0)
        return results

# 构建"动物世界"知识库
kb = SemanticNetwork()
kb.add_concept("动物")
kb.add_concept("鸟", {"有羽毛": True, "温血": True})
kb.add_concept("鱼", {"有鳞": True, "水生": True})
kb.add_concept("企鹅", {"会飞": False, "南极": True})
kb.add_concept("麻雀", {"会飞": True, "常见": True})

kb.add_relation("鸟", "is-a", "动物")
kb.add_relation("鱼", "is-a", "动物")
kb.add_relation("企鹅", "is-a", "鸟")
kb.add_relation("麻雀", "is-a", "鸟")

# 继承推理: 企鹅是动物吗?
print(kb.query("企鹅", "is-a"))  # [(企鹅,is-a,鸟), (鸟,is-a,动物)]
```

### 1.3 框架表示

框架是**结构化**的知识表示，包含槽(slot)和默认值。

```python
class Frame:
    """框架系统实现"""

    def __init__(self, name, parent=None):
        self.name = name
        self.parent = parent
        self.slots = {}  # 槽: {值, 类型, 约束}
        self.axioms = [] # 附加规则

    def add_slot(self, slot_name, value=None,
                 slot_type='any', constraints=None):
        self.slots[slot_name] = {
            'value': value,
            'type': slot_type,
            'constraints': constraints or [],
            'if_needed': None,    # 按需计算
            'if_added': None      # 添加时触发
        }

    def get_slot(self, slot_name):
        """带继承的槽值获取"""
        if slot_name in self.slots:
            slot = self.slots[slot_name]
            if slot['value'] is not None:
                return slot['value']
            if slot['if_needed']:
                return slot['if_needed'](self)
        # 继承自父框架
        if self.parent:
            return self.parent.get_slot(slot_name)
        return None

# 定义"房间"框架体系
room_frame = Frame("房间")
room_frame.add_slot("墙数", 4, 'integer')
room_frame.add_slot("天花板", True, 'boolean')
room_frame.add_slot("地板", True, 'boolean')
room_frame.add_slot("功能", None, 'string')

kitchen_frame = Frame("厨房", parent=room_frame)
kitchen_frame.add_slot("功能", "烹饪")
kitchen_frame.add_slot("设备", ["炉灶", "冰箱", "水槽"])
kitchen_frame.add_slot("通风", "必需")

bedroom_frame = Frame("卧室", parent=room_frame)
bedroom_frame.add_slot("功能", "睡眠")
bedroom_frame.add_slot("家具", ["床", "衣柜"])

# 实例化
my_kitchen = Frame("我的厨房", parent=kitchen_frame)
print(my_kitchen.get_slot("墙数"))  # 继承: 4
print(my_kitchen.get_slot("功能"))  # 继承: 烹饪
```

### 1.4 脚本表示

脚本描述**典型事件序列**，用于理解叙事和预期行为。

```python
class Script:
    """事件脚本表示"""

    def __init__(self, name):
        self.name = name
        self.roles = {}      # 参与角色
        self.props = {}      # 道具
        self.scenes = []     # 场景序列
        self.preconditions = []
        self.results = []

    def add_role(self, role_name, constraints=None):
        self.roles[role_name] = constraints or {}

    def add_scene(self, action, actor, object=None,
                  preconditions=None, duration=None):
        self.scenes.append({
            'action': action,
            'actor': actor,
            'object': object,
            'pre': preconditions or [],
            'duration': duration
        })

# 餐厅脚本
restaurant_script = Script("餐厅就餐")
restaurant_script.add_role("顾客", {"有": "钱"})
restaurant_script.add_role("服务员", {"工作于": "餐厅"})
restaurant_script.add_role("厨师", {"能": "烹饪"})

restaurant_script.add_scene("进入", "顾客")
restaurant_script.add_scene("找座位", "顾客", preconditions=["有空位"])
restaurant_script.add_scene("看菜单", "顾客")
restaurant_script.add_scene("点餐", "顾客", object="服务员")
restaurant_script.add_scene("烹饪", "厨师", preconditions=["有食材"])
restaurant_script.add_scene("上菜", "服务员")
restaurant_script.add_scene("用餐", "顾客")
restaurant_script.add_scene("付款", "顾客", object="服务员")

# 脚本用于填补信息空缺
def fill_gaps(narrative, script):
    """
    给定: "约翰进了餐厅, 吃了牛排"
    推断: 看了菜单、点了餐、付了钱...
    """
    inferred = []
    narrative_actions = {n['action'] for n in narrative}

    for scene in script.scenes:
        if scene['action'] in narrative_actions:
            # 标记已发生的
            pass
        else:
            # 推断隐含步骤
            can_infer = all(p in narrative_actions for p in scene['pre'])
            if can_infer:
                inferred.append(scene)

    return inferred
```

---

## 2. 概念图与本体论

### 2.1 概念图 (Conceptual Graphs)

概念图是语义网络的形式化扩展，基于**存在图**和**语义网络**。

```
┌─────────────────────────────────────────────────────────────┐
│                     概念图基本元素                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   概念节点: [概念类型: 实例]                                 │
│                                                             │
│      [人: 约翰] ──(AGNT)──→ [吃] ←──(OBJ)── [食物: 苹果]   │
│                             ↑                               │
│                          (TME)                              │
│                             │                               │
│                         [时间: 现在]                         │
│                                                             │
│   关系类型:                                                 │
│   - AGNT: 施事者 (Agent)                                    │
│   - OBJ: 受事者 (Object)                                    │
│   - TME: 时间 (Time)                                        │
│   - LOC: 地点 (Location)                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

```python
class ConceptualGraph:
    """概念图实现"""

    def __init__(self):
        self.concepts = []
        self.relations = []
        self.type_hierarchy = {}

    def add_concept(self, concept_type, referent=None):
        """
        概念: [类型: 指称]
        例: [人: 约翰], [人: *], [数字: 42]
        """
        concept_id = len(self.concepts)
        self.concepts.append({
            'id': concept_id,
            'type': concept_type,
            'referent': referent,  # 具体实例或通配符
            'generic': referent is None or referent == '*'
        })
        return concept_id

    def add_relation(self, rel_type, from_concept, to_concept):
        self.relations.append({
            'type': rel_type,
            'from': from_concept,
            'to': to_concept
        })

    def canonical_form(self):
        """转换为线性规范形式 (CGIF)"""
        lines = []
        for c in self.concepts:
            ref = c['referent'] if c['referent'] else '*'
            lines.append(f"[C{c['id']}: {c['type']} {ref}]")
        for r in self.relations:
            lines.append(f"({r['type']} C{r['from']} C{r['to']})")
        return '\n'.join(lines)

# 构建 "猫在垫子上" 概念图
cg = ConceptualGraph()
c_cat = cg.add_concept("猫", "加菲")
c_mat = cg.add_concept("垫子")
c_on = cg.add_concept("在上面")

cg.add_relation("AGNT", c_on, c_cat)   # 猫是 "在上面" 的施事者
cg.add_relation("LOC", c_on, c_mat)    # 垫子是地点

print(cg.canonical_form())
```

### 2.2 本体论工程

本体论是**显式的概念化规范**，定义领域中的概念、关系和约束。

```python
from dataclasses import dataclass, field
from typing import Set, Optional, List
from enum import Enum

class RelationType(Enum):
    SUBCLASS_OF = "is-a"
    PART_OF = "part-of"
    INSTANCE_OF = "instance-of"
    HAS_PROPERTY = "has-property"

@dataclass
class OntologyClass:
    """本体类定义"""
    name: str
    parent: Optional['OntologyClass'] = None
    properties: Set[str] = field(default_factory=set)
    constraints: List[callable] = field(default_factory=list)

    def is_subclass_of(self, other: 'OntologyClass') -> bool:
        if self.name == other.name:
            return True
        if self.parent:
            return self.parent.is_subclass_of(other)
        return False

    def all_properties(self) -> Set[str]:
        props = set(self.properties)
        if self.parent:
            props.update(self.parent.all_properties())
        return props

@dataclass
class OntologyInstance:
    """本体实例"""
    name: str
    ontology_class: OntologyClass
    property_values: dict = field(default_factory=dict)

class Ontology:
    """本体管理系统"""

    def __init__(self):
        self.classes = {}
        self.instances = {}
        self.relations = []

    def define_class(self, name, parent=None, properties=None):
        parent_class = self.classes.get(parent) if parent else None
        cls = OntologyClass(
            name=name,
            parent=parent_class,
            properties=set(properties or [])
        )
        self.classes[name] = cls
        return cls

    def create_instance(self, name, class_name, **props):
        cls = self.classes[class_name]
        instance = OntologyInstance(
            name=name,
            ontology_class=cls,
            property_values=props
        )
        self.instances[name] = instance
        return instance

    def check_consistency(self, instance: OntologyInstance) -> bool:
        """实例一致性检查"""
        required = instance.ontology_class.all_properties()
        actual = set(instance.property_values.keys())
        return required.issubset(actual)

# 构建计算机领域本体
comp_ont = Ontology()

# 定义类层次
comp_ont.define_class("事物")
comp_ont.define_class("硬件", parent="事物",
                      properties=["制造商", "价格"])
comp_ont.define_class("软件", parent="事物",
                      properties=["版本", "许可证"])
comp_ont.define_class("处理器", parent="硬件",
                      properties=["核心数", "频率", "架构"])
comp_ont.define_class("操作系统", parent="软件",
                      properties=["内核版本", "支持的架构"])

# 创建实例
cpu = comp_ont.create_instance(
    "i9-13900K", "处理器",
    制造商="Intel",
    价格=589.99,
    核心数=24,
    频率="5.8GHz",
    架构="x86_64"
)

# 推理示例
def can_run(os: OntologyInstance, cpu: OntologyInstance) -> bool:
    """检查操作系统是否能在处理器上运行"""
    supported = os.property_values.get("支持的架构", [])
    cpu_arch = cpu.property_values.get("架构")
    return cpu_arch in supported

print(f"CPU 属性: {cpu.ontology_class.all_properties()}")
```

---

## 3. 心智模型理论

### 3.1 心智模型是什么

心智模型是人在心中构建的**外部现实的内部表征**，用于解释、预测和推理。

```
┌─────────────────────────────────────────────────────────────┐
│                   心智模型的形成与使用                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   外部现实    感知    心智模型    推理    行为    结果       │
│      ↓        ↓         ↓         ↓       ↓       ↓        │
│                                                             │
│   ┌────┐    ┌───┐    ┌────┐    ┌───┐   ┌───┐   ┌───┐     │
│   │世界│───→│输入│───→│表示│───→│思考│──→│行动│──→│反馈│     │
│   └────┘    └───┘    └────┘    └───┘   └───┘   └───┘     │
│                                              ↑___________│  │
│                                               模型修正      │
│                                                             │
│   关键特征:                                                 │
│   - 不完全性: 仅包含相关细节                                │
│   - 动态性: 随经验更新                                      │
│   - 功能性: 服务于特定目标                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**不同类型的心智模型:**

| 类型 | 描述 | 示例 |
|:-----|:-----|:-----|
| 结构模型 | 物体如何组织 | 计算机内部组件布局 |
| 因果模型 | 事件如何引发 | 程序错误导致崩溃 |
| 功能模型 | 系统如何工作 | 垃圾回收机制 |
| 过程模型 | 步骤如何执行 | 代码执行流程 |

### 3.2 程序员的心智模型

```python
# 心智模型示例: 理解 Python 变量绑定
# 初学者模型 (错误): 变量是盒子
# x = 1 表示把 1 放进 x 这个盒子

# 正确心智模型: 变量是标签/引用
# x = 1 表示 x 这个名字指向对象 1

x = [1, 2, 3]
y = x           # y 也指向同一个列表对象
y.append(4)     # 修改的是共享的对象!
print(x)        # [1, 2, 3, 4] -  surprising!

# 可视化心智模型:
#  变量名     对象
#    x    →  [1,2,3]
#    y    ───┘
#
# 两个变量引用同一个对象!
```

```python
# 心智模型对比: 值类型 vs 引用类型

# Python 中的可变 vs 不可变
# 不可变对象 (数字, 字符串, 元组)
a = 5
b = a
a = a + 1       # 新对象! b 仍然是 5

# 可变对象 (列表, 字典, 集合)
list1 = [1, 2]
list2 = list1
list1.append(3)  # 修改共享对象! list2 也变了

# 正确的代码心智模型:
def demonstrate_models():
    """
    [栈帧]              [堆]
    ───────────────────────────
    a ──────┐
    b ──────┼──→ 整数对象 5
            │
    list1 ──┼──→ 列表对象 [1, 2, 3]
    list2 ──┘

    赋值是引用复制, 不是深拷贝!
    """
    pass
```

---

## 4. 编程中的心智模型

### 4.1 执行模型

```c
// 心智模型: C 内存布局
// 理解栈帧、堆、数据段的区别

int global_var = 10;        // 数据段 (.data)
const int const_var = 20;   // 只读数据段 (.rodata)

void function(int param) {  // param 在栈上
    int local_var = 30;     // 栈分配
    int* heap_ptr = malloc(sizeof(int));  // 堆分配
    *heap_ptr = 40;

    // 心智模型可视化:
    // ┌─────────────────┐ 高地址
    // │   栈 (向下增长)  │
    // │   param: 值     │
    // │   local_var: 30 │
    // ├─────────────────┤
    // │   ...           │
    // ├─────────────────┤
    // │   堆 (向上增长)  │
    // │   *heap_ptr: 40 │
    // ├─────────────────┤
    // │   数据段         │
    // │   global_var: 10│
    // │   const_var: 20 │
    // └─────────────────┘ 低地址

    free(heap_ptr);
}
```

### 4.2 并发心智模型

```rust
// Rust 的所有权模型: 编译时内存安全心智模型

// 单一所有权
fn ownership_demo() {
    let s1 = String::from("hello");  // s1 拥有字符串
    let s2 = s1;                      // 所有权转移到 s2
    // println!("{}", s1);            // 错误! s1 不再有效
    println!("{}", s2);               // OK
} // s2 离开作用域, 内存释放

// 借用模型
fn borrowing_demo() {
    let mut data = vec![1, 2, 3];

    // 多个不可变借用 - 安全
    let ref1 = &data;
    let ref2 = &data;
    println!("{} {}", ref1[0], ref2[0]);

    // 或一个可变借用 - 独占
    let ref_mut = &mut data;
    ref_mut.push(4);

    // 心智模型: 读写锁的编译时实现
    // - 任意数量读者 或
    // - 一个写者 (且不能有读者)
}
```

### 4.3 类型系统心智模型

```typescript
// TypeScript: 结构化类型系统心智模型

// 名义类型 (Java/C#): 类型由声明决定
// class Dog {}
// class Cat {}
// Dog 和 Cat 是不同的类型, 即使结构相同

// 结构类型 (TypeScript): 类型由结构决定
type Animal = {
    name: string;
    makeSound(): void;
};

// 只要结构匹配, 就是兼容的
const dog = {
    name: "Buddy",
    makeSound() { console.log("Woof"); },
    breed: "Golden"  // 额外属性也 OK
};

function greet(animal: Animal) {
    console.log(`Hello ${animal.name}`);
}

greet(dog);  // OK! 结构兼容

// 心智模型: "鸭子类型"的静态检查版本
// "如果它走路像鸭子, 叫起来像鸭子, 那它就是鸭子"
```

---

## 5. 代码认知复杂度

### 5.1 圈复杂度 (Cyclomatic Complexity)

```python
# 圈复杂度 = 决策点数量 + 1
# 决策点: if, while, for, case, and, or

def low_complexity(x):
    """圈复杂度 = 1"""
    return x * 2

def medium_complexity(x, y, z):
    """圈复杂度 = 3 (2个if + 1)"""
    if x > 0:
        if y > 0:
            return x + y
    return z

def high_complexity(a, b, c, d):
    """圈复杂度 = 8 - 难以理解和测试"""
    if a and b:
        if c:
            for i in range(d):
                if i % 2 == 0:
                    process(i)
                elif i % 3 == 0:
                    handle(i)
                else:
                    skip(i)
        elif d > 10:
            fallback()
    return result

# 重构: 降低认知负荷
def refactored_process(a, b, c, d):
    if not (a and b):
        return result

    if c:
        process_with_c(d)
    elif d > 10:
        fallback()

    return result

def process_with_c(d):
    for i in range(d):
        process_number(i)

def process_number(i):
    if i % 2 == 0:
        process(i)
    elif i % 3 == 0:
        handle(i)
    else:
        skip(i)
```

### 5.2 认知复杂度 (Cognitive Complexity)

| 复杂度类型 | 计算方式 | 目的 |
|:-----------|:---------|:-----|
| 圈复杂度 | 控制流图路径数 | 测试覆盖难度 |
| 认知复杂度 | 嵌套增量 + 逻辑增量 | 人类理解难度 |

```python
# 认知复杂度规则:
# - 顺序代码: +0
# - 条件/循环: +1 (每个)
# - 嵌套: 额外 +n (n为嵌套深度)

def example_cognitive():
    sum = 0                      # 0
    for i in range(10):          # +1 (循环)
        if i % 2 == 0:           # +2 (条件 + 嵌套深度1)
            for j in range(5):   # +3 (循环 + 嵌套深度2)
                sum += j         # 0
    return sum                   # 总认知复杂度 = 6
```

### 5.3 代码可读性原则

```python
# 原则1: 单一职责
def calculate_and_print_and_save(data):
    """违反单一职责 - 认知负担高"""
    result = complex_calculation(data)
    print(f"Result: {result}")
    save_to_database(result)
    return result

def calculate(data):      # 计算
    return complex_calculation(data)

def display(result):      # 展示
    print(f"Result: {result}")

def persist(result):      # 持久化
    save_to_database(result)

# 原则2: 显式优于隐式
class Implicit:
    """隐式行为难以理解"""
    def __init__(self):
        self._setup()  # 隐藏的副作用!

    def _setup(self):
        # 做了很多神奇的事情...
        pass

class Explicit:
    """显式行为易于理解"""
    def __init__(self):
        self.ready = False

    def setup(self):
        """用户必须显式调用, 了解正在发生什么"""
        # 设置逻辑
        self.ready = True

# 原则3: 局部性原则
def poor_locality(items):
    """分散的注意力"""
    results = []
    # 20行无关代码...
    for item in items:
        # 读者已经忘记 results 是干什么的了
        results.append(process(item))
    # 又20行无关代码...
    return results

def good_locality(items):
    """相关的代码在一起"""
    # 20行其他逻辑...

    results = []
    for item in items:
        results.append(process(item))
    return results

    # 其他逻辑分离到不同函数
```

---

## 6. 编程语言设计的认知维度

### 6.1 认知维度框架

Green & Petre 提出的认知维度用于**评估表示法**(包括编程语言)的认知特性。

| 维度 | 描述 | 权衡 |
|:-----|:-----|:-----|
| 抽象梯度 | 抽象层次的数量 | 太多难选, 太少受限 |
| 一致性 | 相似概念是否相似表示 | 高一致性降低学习成本 |
| 错误倾向性 | 表示法是否容易出错 | 静态类型降低运行时错误 |
| 隐蔽性 | 组件是否可见 | 可见性增强理解 |
| 硬心理操作 | 需要多少工作记忆 | 声明式 vs 命令式 |
| 渐进式评估 | 能否部分执行/测试 | REPL 支持渐进开发 |
| 粘度 | 改变表示的阻力 | 高粘度阻碍实验 |
|  premature commitment | 何时必须做决定 | 延迟决策增加灵活性 |

```python
# Python vs Java 的认知维度对比

# Python - 低粘度, 渐进式评估
>>> data = [1, 2, 3]
>>> [x * 2 for x in data]  # 立即看到结果
[2, 4, 6]

# Java - 高形式化, 更多 premature commitment
// 需要完整的类结构才能测试
public class Main {
    public static void main(String[] args) {
        List<Integer> data = Arrays.asList(1, 2, 3);
        List<Integer> doubled = data.stream()
            .map(x -> x * 2)
            .collect(Collectors.toList());
        System.out.println(doubled);
    }
}
```

### 6.2 语言特性与心智模型

```rust
// 所有权系统: 显式生命周期心智模型

// 借用检查器强制正确的心智模型
fn lifetime_example() {
    let r;
    {
        let x = 5;
        r = &x;      // 错误! x 的生命周期不够长
    }                // x 在这里被释放
    println!("{}", r); // r 引用无效内存!
}

// 正确版本
fn correct_lifetime() {
    let x = 5;
    let r = &x;      // r 的生命周期 ≤ x 的生命周期
    println!("{}", r); // OK
}

// 心智模型可视化:
// 'a: ──────────────────────────────
// 'b:      ────────────
//          ↑ 借用不能超过所有者生命周期
```

```haskell
-- Haskell: 纯函数心智模型
-- 数学函数: 输入 → 输出, 无副作用

-- 心智模型: 代码是值的转换, 不是指令序列
square :: Num a => a -> a
square x = x * x

-- 不是: "获取 x, 计算 x*x, 存储结果"
-- 而是: "square 是函数, 将 x 映射到 x*x"

-- 单子用于处理副作用
data Maybe a = Nothing | Just a

-- 心智模型: 用类型系统追踪 "可能失败" 的计算
safeDivide :: Int -> Int -> Maybe Int
safeDivide _ 0 = Nothing
safeDivide x y = Just (x `div` y)

-- 顺序组合保持纯函数心智模型
combined :: Maybe Int
combined = do
    a <- Just 10      -- a = 10
    b <- safeDivide a 2  -- b = 5
    c <- safeDivide b 0  -- 失败! 整体为 Nothing
    return c
```

### 6.3 语法设计原则

```python
# 原则1: 相似操作相似语法
# 好的设计:
items.append(x)      # 列表添加
items.extend([x, y]) # 列表扩展
dict.update(other)   # 字典更新 - 一致的动词风格

# 原则2: 可预测的语法规则
# Python 的 "只有一种明显的方式"
squares = [x**2 for x in range(10)]  # 列表推导式

# 原则3: 视觉一致性
# 缩进表示块结构 - 所见即所得
def example():
    if condition:
        do_something()
    else:
        do_other()
# 代码外观反映逻辑结构

# 反例: 过多语法变体增加认知负担
# C++ 初始化: 多种语法表示同一概念
int x = 5;
int y(5);
int z{5};  // C++11
auto w = int{5};
```

---

## 7. 参考资料

### 经典文献

| 书名 | 作者 | 重点 |
|:-----|:-----|:-----|
| *Mental Models* | Gentner & Stevens | 心智模型理论基础 |
| *The Psychology of Programming* | Green et al. | 编程心理学 |
| *Conceptual Structures* | John Sowa | 概念图理论 |
| *Knowledge Representation* | Brachman & Levesque | KR 权威教材 |

### 在线资源

- [Conceptual Graphs Standard](http://www.jfsowa.com/cg/)
- [OWL Web Ontology Language](https://www.w3.org/OWL/)
- [Cognitive Dimensions of Notations](http://www.cl.cam.ac.uk/~afb21/CognitiveDimensions/)

### 工具

| 工具 | 用途 |
|:-----|:-----|
| Protégé | 本体论编辑器 |
| Graphviz | 概念图可视化 |
| SonarQube | 代码复杂度分析 |
| Cognitive Complexity Linter | 认知复杂度检查 |

---

> **核心洞察**: 认知表示的本质是**压缩与重构**——将复杂的现实压缩为可操作的符号结构，并在需要时准确重构。好的代码和好的知识表示一样，必须在表达力、效率和可理解性之间取得平衡。
