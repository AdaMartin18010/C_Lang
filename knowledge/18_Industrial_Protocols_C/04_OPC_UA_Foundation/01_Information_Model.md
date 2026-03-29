# OPC UA 信息模型深度解析

> **核心特性**: 语义互操作性、面向对象、安全通信
> **架构层次**: 传输层、服务层、信息模型层
> **应用领域**: 工业4.0、智能工厂、云边协同
> **最后更新**: 2026-03-29

---

## 1. OPC UA架构形式化定义

### 1.1 OPC UA层次架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       OPC UA层次架构                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        应用层 (Application Layer)                    │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐ │   │
│  │  │   HMI客户端  │  │   MES系统   │  │   ERP集成   │  │   云网关    │ │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘ │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│  ┌─────────────────────────────────▼─────────────────────────────────────┐   │
│  │                        服务层 (Services Layer)                       │   │
│  │                                                                     │   │
│  │  核心服务:                                                           │   │
│  │  • 发现服务 (Discovery)     • 会话服务 (Session)                    │   │
│  │  • 节点管理服务 (NodeManagement) • 视图服务 (View)                  │   │
│  │  • 属性服务 (Attribute)     • 方法服务 (Method)                     │   │
│  │  • 监控项服务 (MonitoredItem) • 订阅服务 (Subscription)             │   │
│  │  • 查询服务 (Query)         • 历史访问服务 (History)                │   │
│  │                                                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│  ┌─────────────────────────────────▼─────────────────────────────────────┐   │
│  │                      信息模型层 (Information Model)                  │   │
│  │                                                                     │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐              │   │
│  │  │  核心模型     │  │  配套模型     │  │  行业模型    │              │   │
│  │  │  (Core)      │  │  (Companion)  │  │  (Vertical)  │              │   │
│  │  │              │  │              │  │              │              │   │
│  │  │ • ObjectTypes│  │ • DI设备集成 │  │ • PLCopen    │              │   │
│  │  │ • Variable   │  │ • Robotics   │  │ • PackML     │              │   │
│  │  │ • Method     │  │ • CNC        │  │ • MTConnect  │              │   │
│  │  │ • DataTypes  │  │ • Analyzer   │  │ • AutoID     │              │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘              │   │
│  │                                                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│  ┌─────────────────────────────────▼─────────────────────────────────────┐   │
│  │                      传输层 (Transport Layer)                        │   │
│  │                                                                     │   │
│  │  ┌──────────────────┐    ┌──────────────────┐                      │   │
│  │  │   OPC TCP        │    │   HTTPS          │                      │   │
│  │  │   (Binary)       │    │   (JSON/XML)     │                      │   │
│  │  │                  │    │                  │                      │   │
│  │  │  高效二进制编码   │    │  防火墙友好      │                      │   │
│  │  │  高性能工业应用   │    │  Web集成         │                      │   │
│  │  └──────────────────┘    └──────────────────┘                      │   │
│  │                                                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    │                                        │
│  ┌─────────────────────────────────▼─────────────────────────────────────┐   │
│  │                      安全层 (Security Layer)                         │   │
│  │                                                                     │   │
│  │  • 应用认证 (X.509证书)    • 用户认证 (用户名/证书/令牌)             │   │
│  │  • 通道加密 (AES-256)      • 消息签名 (SHA-256)                      │   │
│  │  • 安全策略协商            • 会话管理                                │   │
│  │                                                                     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 信息模型核心概念形式化定义

**定义 1.1 (OPC UA地址空间)**

```
地址空间 = (Nodes, References, Namespaces)

Nodes: 节点集合，每个节点表示地址空间中的实体
  Node = (NodeId, NodeClass, Attributes, References)

  NodeClass ∈ {
    Object,         /* 对象节点 */
    Variable,       /* 变量节点 */
    Method,         /* 方法节点 */
    ObjectType,     /* 对象类型 */
    VariableType,   /* 变量类型 */
    DataType,       /* 数据类型 */
    ReferenceType   /* 引用类型 */
  }

  NodeId: 全局唯一标识符
    NodeId = (NamespaceIndex, IdentifierType, Identifier)
    IdentifierType ∈ {NUMERIC, STRING, GUID, BYTESTRING}

References: 引用关系，连接节点形成图结构
  Reference = (SourceNode, ReferenceType, TargetNode, IsForward)

Namespaces: 命名空间，隔离不同模型的标识符
  NamespaceURI: 唯一URI标识命名空间
  NamespaceIndex: 本地索引 (0=OPC UA标准, 1=服务器特定, ≥2=配套规范)
```

### 1.3 节点类别属性矩阵

| 节点类别 | 核心属性 | 用途 | 可实例化 |
|----------|----------|------|----------|
| **Object** | EventNotifier | 表示物理/抽象实体 | 是 |
| **Variable** | Value, DataType, AccessLevel | 表示数据值 | 是 |
| **Method** | Executable, UserExecutable | 可调用操作 | 是 |
| **ObjectType** | IsAbstract | 对象类型定义 | 否 (供继承) |
| **VariableType** | Value, DataType, ArrayDimensions | 变量类型定义 | 否 (供继承) |
| **DataType** | IsAbstract, EnumStrings | 数据类型定义 | 否 |
| **ReferenceType** | IsAbstract, Symmetric, InverseName | 引用语义定义 | 否 |

---

## 2. 节点与引用关系深度分析

### 2.1 引用类型层次结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       OPC UA引用类型层次                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  References (抽象)                                                          │
│  │                                                                          │
│  ├── HierarchicalReferences (层次引用)                                      │
│  │   ├── HasChild                                                          │
│  │   │   ├── HasComponent          /* 组件关系 */                           │
│  │   │   ├── HasOrderedComponent   /* 有序组件 */                           │
│  │   │   ├── HasProperty           /* 属性关系 */                           │
│  │   │   └── HasSubType            /* 子类型关系 */                         │
│  │   │                                                                      │
│  │   └── NonHierarchicalReferences                                          │
│  │       └── (空，无具体子类型)                                             │
│  │                                                                          │
│  └── NonHierarchicalReferences (非层次引用)                                 │
│      ├── HasTypeDefinition        /* 类型定义引用 */                         │
│      ├── HasModellingRule         /* 建模规则引用 */                         │
│      ├── HasEncoding              /* 编码引用 */                             │
│      ├── HasDescription           /* 描述引用 */                             │
│      ├── GeneratesEvent           /* 事件生成 */                             │
│      ├── AlwaysGeneratesEvent     /* 总是生成事件 */                         │
│      └── Aggregates               /* 聚合引用 (HasChild父类型) */            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 类型定义与实例化决策树

```
OPC UA类型系统
│
├── 类型定义 (Type Definition)
│   ├── ObjectType
│   │   └── 定义对象的蓝图
│   │       ├── 必需的组件 (HasComponent)
│   │       ├── 必需的属性 (HasProperty)
│   │       ├── 可选的组件/属性 (ModellingRule=Optional)
│   │       └── 建模规则 (Mandatory/Optional/MandatoryPlaceholder...)
│   │
│   ├── VariableType
│   │   └── 定义变量的蓝图
│   │       ├── 数据类型约束
│   │       ├── 值范围约束
│   │       └── 数组维度
│   │
│   └── DataType
│       ├── 内置类型 (Built-in: Int32, String, DateTime...)
│       ├── 枚举类型 (Enumeration)
│       └── 结构体类型 (Structure)
│
└── 实例化 (Instantiation)
    ├── 基于ObjectType创建Object
    │   └── 自动继承类型定义的组件和属性
    │
    ├── 基于VariableType创建Variable
    │   └── 继承数据类型和约束
    │
    └── 类型定义引用 (HasTypeDefinition)
        └── 实例节点指向其类型定义
```

### 2.3 类型继承形式化定义

**定义 2.1 (类型继承)**

```
类型继承关系: SubType ⊂ SuperType

继承规则:
  1. 子类型继承父类型的所有属性和引用
  2. 子类型可以添加新的属性和引用
  3. 子类型可以重写父类型的引用 (ModellingRule)
  4. 子类型不能删除父类型的Mandatory元素

HasSubType引用:
  Source: 父类型节点
  Target: 子类型节点
  IsForward: false (子类型指向父类型)

多继承: OPC UA不支持多继承
  - 一个类型只能有一个直接父类型
  - 通过接口或组合实现类似功能
```

---

## 3. 变量与数据类型

### 3.1 变量节点结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        变量节点详细结构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Variable Node                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │  NodeId: ns=2;s="Temperature_Sensor_01"                             │   │
│  │  BrowseName: 2:Temperature                                          │   │
│  │  DisplayName: {"en": "Temperature"}                                 │   │
│  │  Description: {"en": "当前温度值"}                                   │   │
│  ├─────────────────────────────────────────────────────────────────────┤   │
│  │  Value: 23.5                                                        │   │
│  │  DataType: ns=0;i=10 (Double)                                       │   │
│  │  ValueRank: -1 (Scalar)                                             │   │
│  │  ArrayDimensions: []                                                │   │
│  ├─────────────────────────────────────────────────────────────────────┤   │
│  │  AccessLevel: 0x03 (CurrentRead | CurrentWrite)                     │   │
│  │  UserAccessLevel: 0x01 (CurrentRead)                                │   │
│  │  MinimumSamplingInterval: 100 (ms)                                  │   │
│  │  Historizing: false                                                 │   │
│  ├─────────────────────────────────────────────────────────────────────┤   │
│  │  References:                                                        │   │
│  │   • HasTypeDefinition → BaseDataVariableType                        │   │
│  │   • HasComponent → EngineeringUnits                                 │   │
│  │   • HasComponent → InstrumentRange                                  │   │
│  │   • OrganizedBy → SensorsFolder                                    │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.2 访问级别矩阵

| 位 | 名称 | 含义 |
|-----|------|------|
| 0 | CurrentRead | 可读当前值 |
| 1 | CurrentWrite | 可写当前值 |
| 2 | HistoryRead | 可读历史值 |
| 3 | HistoryWrite | 可写历史值 |
| 4 | SemanticChange | 语义变化时通知 |
| 5 | StatusWrite | 可写状态码 |
| 6 | TimestampWrite | 可写时间戳 |

### 3.3 数据类型层次

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      OPC UA数据类型层次                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  BaseDataType (抽象)                                                        │
│  │                                                                          │
│  ├── Boolean                                                                │
│  ├── SByte, Byte                                                            │
│  ├── Int16, UInt16                                                          │
│  ├── Int32, UInt32                                                          │
│  ├── Int64, UInt64                                                          │
│  ├── Float, Double                                                          │
│  ├── String                                                                 │
│  ├── DateTime                                                               │
│  ├── Guid                                                                   │
│  ├── ByteString                                                             │
│  ├── XmlElement                                                             │
│  ├── NodeId                                                                 │
│  ├── ExpandedNodeId                                                         │
│  ├── StatusCode                                                             │
│  ├── QualifiedName                                                          │
│  ├── LocalizedText                                                          │
│  ├── Structure (抽象)                                                       │
│  │   ├── Argument       /* 方法参数 */                                      │
│  │   ├── EnumValueType  /* 枚举值 */                                        │
│  │   └── ... (更多内置结构)                                                  │
│  │                                                                          │
│  ├── Enumeration (抽象)                                                     │
│  │   ├── NodeClass       /* 节点类别枚举 */                                  │
│  │   └── ... (更多内置枚举)                                                  │
│  │                                                                          │
│  ├── Number (抽象)                                                          │
│  │   ├── Integer (抽象)                                                     │
│  │   └── Float (抽象)                                                       │
│  │                                                                          │
│  └── ... (用户自定义类型)                                                    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 4. 建模规则与实例声明

### 4.1 建模规则矩阵

| 规则 | 含义 | 实例化行为 |
|------|------|------------|
| **Mandatory** | 必须存在 | 每个实例必须包含 |
| **Optional** | 可选存在 | 实例可选择包含 |
| **MandatoryPlaceholder** | 必须被替换 | 子类型必须提供实现 |
| **OptionalPlaceholder** | 可选被替换 | 子类型可选择提供实现 |
| **ExposesItsArray** | 数组元素独立 | 每个数组元素建模为独立节点 |

### 4.2 实例声明决策树

```
实例声明 (InstanceDeclaration)
│
├── 定义位置
│   ├── 类型定义内部 (ObjectType/VariableType)
│   │   └── 作为实例建模的模板
│   │
│   └── 实例 (Instance)
│       └── 由客户端或服务器在地址空间中创建
│
├── 建模规则应用
│   ├── Mandatory
│   │   └── 实例化时必须创建对应节点
│   │
│   ├── Optional
│   │   └── 实例化时可选择创建
│   │
│   └── Placeholder
│       └── 在子类型中提供具体定义
│
└── 命名规则
    ├── BrowseName继承
    │   └── 实例继承类型定义的BrowseName
    │
    └── DisplayName可定制
        └── 实例可以有本地化显示名称
```

---

## 5. 地址空间组织模式

### 5.1 标准地址空间结构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      标准OPC UA地址空间结构                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Objects (对象文件夹)                                                       │
│  │                                                                          │
│  ├── Server          /* 服务器信息对象 */                                   │
│  │   ├── ServerCapabilities                                                 │
│  │   ├── ServerDiagnostics                                                  │
│  │   ├── VendorServerInfo                                                   │
│  │   └── ServerRedundancy                                                   │
│  │                                                                          │
│  └── (应用特定对象)                                                          │
│      ├── ProductionLine_01                                                  │
│      │   ├── Robot_Arm_01                                                   │
│      │   ├── Conveyor_Belt_01                                               │
│      │   └── TemperatureSensor_01                                           │
│      └── ...                                                                │
│                                                                             │
│  Types (类型定义文件夹)                                                     │
│  │                                                                          │
│  ├── ObjectTypes         /* 对象类型定义 */                                 │
│  │   ├── BaseObjectType                                                     │
│  │   ├── FolderType                                                         │
│  │   └── (应用特定类型)                                                      │
│  │                                                                          │
│  ├── VariableTypes       /* 变量类型定义 */                                 │
│  │   ├── BaseVariableType                                                   │
│  │   ├── BaseDataVariableType                                               │
│  │   └── (应用特定类型)                                                      │
│  │                                                                          │
│  ├── DataTypes           /* 数据类型定义 */                                 │
│  │   ├── BaseDataType                                                       │
│  │   └── (应用特定类型)                                                      │
│  │                                                                          │
│  ├── ReferenceTypes      /* 引用类型定义 */                                 │
│  │   └── References                                                         │
│  │                                                                          │
│  └── EventTypes          /* 事件类型定义 */                                 │
│      └── BaseEventType                                                      │
│                                                                             │
│  Views (视图文件夹)                                                         │
│  └── (应用特定视图)                                                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 视图机制形式化定义

**定义 5.1 (视图)**

```
视图 = (ViewNode, VisibleNodes)

ViewNode: 视图定义节点
  - NodeClass: View
  - ContainsNoLoops: 是否允许循环引用
  - EventNotifier: 是否支持事件

VisibleNodes: 视图中可见的节点子集
  - 每个可见节点是一个BrowsePath
  - 视图可以隐藏地址空间的部分内容
  - 不同用户/应用可以看到不同视图

视图用途:
  1. 简化复杂地址空间的导航
  2. 基于角色的访问控制
  3. 多维度数据组织
  4. 子集导出/导入
```

---

## 6. 参考资源

- **OPC UA Specification Part 3**: Address Space Model
- **OPC UA Specification Part 4**: Services
- **OPC UA Specification Part 5**: Information Model
- **OPC 10000-100**: Device Integration (DI) Model
- **IEC 62541**: OPC Unified Architecture标准
