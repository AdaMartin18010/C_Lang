# DO-178C - 航空机载系统软件标准

> 航空软件的开发与认证标准

---

## 概述

DO-178C（Software Considerations in Airborne Systems and Equipment Certification）是民用航空软件开发和认证的核心标准，由RTCA组织发布。

| 属性 | 说明 |
|:-----|:-----|
| **前身** | DO-178B (1992) |
| **当前版本** | DO-178C (2011) |
| **补充标准** | DO-330（工具鉴定）、DO-331（模型开发）等 |
| **适用范围** | 民用航空机载软件 |

---

## 软件等级 (DAL)

| 等级 | 失效条件 | 示例系统 |
|:-----|:---------|:---------|
| **Level A** | 灾难性 (Catastrophic) | 飞行控制、引擎控制 |
| **Level B** | 危险/严重 (Hazardous) | 自动驾驶、导航 |
| **Level C** | 重大 (Major) | 通信系统、燃油管理 |
| **Level D** | 较小 (Minor) | 客舱娱乐、维护系统 |
| **Level E** | 无安全影响 (No Effect) | 乘客信息服务 |

---

## 核心要求

### 验证要求

| 目标 | Level A | Level B | Level C | Level D | Level E |
|:-----|:-------:|:-------:|:-------:|:-------:|:-------:|
| 需求验证 | ✓ | ✓ | ✓ | ✓ | - |
| 设计验证 | ✓ | ✓ | ✓ | - | - |
| 代码验证 | ✓ | ✓ | - | - | - |
| MC/DC覆盖 | ✓ | - | - | - | - |

### MC/DC覆盖（Level A）

Modified Condition/Decision Coverage，修改条件/判定覆盖：

```c
/* 示例：MC/DC要求每个条件独立影响结果 */
if ((A && B) || C) {
    /* ... */
}

/* 需要测试用例：
 * 1. A=T, B=T, C=F -> 结果=T
 * 2. A=F, B=T, C=F -> 结果=F (A独立影响)
 * 3. A=T, B=F, C=F -> 结果=F (B独立影响)
 * 4. A=F, B=F, C=T -> 结果=T (C独立影响)
 */
```

---

## 与MISRA C关系

DO-178C推荐使用MISRA C作为编码规范，Level A/B通常要求符合MISRA C Mandatory和Required规则。

---

**维护者**: C_Lang Knowledge Base Team
