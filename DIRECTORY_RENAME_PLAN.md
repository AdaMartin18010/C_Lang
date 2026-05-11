# C_Lang 目录重命名方案

> **目标**: 消除编号重复，建立可维护的目录结构
> **原则**: 最小化链接破坏，优先处理低风险目录
> **状态**: 执行中（第一阶段结构治理）

---

## 当前状态（幽灵目录清理后）

剩余重复编号目录：

| 编号 | 目录A（保留） | 目录B（重命名） | 策略 |
|:----:|:-------------|:---------------|:-----|
| 01 | Core_Knowledge_System | System_Programming | 合并到 Core |
| 04 | Industrial_Scenarios | Standard_Library_Frameworks | 改为 `standard_library_frameworks/` |
| 05 | Deep_Structure_MetaPhysics | C_Language_Standards | 改为 `c_language_standards/` |
| 05 | Deep_Structure_MetaPhysics | Engineering_Layer | 已合并 ✅ |
| 06 | Thinking_Representation | Embedded_Bare_Metal | 改为 `embedded_bare_metal/` |
| 07 | Modern_Toolchain | Development_Methodology | 合并到 Toolchain |
| 08 | Zig_C_Connection | Toolchain_Ecosystem | 合并到 Toolchain |
| 09 | Data_Structures_Algorithms | Cross_Platform_Development | 改为 `cross_platform_development/` |
| 10 | WebAssembly_C | Modern_C_Development | 合并到 Core/07_Modern_C |
| 11 | Machine_Learning_C | Performance_Engineering | 改为 `performance_engineering/` |
| 15 | Network_Programming | Memory_Safe_Languages | 改为 `memory_safe_languages/` |
| 16 | Embedded_AI_Agents | OS_Kernel | 改为 `os_kernel/` |
| 18 | Industrial_Protocols_C | Robotics_Physical_Control | 改为 `robotics_physical_control/` |
| 20 | Industrial_Case_Studies | Hardware_Interfaces_C | 改为 `hardware_interfaces_c/` |
| 21 | Computation_Theory | OS_Internals | 改为 `os_internals/` |

---

## 重命名执行映射

### 批次1：低风险（内容少、链接少）

| 原路径 | 新路径 | 风险 |
|:-------|:-------|:----:|
| `01_System_Programming/` | 合并到 `01_Core_Knowledge_System/01_System_Programming/` | 低 |
| `07_Development_Methodology/` | 合并到 `07_Modern_Toolchain/07_Development_Methodology/` | 低 |
| `08_Toolchain_Ecosystem/` | 合并到 `07_Modern_Toolchain/08_Toolchain_Ecosystem/` | 低 |
| `10_Modern_C_Development/` | 合并到 `01_Core_Knowledge_System/07_Modern_C/` | 低 |
| `21_OS_Internals/` | 合并到 `16_OS_Kernel/` 或 `04_Industrial_Scenarios/13_Linux_Kernel/` | 低 |

### 批次2：中风险（需要更新链接）

| 原路径 | 新路径 | 风险 |
|:-------|:-------|:----:|
| `04_Standard_Library_Frameworks/` | `standard_library_frameworks/` | 中 |
| `05_C_Language_Standards/` | `c_language_standards/` | 中 |
| `06_Embedded_Bare_Metal/` | `embedded_bare_metal/` | 中 |
| `09_Cross_Platform_Development/` | `cross_platform_development/` | 中 |
| `11_Performance_Engineering/` | `performance_engineering/` | 中 |
| `15_Memory_Safe_Languages/` | `memory_safe_languages/` | 中 |
| `16_OS_Kernel/` | `os_kernel/` | 中 |
| `18_Robotics_Physical_Control/` | `robotics_physical_control/` | 中 |
| `20_Hardware_Interfaces_C/` | `hardware_interfaces_c/` | 中 |

---

## 执行建议

鉴于重命名操作会破坏大量内部链接，建议采用**渐进式策略**：

1. **立即执行**：批次1（合并内容少的目录）
2. **延后执行**：批次2（在建立自动化链接修复工具后执行）
3. **长期目标**：全部迁移到语义化命名（去除所有数字前缀）

---

*最后更新: 2026-05-11*
