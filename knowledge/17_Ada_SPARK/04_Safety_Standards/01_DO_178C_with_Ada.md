# DO-178C与Ada

## 1. DO-178C概述

### 1.1 什么是DO-178C？

DO-178C（Software Considerations in Airborne Systems and Equipment Certification）是国际航空电子软件认证标准：

| 属性 | 说明 |
|------|------|
| **全称** | Software Considerations in Airborne Systems and Equipment Certification |
| **发布** | 2012年（取代DO-178B） |
| **适用范围** | 民用航空机载软件 |
| **认可机构** | FAA（美国）、EASA（欧洲）、Transport Canada |

### 1.2 软件等级（DAL）

| 等级 | 故障影响 | 失效概率目标 | 典型应用 |
|------|----------|-------------|----------|
| **A** | 灾难性 (Catastrophic) | 10⁻⁹/飞行小时 | 飞行控制 |
| **B** | 危险 (Hazardous) | 10⁻⁷/飞行小时 | 自动油门 |
| **C** | 重大 (Major) | 10⁻⁶/飞行小时 | 飞行管理 |
| **D** | 轻微 (Minor) | 10⁻⁵/飞行小时 | 维护系统 |
| **E** | 无影响 (No Effect) | 无要求 | 乘客娱乐 |

---

## 2. Ada在DO-178C中的地位

### 2.1 语言选择建议

```
DO-178C软件计划过程 → 选择编程语言
├── 强类型语言推荐
│   ├── Ada (强烈推荐)
│   ├── SPARK Ada (高完整性)
│   └── C with MISRA (可接受)
├── 语言子集
│   ├── 定义允许使用的语言特性
│   └── 禁止使用易出错特性
└── 编码标准
    ├── 命名规范
    ├── 注释要求
    └── 复杂性限制
```

### 2.2 Ada vs C在DO-178C中的对比

| 方面 | Ada | C (MISRA) |
|------|-----|-----------|
| **类型安全** | 编译器强检查 | 需编码规范约束 |
| **数组边界** | 运行时检查 | 需静态分析 |
| **内存管理** | 范围检查+可控 | 需额外工具 |
| **并发** | 语言原生支持 | 库依赖 |
| **Ravenscar** | 适合高DAL | 不适用 |
| **形式化验证** | SPARK支持 | 有限支持 |
| **历史验证** | 波音787、F-35 | 较少 |

---

## 3. Ada开发流程

### 3.1 DO-178C核心流程

```
1. 软件计划过程
   └── 软件开发计划 (SDP)
       ├── 使用Ada语言的理由
       ├── Ada编译器资格
       └── SPARK验证计划 (可选)

2. 软件开发过程
   ├── 软件需求分析
   │   └── 系统需求 → 软件高层需求
   ├── 软件设计
   │   ├── 架构设计 (Ada包结构)
   │   └── 详细设计 (SPARK合约)
   ├── 软件编码
   │   ├── Ada编码标准
   │   └── 代码审查
   └── 软件集成
       └── 构建和链接

3. 软件验证过程
   ├── 评审和分析
   │   ├── 需求评审
   │   ├── 设计评审
   │   └── 代码评审
   ├── 测试
   │   ├── 基于需求的测试
   │   ├── 结构覆盖分析
   │   └── 形式化方法 (SPARK)
   └── 验证结果分析

4. 软件配置管理
5. 软件质量保证
6. 合格审定联络
```

### 3.2 Ada编码标准示例

```ada
-- 命名规范
package Flight_Control is
   -- 类型: PascalCase
   type Control_Surface is (Elevator, Aileron, Rudder);
   type Angle_Degrees is range -90 .. 90;

   -- 常量: 大写下划线
   Max_Deflection : constant Angle_Degrees := 30;

   -- 变量/参数: 蛇形命名
   procedure Set_Surface_Angle (
      Surface : in Control_Surface;
      Angle   : in Angle_Degrees
   ) with
      Pre  => Angle in -Max_Deflection .. Max_Deflection,
      Post => Get_Surface_Angle (Surface) = Angle;

   function Get_Surface_Angle (Surface : Control_Surface)
      return Angle_Degrees;

private
   -- 私有变量前缀
   Current_Angle : array (Control_Surface) of Angle_Degrees :=
      (others => 0);
end Flight_Control;

-- 限制使用
type Unconstrained_Array is array (Positive range <>) of Float;
-- DAL A/B: 需要额外验证无约束数组的使用
```

---

## 4. 结构覆盖要求

### 4.1 覆盖等级与DAL关系

| 覆盖类型 | DAL A | DAL B | DAL C | DAL D |
|----------|-------|-------|-------|-------|
| 语句覆盖 | ✓ | ✓ | ✓ | ✗ |
| 决策覆盖 | ✓ | ✓ | ✗ | ✗ |
| MC/DC | ✓ | ✗ | ✗ | ✗ |

### 4.2 Ada代码覆盖分析

```ada
-- 决策覆盖示例
procedure Control_Logic (Alt : Altitude; Speed : Airspeed) is
begin
   -- 决策点1: if语句
   if Alt < 1000 and Speed > 200 then  -- 需要测试4种组合
      -- 条件1 | 条件2 | 结果
      --   T   |   T   | 执行
      --   T   |   F   | 跳过
      --   F   |   T   | 跳过
      --   F   |   F   | 跳过
      Lower_Landing_Gear;
   end if;

   -- 决策点2: case语句
   case Flight_Phase is  -- 每个分支都要覆盖
      when Takeoff =>
         Set_Flaps (Takeoff_Position);
      when Cruise =>
         Set_Flaps (Retracted);
      when Landing =>
         Set_Flaps (Landing_Position);
   end case;

   -- 决策点3: 循环
   while Fuel_Remaining < Reserve_Fuel loop  -- 需要测试T和F
      Declare_Fuel_Emergency;
   end loop;
end Control_Logic;
```

---

## 5. SPARK在DO-178C中的应用

### 5.1 替代验证方法

```
DO-178C新增技术补充
├── DO-330: 工具鉴定
├── DO-331: 模型驱动开发
├── DO-332: 面向对象技术
├── DO-333: 形式化方法 ← SPARK应用
└── DO-334: 多核处理器
```

### 5.2 使用SPARK满足MC/DC

```ada
-- 传统测试方法: 需要大量测试用例
-- SPARK方法: 形式化证明替代部分测试

package body Flight_Control with SPARK_Mode is
   procedure Calculate_Control (
      Input  : in Sensor_Data;
      Output : out Control_Command
   ) with
      Pre  => Input.Valid,
      Post => Output.Within_Safe_Limits
   is
   begin
      -- 复杂决策逻辑
      if Input.Altitude < Threshold_1 then
         if Input.Speed < Threshold_2 then
            Output := Low_Altitude_Low_Speed (Input);
         else
            Output := Low_Altitude_High_Speed (Input);
         end if;
      else
         Output := Normal_Operation (Input);
      end if;
   end Calculate_Control;

   -- SPARK证明:
   -- 1. 无运行时错误
   -- 2. 后置条件满足
   -- 3. 可作为MC/DC的补充证据
end Flight_Control;
```

---

## 6. 工业案例

### 6.1 波音787

```
项目: 波音787梦幻客机
软件规模: 6.5百万行代码
Ada代码比例: ~80%
DAL分布:
  - DAL A: 飞行控制、引擎控制
  - DAL B: 导航系统
  - DAL C: 客舱管理
方法:
  - Ada + SPARK (DAL A模块)
  - 模型驱动开发
  - 自动化测试
认证: FAA/EASA双认证
```

### 6.2 F-35战斗机

```
项目: F-35联合攻击战斗机
软件规模: 8+百万行代码
Ada代码比例: ~90%
特点:
  - SPARK形式化验证关键模块
  - DO-178C DAL A严格认证
  - 多版本并行开发
```

---

## 7. 工具资格

### 7.1 Ada工具链鉴定

| 工具类型 | 工具示例 | 鉴定等级 |
|---------|----------|---------|
| Ada编译器 | GNAT Pro | TQL-1 |
| 静态分析 | SPARK Pro | TQL-1 |
| 覆盖率 | AdaCoverage | TQL-3 |
| 需求管理 | DOORS | TQL-4 |

### 7.2 开发环境配置

```gpr
-- 项目文件 (flight_control.gpr)
project Flight_Control is
   for Source_Dirs use ("src");
   for Object_Dir use "obj";
   for Exec_Dir use "bin";

   -- DO-178C合规编译选项
   package Builder is
      for Global_Configuration_Pragmas use "pragmas.adc";
   end Builder;

   package Compiler is
      for Switches ("Ada") use (
         "-gnatwa",     -- 所有警告
         "-gnaty",      -- 风格检查
         "-O2",         -- 优化级别
         "-gnata"       -- 断言启用
      );
   end Compiler;

   package Prove is
      for Proof_Switches ("Ada") use ("--level=4");
   end Prove;
end Flight_Control;
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**维护者**: C_Lang Knowledge Base Team


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
