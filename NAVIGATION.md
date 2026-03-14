# C_Lang 知识库导航指南

> **快速找到您需要的内容**  
> 本指南提供多种浏览路径，帮助您高效使用知识库。

---

## 🚀 快速开始

### 新用户推荐路径

```
1. 📖 阅读 [全局索引](knowledge/00_INDEX.md)
   └─ 了解知识库全貌
   
2. 🎯 选择学习路径
   ├─ [初学者](knowledge/06_Thinking_Representation/06_Learning_Paths/01_Beginner_to_Advanced.md)
   ├─ [进阶者](knowledge/01_Core_Knowledge_System/02_Core_Layer/)
   └─ [专家](knowledge/02_Formal_Semantics_and_Physics/)
   
3. 📚 开始阅读
   └─ 从推荐的第一篇文章开始
```

### 热门入口

| 需求 | 直接跳转 |
|:-----|:---------|
| 学习C23新特性 | [C23核心特性](knowledge/01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md) |
| 了解Zig语言 | [Zig-C连接](knowledge/08_Zig_C_Connection/README.md) |
| 形式化验证入门 | [核心语义基础](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/) |
| 安全编码规范 | [MISRA C:2023](knowledge/01_Core_Knowledge_System/09_Safety_Standards/README.md) |
| LLM部署实战 | [llama.cpp指南](knowledge/11_Machine_Learning_C/07_llama_cpp_Deployment.md) |

---

## 📚 按主题浏览

### 语言基础

```
01_Core_Knowledge_System/
├── 📘 基础层
│   ├── [语法元素](knowledge/01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md)
│   ├── [数据类型](knowledge/01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md)
│   ├── [运算符](knowledge/01_Core_Knowledge_System/01_Basic_Layer/03_Operators_Expressions.md)
│   └── [控制流](knowledge/01_Core_Knowledge_System/01_Basic_Layer/04_Control_Flow.md)
│
├── 📗 核心层
│   ├── [指针深度](knowledge/01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md)
│   ├── [内存管理](knowledge/01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)
│   ├── [字符串处理](knowledge/01_Core_Knowledge_System/02_Core_Layer/03_String_Processing.md)
│   └── [函数与作用域](knowledge/01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md)
│
├── 📙 构造层
│   ├── [结构体与联合体](knowledge/01_Core_Knowledge_System/03_Construction_Layer/01_Structures_Unions.md)
│   ├── [预处理器](knowledge/01_Core_Knowledge_System/03_Construction_Layer/02_Preprocessor.md)
│   └── [模块化](knowledge/01_Core_Knowledge_System/03_Construction_Layer/03_Modularization_Linking.md)
│
└── 📕 现代C (C11-C23)
    ├── [C11特性](knowledge/01_Core_Knowledge_System/07_Modern_C/01_C11_Features.md)
    ├── [C17/C23特性](knowledge/01_Core_Knowledge_System/07_Modern_C/02_C17_C23_Features.md)
    └── [C23核心特性](knowledge/01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md)
```

### 形式化方法与验证

```
02_Formal_Semantics_and_Physics/
├── 🔬 核心语义基础
│   ├── [操作语义](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/01_Operational_Semantics.md)
│   ├── [指称语义](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/02_Denotational_Semantics.md)
│   ├── [公理语义](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/03_Axiomatic_Semantics_Hoare.md)
│   └── [C类型理论](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/04_C_Type_Theory.md)
│
├── 🎮 博弈语义
│   └── [C11内存模型](knowledge/02_Formal_Semantics_and_Physics/01_Game_Semantics/02_C11_Memory_Model.md)
│
└── ✅ CompCert验证
    ├── [概述](knowledge/02_Formal_Semantics_and_Physics/11_CompCert_Verification/01_Compcert_Overview.md)
    ├── [VST分离逻辑](knowledge/02_Formal_Semantics_and_Physics/11_CompCert_Verification/02_VST_Separation_Logic_Practical.md)
    └── [Coq案例](knowledge/02_Formal_Semantics_and_Physics/11_CompCert_Verification/06_Coq_Examples/01_Swap_Proof.v)
```

### 系统编程

```
03_System_Technology_Domains/
├── ⚡ 并发并行
│   └── [POSIX线程](knowledge/03_System_Technology_Domains/14_Concurrency_Parallelism/01_POSIX_Threads.md)
│
├── 🌐 网络编程
│   └── [Socket编程](knowledge/03_System_Technology_Domains/15_Network_Programming/01_Socket_Programming.md)
│
├── 🔒 安全启动
│   └── [ARM Trusted Firmware](knowledge/03_System_Technology_Domains/06_Security_Boot/01_ARM_Trusted_Firmware.md)
│
└── 💾 内存数据库
    └── [B树索引](knowledge/03_System_Technology_Domains/11_In_Memory_Database/01_B_Tree_Index.md)
```

### 工业场景

```
04_Industrial_Scenarios/
├── 🚗 汽车电子
│   ├── [ABS系统](knowledge/04_Industrial_Scenarios/01_Automotive_ABS/01_ABS_System.md)
│   └── [CAN总线](knowledge/04_Industrial_Scenarios/01_Automotive_ECU/01_CAN_Bus_Protocol.md)
│
├── ✈️ 航空电子
│   └── [ARINC 429](knowledge/04_Industrial_Scenarios/02_Avionics_Systems/01_ARINC_429.md)
│
├── 📡 5G基带
│   └── [SIMD向量化](knowledge/04_Industrial_Scenarios/04_5G_Baseband/01_SIMD_Vectorization.md)
│
├── 🎮 游戏引擎
│   └── [ECS架构](knowledge/04_Industrial_Scenarios/05_Game_Engine/01_ECS_Architecture.md)
│
└── ⚛️ 量子计算
    └── [量子-经典接口](knowledge/04_Industrial_Scenarios/06_Quantum_Computing/01_Quantum_Classical_Interface.md)
```

### 现代工具链

```
07_Modern_Toolchain/
├── 🛠️ 构建系统
│   ├── [CMake最佳实践](knowledge/07_Modern_Toolchain/02_Build_Systems_Modern/01_CMake_Modern_Best_Practices.md)
│   ├── [Meson构建](knowledge/07_Modern_Toolchain/02_Build_Systems_Modern/02_Meson_Build.md)
│   └── [Bazel](knowledge/07_Modern_Toolchain/02_Build_Systems_Modern/04_Bazel_C_Projects.md)
│
├── 📦 包管理
│   ├── [Conan](knowledge/07_Modern_Toolchain/04_Package_Management/01_Conan_Package_Manager.md)
│   └── [vcpkg](knowledge/07_Modern_Toolchain/04_Package_Management/02_vcpkg_Guide.md)
│
├── 🔍 代码质量
│   ├── [静态分析](knowledge/07_Modern_Toolchain/05_Code_Quality_Toolchain/02_Static_Analysis_Deep_Dive.md)
│   └── [测试框架](knowledge/07_Modern_Toolchain/05_Code_Quality_Toolchain/03_Testing_Frameworks.md)
│
└── 🔧 IDE与编辑器
    ├── [VS Code配置](knowledge/07_Modern_Toolchain/01_IDE_Editors/01_VS_Code_Setup.md)
    ├── [Neovim](knowledge/07_Modern_Toolchain/01_IDE_Editors/02_Neovim_Modern.md)
    └── [CLion](knowledge/07_Modern_Toolchain/01_IDE_Editors/03_CLion_Guide.md)
```

### Zig语言连接

```
08_Zig_C_Connection/
├── 🔄 类型系统映射
│   ├── [C到Zig类型对应](knowledge/08_Zig_C_Connection/01_Type_System_Mapping/01_C_to_Zig_Type_Correspondence.md)
│   └── [结构体等价性](knowledge/08_Zig_C_Connection/01_Type_System_Mapping/02_Extern_Struct_Equivalence.md)
│
├── 🧠 内存模型桥接
│   └── [C11 vs Zig](knowledge/08_Zig_C_Connection/02_Memory_Model_Bridge/01_C11_vs_Zig_Memory_Model.md)
│
├── 📐 ABI形式化
│   └── [System V ABI](knowledge/08_Zig_C_Connection/03_ABI_Formalization/01_System_V_ABI_Zig_C.md)
│
├── 📝 翻译验证
│   └── [translate-c语义](knowledge/08_Zig_C_Connection/04_Translation_Validation/01_Translate_C_Semantics.md)
│
└── 🚀 迁移方法论
    └── [渐进式迁移](knowledge/08_Zig_C_Connection/05_Migration_Methodology/01_Incremental_Migration_Patterns.md)
```

---

## 🎯 按学习目标浏览

### 我想学习...

#### C语言基础
→ [01_Core_Knowledge_System/01_Basic_Layer/](knowledge/01_Core_Knowledge_System/01_Basic_Layer/)

#### 指针和内存
→ [01_Core_Knowledge_System/02_Core_Layer/](knowledge/01_Core_Knowledge_System/02_Core_Layer/)

#### C23新特性
→ [01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md](knowledge/01_Core_Knowledge_System/07_Modern_C/03_C23_Core_Features.md)

#### 形式化验证
→ [02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/](knowledge/02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/)

#### 编译器原理
→ [02_Formal_Semantics_and_Physics/11_CompCert_Verification/](knowledge/02_Formal_Semantics_and_Physics/11_CompCert_Verification/)

#### Zig语言
→ [08_Zig_C_Connection/](knowledge/08_Zig_C_Connection/)

#### 嵌入式开发
→ [04_Industrial_Scenarios/01_Automotive_ECU/](knowledge/04_Industrial_Scenarios/01_Automotive_ECU/)

#### 安全编码
→ [01_Core_Knowledge_System/09_Safety_Standards/](knowledge/01_Core_Knowledge_System/09_Safety_Standards/)

#### 现代构建工具
→ [07_Modern_Toolchain/02_Build_Systems_Modern/](knowledge/07_Modern_Toolchain/02_Build_Systems_Modern/)

---

## 🔍 按问题类型浏览

### 编译问题
→ [编译构建](knowledge/01_Core_Knowledge_System/05_Engineering_Layer/01_Compilation_Build.md)

### 内存问题
→ [内存管理](knowledge/01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)  
→ [调试技巧](knowledge/01_Core_Knowledge_System/05_Engineering_Layer/02_Debug_Techniques.md)

### 性能优化
→ [性能优化](knowledge/01_Core_Knowledge_System/05_Engineering_Layer/03_Performance_Optimization.md)

### 并发问题
→ [并发并行](knowledge/03_System_Technology_Domains/14_Concurrency_Parallelism/)

---

## 📖 参考资源

### 标准文档
- [C23到C2y路线图](knowledge/00_VERSION_TRACKING/C23_to_C2y_Roadmap.md)
- [Zig路线图追踪](knowledge/00_VERSION_TRACKING/Zig_Roadmap_Tracking.md)

### 项目信息
- [项目状态](PROJECT_STATUS.md) - 当前状态和统计
- [版本历史](CHANGELOG.md) - 更新记录
- [贡献指南](CONTRIBUTING.md) - 如何参与

### 工具
- [维护工具](scripts/maintenance_tool.py) - 自动化检查

---

## 💡 使用技巧

### 搜索技巧

1. **使用GitHub搜索**: 在仓库页面按 `t` 键，然后输入文件名
2. **使用编辑器**: 克隆仓库后用VS Code等编辑器全局搜索
3. **查看索引**: [全局索引](knowledge/00_INDEX.md) 包含完整目录

### 阅读建议

1. **循序渐进**: 按照学习路径从基础到高级
2. **动手实践**: 每个知识点配合代码示例
3. **交叉参考**: 利用文档间的链接建立知识网络
4. **定期复习**: 使用思维导图和概念映射工具

---

> **提示**: 如果找不到需要的内容，欢迎 [提交Issue](../../issues/new) 建议添加！

---

*最后更新: 2026-03-15*
