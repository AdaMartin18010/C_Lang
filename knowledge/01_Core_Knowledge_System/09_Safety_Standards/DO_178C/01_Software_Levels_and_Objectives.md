# DO-178C 软件等级与认证目标

> 航空软件设计保证等级(DAL)和认证目标详解

---

## 软件设计保证等级 (DAL)

| DAL | 失效条件 | 对飞机/乘员影响 | 示例系统 |
|:---:|:---------|:----------------|:---------|
| **A** | 灾难性 (Catastrophic) | 导致多人死亡，通常失去飞机 | 飞控主计算机、引擎FADEC |
| **B** | 危险的/严重的 (Hazardous/Severe) | 严重伤害，飞机严重损坏 | 自动驾驶、飞行管理 |
| **C** | 重大的 (Major) | 工作负荷增加，舒适度降低 | 通信系统、燃油管理 |
| **D** | 较小的 (Minor) | 轻微不便 | 客舱娱乐、维护辅助 |
| **E** | 无安全影响 (No Effect) | 无影响 | 乘客信息服务 |

---

## 认证目标总结

### 软件计划过程

| 目标 | DAL A | DAL B | DAL C | DAL D | DAL E |
|:----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| 计划软件生命周期 | ✓ | ✓ | ✓ | ✓ | - |
| 定义开发标准 | ✓ | ✓ | ✓ | - | - |
| 定义附加目标 | ✓ | - | - | - | - |

### 软件开发过程

| 目标 | DAL A | DAL B | DAL C | DAL D | DAL E |
|:----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| 软件需求开发 | ✓ | ✓ | ✓ | ✓ | - |
| 软件设计 | ✓ | ✓ | ✓ | - | - |
| 软件编码 | ✓ | ✓ | - | - | - |
| 集成 | ✓ | ✓ | ✓ | ✓ | - |

### 软件验证过程

| 目标 | DAL A | DAL B | DAL C | DAL D | DAL E |
|:----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| 需求可追踪性 | ✓ | ✓ | ✓ | - | - |
| 需求正确性 | ✓ | ✓ | ✓ | - | - |
| 需求完整性 | ✓ | ✓ | ✓ | - | - |
| 算法正确性 | ✓ | ✓ | - | - | - |
| 软件正确性 | ✓ | ✓ | ✓ | - | - |
| 软件完整性 | ✓ | ✓ | ✓ | - | - |

---

## 结构覆盖率要求

### 覆盖率目标

| 覆盖率类型 | DAL A | DAL B | DAL C | DAL D | DAL E |
|:-----------|:-----:|:-----:|:-----:|:-----:|:-----:|
| 语句覆盖 | ✓ | ✓ | ✓ | - | - |
| 判定覆盖 | ✓ | ✓ | - | - | - |
| MC/DC | ✓ | - | - | - | - |

### MC/DC (Modified Condition/Decision Coverage)

MC/DC是DAL A软件必须满足的最高级别覆盖率要求。

#### MC/DC原理

```c
/**
 * MC/DC要求：
 * 1. 每个条件的每个可能结果至少出现一次
 * 2. 每个判定至少出现一次所有可能结果
 * 3. 每个条件独立影响判定输出
 */

/* 示例判定 */
if ((A && B) || C) {
    action();
}

/*
 * 真值表：
 * A B C | 结果 | 测试用例
 * ------+------+----------
 * T T T |   T  |  1
 * T T F |   T  |  2
 * T F T |   T  |  3
 * T F F |   F  |  4
 * F T T |   T  |  5
 * F T F |   F  |  6
 * F F T |   T  |  7
 * F F F |   F  |  8
 *
 * MC/DC最小测试集（证明每个条件独立影响输出）：
 * - A: 用例2(T,T,F→T) 和 6(F,T,F→F) - B=T,C=F时A改变输出
 * - B: 用例2(T,T,F→T) 和 4(T,F,F→F) - A=T,C=F时B改变输出
 * - C: 用例4(T,F,F→F) 和 7(F,F,T→T) - A=F,B=F时C改变输出
 *
 * 最小测试集: {2, 4, 6, 7} 或 {2, 4, 7} + {6或8}
 */
```

#### MC/DC测试用例生成

```c
/**
 * DAL A级别函数 - MC/DC覆盖要求
 */

/* 判定1: 高度检查 */
bool check_altitude(int32_t current, int32_t target, int32_t tolerance)
{
    int32_t diff = abs(current - target);

    /* 判定: (diff <= tolerance) && (current > 0) */
    if ((diff <= tolerance) && (current > 0)) {
        return true;  /* 在目标高度范围内 */
    }
    return false;
}

/* MC/DC测试用例 */
void test_check_altitude(void)
{
    /* 证明diff <= tolerance独立影响输出 */
    TEST(check_altitude(1000, 1005, 10, 1000) == true);   /* T && T = T */
    TEST(check_altitude(1000, 1050, 10, 1000) == false);  /* F && T = F */

    /* 证明current > 0独立影响输出 */
    TEST(check_altitude(1000, 1005, 10, 1000) == true);   /* T && T = T */
    TEST(check_altitude(-100, 1005, 10, -100) == false);  /* T && F = F */
}

/* 复杂判定 */
bool complex_decision(bool a, bool b, bool c, bool d)
{
    /* 判定: (a && b) || (c && d) */
    return (a && b) || (c && d);
}

/* MC/DC需要证明a,b,c,d每个都能独立影响输出 */
void test_complex_decision(void)
{
    /* a独立: 其他固定，改变a改变输出 */
    TEST(complex_decision(true, true, false, false) == true);   /* T&&T || F = T */
    TEST(complex_decision(false, true, false, false) == false); /* F&&T || F = F */

    /* b独立 */
    TEST(complex_decision(true, true, false, false) == true);   /* T&&T || F = T */
    TEST(complex_decision(true, false, false, false) == false); /* T&&F || F = F */

    /* c独立 */
    TEST(complex_decision(false, false, true, true) == true);   /* F || T&&T = T */
    TEST(complex_decision(false, false, false, true) == false); /* F || F&&T = F */

    /* d独立 */
    TEST(complex_decision(false, false, true, true) == true);   /* F || T&&T = T */
    TEST(complex_decision(false, false, true, false) == false); /* F || T&&F = F */
}
```

---

## DAL A软件开发要求

### 编码标准

```c
/**
 * DAL A级别编码要求
 */

/* 1. 无动态内存分配 */
/* 禁止: malloc/free在运行时 */

/* 2. 无递归 */
int factorial(int n)  /* 不允许递归 */
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* 3. 所有路径有确定行为 */
int divide(int a, int b, int *result)
{
    if (b == 0) {
        *result = 0;
        return ERROR_DIV_ZERO;  /* 明确处理 */
    }
    *result = a / b;
    return SUCCESS;
}

/* 4. 初始化所有变量 */
void process(void)
{
    int sum = 0;  /* 初始化 */
    for (int i = 0; i < 10; i++) {
        sum += i;
    }
}

/* 5. 无未使用代码 */
/* 所有函数、变量、宏必须使用 */

/* 6. 单一出口点 */
int func(int x)
{
    int result;
    if (x < 0) {
        result = ERROR_NEGATIVE;
    } else if (x > 100) {
        result = ERROR_TOO_LARGE;
    } else {
        result = process(x);
    }
    return result;  /* 单一出口 */
}
```

### 数据流分析

```c
/**
 * DAL A要求进行数据流分析，确保：
 * - 所有变量在使用前定义
 * - 无死存储
 * - 无内存泄漏
 */

/* ✅ 良好 - 明确定义和使用 */
int good_example(void)
{
    int a = 10;  /* d(definition) */
    int b = a + 5;  /* a被使用(r), b被定义(d) */
    return b;  /* b被使用(r) */
}

/* ❌ 问题 - 未定义使用 */
int bad_example(void)
{
    int a;  /* 未初始化 */
    int b = a + 5;  /* a被使用但未定义！ */
    return b;
}

/* ❌ 问题 - 死存储 */
int dead_store(void)
{
    int a = 10;  /* d */
    a = 20;  /* 死存储 - 上一个值未被使用 */
    return a;
}
```

---

## 验证过程

### 测试要求

```markdown
## DAL A测试要求

### 单元测试
- 100% 语句覆盖
- 100% 判定覆盖
- MC/DC覆盖
- 边界值分析
- 等价类划分
- 错误猜测

### 集成测试
- 软硬件接口测试
- 组件间接口测试
- 时序分析

### 系统测试
- 需求覆盖
- 功能测试
- 鲁棒性测试
- 压力测试
- 性能测试

### 验证评审
- 需求评审
- 设计评审
- 代码评审
- 测试评审
```

### 工具鉴定

```c
/**
 * DO-178C要求对用于验证的工具进行鉴定
 */

/* 工具鉴定等级 */
#define TOOL_QUALIFICATION_LEVEL_1 1  /* 用于检测或避免错误 */
#define TOOL_QUALIFICATION_LEVEL_2 2  /* 用于验证或验证 */

/* 需要鉴定的工具类型:
 * - 编译器 (如果用于验证)
 * - 静态分析工具
 * - 测试覆盖工具
 * - 需求管理工具
 * - 配置管理工具
 */
```

---

## 补充标准 (DO-178C家族)

| 标准 | 名称 | 用途 |
|:-----|:-----|:-----|
| DO-330 | Software Tool Qualification Considerations | 工具鉴定 |
| DO-331 | Model-Based Development and Verification | 基于模型的开发 |
| DO-332 | Object-Oriented Technology and Related Techniques | 面向对象技术 |
| DO-333 | Formal Methods | 形式化方法 |
| DO-178C | Core Document | 核心标准 |

---

**参考**: RTCA DO-178C Software Considerations in Airborne Systems and Equipment Certification
