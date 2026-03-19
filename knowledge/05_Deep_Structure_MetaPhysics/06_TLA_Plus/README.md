# TLA+ 形式化验证

本目录介绍TLA+（Temporal Logic of Actions）形式化规约语言，用于并发和分布式系统的严格建模与验证。

## 内容概览

| 文件 | 描述 |
|------|------|
| `01_TLA_Introduction.md` | TLA+基础概念与语言介绍 |
| `02_PlusCal_Language.md` | PlusCal算法语言详解 |
| `03_Model_Checking.md` | 模型检查技术与TLC使用 |
| `04_Distributed_Systems_Specs.md` | 分布式系统规约案例 |
| `05_C_Verification_with_TLA.md` | 使用TLA+验证C程序 |

## 什么是TLA+

TLA+是由Leslie Lamport开发的形式化规约语言，用于：

- 精确描述系统行为
- 验证系统设计正确性
- 发现并发系统中的边界情况
- 在编码前发现设计缺陷

## 核心概念

- **状态机**: 系统作为状态转换的数学描述
- **时序逻辑**: 描述系统随时间演化的性质
- **模型检查**: 自动验证规约满足性质
- **PlusCal**: 类Pascal的算法描述语言

## 典型应用

- 分布式算法验证
- 并发协议设计
- 数据库事务模型
- 硬件协议验证

## 学习资源

- 《Specifying Systems》官方教材
- Lamport的TLA+视频课程
- TLA+ Toolbox工具使用

---

[← 返回上级目录](../README.md)
