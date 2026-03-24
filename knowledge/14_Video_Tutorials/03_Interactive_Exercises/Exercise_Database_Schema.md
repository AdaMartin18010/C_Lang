# 练习题数据库设计

> 版本: 1.0
> 日期: 2026-03-19

## 1. 概述

本文档定义在线练习平台的数据库表结构，包括题目、测试用例、用户提交、学习记录等核心数据模型。

---

## 2. 实体关系图

```
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│     users       │       │    problems     │       │  submissions    │
├─────────────────┤       ├─────────────────┤       ├─────────────────┤
│ PK id           │◄──────┤ PK id           │◄──────┤ PK id           │
│    username     │       │    title        │       │ FK user_id      │
│    email        │       │    slug         │       │ FK problem_id   │
│    password_hash│       │    description  │       │    code         │
│    ...          │       │    difficulty   │       │    language     │
└─────────────────┘       │    type         │       │    status       │
         ▲                │ FK category_id  │       │    score        │
         │                │    time_limit   │       │    runtime_ms   │
         │                │    memory_limit │       │    memory_kb    │
         │                │    ...          │       │    ...          │
         │                └─────────────────┘       └─────────────────┘
         │                         ▲                         │
         │                         │                         ▼
         │                         │                ┌─────────────────┐
         │                         │                │submission_results│
         │                         │                ├─────────────────┤
         │                         │                │ PK id           │
         │                ┌────────┴────────┐       │ FK submission_id│
         │                │   categories    │       │ FK test_case_id │
         │                ├─────────────────┤       │    status       │
         │                │ PK id           │       │    runtime_ms   │
         │                │    name         │       │    memory_kb    │
         │                │ FK parent_id    │       │    ...          │
         │                └─────────────────┘       └─────────────────┘
         │
         │                ┌─────────────────┐
         │                │  user_profiles  │
         │                ├─────────────────┤
         └───────────────►│ PK user_id      │
                          │    level        │
                          │    experience   │
                          │    ...          │
                          └─────────────────┘
```

---

## 3. 详细表结构

### 3.1 用户系统

#### users (用户表)

```sql
CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    avatar_url VARCHAR(500),

    -- 状态管理
    status VARCHAR(20) DEFAULT 'active'
        CHECK (status IN ('active', 'suspended', 'deleted')),
    email_verified_at TIMESTAMP,

    -- 时间戳
    last_login_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_users_email ON users(email);
CREATE INDEX idx_users_username ON users(username);
```

#### user_profiles (用户档案)

```sql
CREATE TABLE user_profiles (
    user_id BIGINT PRIMARY KEY REFERENCES users(id) ON DELETE CASCADE,

    -- 等级系统
    level INT DEFAULT 1 CHECK (level >= 1),
    experience INT DEFAULT 0 CHECK (experience >= 0),

    -- 学习统计
    problems_solved INT DEFAULT 0,
    problems_attempted INT DEFAULT 0,
    submissions_count INT DEFAULT 0,
    accepted_count INT DEFAULT 0,

    -- 连续学习
    learning_streak INT DEFAULT 0,
    longest_streak INT DEFAULT 0,
    last_streak_date DATE,

    -- 偏好设置
    preferred_language VARCHAR(20) DEFAULT 'c',
    timezone VARCHAR(50) DEFAULT 'UTC',

    -- 社交
    bio TEXT,
    github_url VARCHAR(255),
    website_url VARCHAR(255),

    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### user_roles (用户角色)

```sql
CREATE TABLE user_roles (
    id SERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    role VARCHAR(30) NOT NULL
        CHECK (role IN ('admin', 'moderator', 'contributor', 'vip', 'user')),
    granted_by BIGINT REFERENCES users(id),
    expires_at TIMESTAMP,  -- NULL表示永久
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(user_id, role)
);
```

---

### 3.2 题目系统

#### problems (题目表)

```sql
CREATE TABLE problems (
    id BIGSERIAL PRIMARY KEY,

    -- 基本信息
    title VARCHAR(255) NOT NULL,
    slug VARCHAR(100) UNIQUE NOT NULL,
    description TEXT NOT NULL,

    -- 分类
    difficulty SMALLINT NOT NULL CHECK (difficulty BETWEEN 1 AND 5),
    type VARCHAR(50) NOT NULL
        CHECK (type IN ('code', 'choice', 'fill_blank', 'error_fix', 'reading')),
    category_id BIGINT REFERENCES categories(id),

    -- 限制
    time_limit_ms INT DEFAULT 1000 CHECK (time_limit_ms > 0),
    memory_limit_mb INT DEFAULT 256 CHECK (memory_limit_mb > 0),
    stack_limit_mb INT DEFAULT 64,

    -- 代码题特有
    function_signature TEXT,  -- 如: int max(int a, int b)
    return_type VARCHAR(50),
    template_code TEXT,

    -- 统计
    total_submissions INT DEFAULT 0,
    accepted_submissions INT DEFAULT 0,
    acceptance_rate DECIMAL(5,2) DEFAULT 0.00,

    -- 元数据
    author_id BIGINT REFERENCES users(id),
    source VARCHAR(255),  -- 题目来源
    hints TEXT[],  -- 提示数组
    is_public BOOLEAN DEFAULT true,

    -- 时间戳
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_problems_difficulty ON problems(difficulty);
CREATE INDEX idx_problems_type ON problems(type);
CREATE INDEX idx_problems_category ON problems(category_id);
CREATE INDEX idx_problems_public ON problems(is_public) WHERE is_public = true;
```

#### categories (分类表)

```sql
CREATE TABLE categories (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    slug VARCHAR(100) UNIQUE NOT NULL,
    description TEXT,
    parent_id BIGINT REFERENCES categories(id),

    -- 层级路径 (用于快速查询)
    path VARCHAR(500),  -- 如: /programming/c/pointers
    depth INT DEFAULT 0,

    -- 排序
    sort_order INT DEFAULT 0,

    -- 统计
    problem_count INT DEFAULT 0,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 知识点标签 (与分类不同，是多对多关系)
CREATE TABLE tags (
    id SERIAL PRIMARY KEY,
    name VARCHAR(50) UNIQUE NOT NULL,
    description TEXT,
    color VARCHAR(7) DEFAULT '#1890ff',  -- 十六进制颜色
    problem_count INT DEFAULT 0
);

CREATE TABLE problem_tags (
    problem_id BIGINT NOT NULL REFERENCES problems(id) ON DELETE CASCADE,
    tag_id INT NOT NULL REFERENCES tags(id) ON DELETE CASCADE,
    PRIMARY KEY (problem_id, tag_id)
);
```

#### test_cases (测试用例)

```sql
CREATE TABLE test_cases (
    id BIGSERIAL PRIMARY KEY,
    problem_id BIGINT NOT NULL REFERENCES problems(id) ON DELETE CASCADE,

    -- 测试数据
    input TEXT,
    expected_output TEXT NOT NULL,

    -- 属性
    is_sample BOOLEAN DEFAULT false,  -- 是否示例测试
    is_active BOOLEAN DEFAULT true,
    weight INT DEFAULT 1,  -- 分值权重
    sort_order INT DEFAULT 0,

    -- 特殊测试 (针对特定边界)
    test_type VARCHAR(30) DEFAULT 'normal'
        CHECK (test_type IN ('normal', 'boundary', 'performance', 'security')),

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_test_cases_problem ON test_cases(problem_id);
CREATE INDEX idx_test_cases_sample ON test_cases(problem_id, is_sample);
```

#### problem_solutions (官方题解)

```sql
CREATE TABLE problem_solutions (
    id BIGSERIAL PRIMARY KEY,
    problem_id BIGINT NOT NULL REFERENCES problems(id) ON DELETE CASCADE,

    title VARCHAR(255),
    explanation TEXT NOT NULL,  -- 详细解释
    code TEXT NOT NULL,  -- 参考代码
    language VARCHAR(20) NOT NULL,

    -- 复杂度分析
    time_complexity VARCHAR(50),
    space_complexity VARCHAR(50),

    -- 元数据
    author_id BIGINT REFERENCES users(id),
    is_official BOOLEAN DEFAULT false,

    -- 统计
    view_count INT DEFAULT 0,
    upvotes INT DEFAULT 0,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

### 3.3 提交系统

#### submissions (提交记录)

```sql
CREATE TYPE submission_status AS ENUM (
    'pending',      -- 等待评测
    'judging',      -- 评测中
    'compiled',     -- 编译完成
    'accepted',     -- 通过
    'wrong_answer', -- 答案错误
    'time_limit_exceeded',    -- 超时
    'memory_limit_exceeded',  -- 超内存
    'runtime_error',          -- 运行时错误
    'compile_error',          -- 编译错误
    'system_error',           -- 系统错误
    'cancelled'               -- 取消
);

CREATE TABLE submissions (
    id BIGSERIAL PRIMARY KEY,

    -- 关联
    user_id BIGINT NOT NULL REFERENCES users(id),
    problem_id BIGINT NOT NULL REFERENCES problems(id),

    -- 提交内容
    code TEXT NOT NULL,
    language VARCHAR(50) NOT NULL,
    compiler_version VARCHAR(50),
    compile_options TEXT,

    -- 评测结果
    status submission_status DEFAULT 'pending',
    score INT CHECK (score >= 0),

    -- 资源使用
    runtime_ms INT CHECK (runtime_ms >= 0),
    memory_kb INT CHECK (memory_kb >= 0),

    -- 统计
    test_cases_total INT DEFAULT 0,
    test_cases_passed INT DEFAULT 0,

    -- 编译信息
    compile_output TEXT,
    error_message TEXT,

    -- 执行信息
    judged_at TIMESTAMP,
    judged_by VARCHAR(100),  -- 评测节点标识

    -- 元数据
    ip_address INET,
    user_agent TEXT,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_submissions_user ON submissions(user_id);
CREATE INDEX idx_submissions_problem ON submissions(problem_id);
CREATE INDEX idx_submissions_status ON submissions(status);
CREATE INDEX idx_submissions_created ON submissions(created_at);
CREATE INDEX idx_submissions_user_problem ON submissions(user_id, problem_id);
```

#### submission_results (提交详细结果)

```sql
CREATE TABLE submission_results (
    id BIGSERIAL PRIMARY KEY,
    submission_id BIGINT NOT NULL REFERENCES submissions(id) ON DELETE CASCADE,
    test_case_id BIGINT REFERENCES test_cases(id),  -- NULL for internal errors

    -- 结果
    status submission_status NOT NULL,
    runtime_ms INT CHECK (runtime_ms >= 0),
    memory_kb INT CHECK (memory_kb >= 0),

    -- 输出 (只存储前N字节)
    actual_output TEXT,
    error_output TEXT,

    -- 差异信息 (WA时有用)
    diff_info JSONB,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_results_submission ON submission_results(submission_id);
CREATE INDEX idx_results_test_case ON submission_results(test_case_id);
```

---

### 3.4 学习系统

#### learning_paths (学习路径)

```sql
CREATE TABLE learning_paths (
    id SERIAL PRIMARY KEY,

    name VARCHAR(255) NOT NULL,
    description TEXT,
    slug VARCHAR(100) UNIQUE NOT NULL,

    -- 难度范围
    min_difficulty SMALLINT DEFAULT 1,
    max_difficulty SMALLINT DEFAULT 5,

    -- 元数据
    is_public BOOLEAN DEFAULT true,
    is_official BOOLEAN DEFAULT false,
    author_id BIGINT REFERENCES users(id),

    -- 统计
    enrolled_count INT DEFAULT 0,
    completed_count INT DEFAULT 0,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 路径中的题目顺序
CREATE TABLE path_problems (
    path_id INT NOT NULL REFERENCES learning_paths(id) ON DELETE CASCADE,
    problem_id BIGINT NOT NULL REFERENCES problems(id) ON DELETE CASCADE,

    stage INT NOT NULL,  -- 阶段
    order_num INT NOT NULL,  -- 顺序
    is_required BOOLEAN DEFAULT true,  -- 是否必做

    PRIMARY KEY (path_id, problem_id)
);
```

#### user_learning_progress (用户学习进度)

```sql
CREATE TABLE user_learning_progress (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    path_id INT NOT NULL REFERENCES learning_paths(id) ON DELETE CASCADE,

    -- 进度
    current_stage INT DEFAULT 1,
    problems_completed INT DEFAULT 0,
    total_problems INT NOT NULL,
    progress_percent DECIMAL(5,2) DEFAULT 0.00,

    -- 状态
    status VARCHAR(20) DEFAULT 'in_progress'
        CHECK (status IN ('in_progress', 'completed', 'paused')),

    -- 时间
    started_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    last_activity_at TIMESTAMP,

    UNIQUE(user_id, path_id)
);
```

#### user_problem_status (用户-题目状态)

```sql
CREATE TABLE user_problem_status (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    problem_id BIGINT NOT NULL REFERENCES problems(id) ON DELETE CASCADE,

    -- 状态
    status VARCHAR(20) DEFAULT 'unsolved'
        CHECK (status IN ('unsolved', 'attempted', 'solved', 'mastered')),

    -- 提交统计
    attempts_count INT DEFAULT 0,
    accepted_count INT DEFAULT 0,

    -- 首次和最近通过
    first_accepted_at TIMESTAMP,
    last_accepted_at TIMESTAMP,

    -- 最佳成绩
    best_runtime_ms INT,
    best_memory_kb INT,

    -- 复习安排 (基于遗忘曲线)
    next_review_at TIMESTAMP,
    review_count INT DEFAULT 0,

    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(user_id, problem_id)
);

CREATE INDEX idx_ups_user ON user_problem_status(user_id);
CREATE INDEX idx_ups_problem ON user_problem_status(problem_id);
CREATE INDEX idx_ups_status ON user_problem_status(status);
```

---

### 3.5 社区系统

#### discussions (讨论)

```sql
CREATE TABLE discussions (
    id BIGSERIAL PRIMARY KEY,

    -- 关联
    problem_id BIGINT REFERENCES problems(id) ON DELETE CASCADE,
    parent_id BIGINT REFERENCES discussions(id),  -- 回复

    -- 内容
    author_id BIGINT NOT NULL REFERENCES users(id),
    title VARCHAR(255),
    content TEXT NOT NULL,

    -- 类型
    type VARCHAR(30) DEFAULT 'question'
        CHECK (type IN ('question', 'solution', 'hint', 'general')),

    -- 统计
    view_count INT DEFAULT 0,
    reply_count INT DEFAULT 0,
    upvotes INT DEFAULT 0,

    -- 状态
    is_accepted BOOLEAN DEFAULT false,  -- 问题被接受
    is_pinned BOOLEAN DEFAULT false,
    is_deleted BOOLEAN DEFAULT false,

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_discussions_problem ON discussions(problem_id);
CREATE INDEX idx_discussions_author ON discussions(author_id);
CREATE INDEX idx_discussions_type ON discussions(type);
```

---

### 3.6 评分规则

#### 评分系统设计

**代码题评分**:

```sql
-- 测试用例得分 = 基础分 × 权重
-- 总分 = Σ(通过的测试用例得分) / Σ(所有测试用例权重) × 100

-- 示例评分计算
CREATE OR REPLACE FUNCTION calculate_submission_score(submission_id BIGINT)
RETURNS INT AS $$
DECLARE
    total_weight INT;
    passed_weight INT;
    final_score INT;
BEGIN
    SELECT COALESCE(SUM(tc.weight), 0)
    INTO total_weight
    FROM test_cases tc
    JOIN submissions s ON s.problem_id = tc.problem_id
    WHERE s.id = submission_id AND tc.is_active = true;

    SELECT COALESCE(SUM(tc.weight), 0)
    INTO passed_weight
    FROM submission_results sr
    JOIN test_cases tc ON sr.test_case_id = tc.id
    WHERE sr.submission_id = submission_id
      AND sr.status = 'accepted'
      AND tc.is_active = true;

    IF total_weight = 0 THEN
        RETURN 0;
    END IF;

    final_score := (passed_weight * 100) / total_weight;
    RETURN final_score;
END;
$$ LANGUAGE plpgsql;
```

**排行榜积分** (Elo-like系统):

```sql
-- 用户等级所需经验值
CREATE TABLE level_requirements (
    level INT PRIMARY KEY,
    required_exp INT NOT NULL,
    title VARCHAR(50)  -- 如: "新手", "学徒", "专家"
);

-- 经验值获取规则
-- 首次通过题目: base_exp × difficulty
-- 重复通过: 10% base_exp
-- 优化挑战 (超过当前最优): bonus_exp
```

---

## 4. 时序数据设计

### 4.1 用户活动日志

使用 ClickHouse 存储：

```sql
-- 提交活动表
CREATE TABLE submission_events (
    event_date Date,
    event_time DateTime,

    user_id UInt64,
    problem_id UInt64,
    submission_id UInt64,

    language String,
    status String,
    runtime_ms UInt32,
    memory_kb UInt32,

    -- 分区和排序
    PROJECTION projection_user (
        SELECT * ORDER BY user_id, event_time
    )
) ENGINE = MergeTree()
PARTITION BY toYYYYMM(event_date)
ORDER BY (event_time, user_id);

-- 学习活动表
CREATE TABLE learning_events (
    event_date Date,
    event_time DateTime,

    user_id UInt64,
    event_type String,  -- 'view', 'submit', 'pass', 'hint_used'
    problem_id UInt64,

    duration_sec UInt32  -- 停留时间
) ENGINE = MergeTree()
PARTITION BY toYYYYMM(event_date)
ORDER BY (event_time, user_id);
```

---

## 5. 缓存策略

### 5.1 Redis 缓存键设计

```
# 用户会话

---

## 🔗 全面知识关联体系

### 【全局层】知识库导航
| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

### 【阶段层】学习定位
**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```

学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]

```

### 【层次层】纵向知识链
| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网
| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

### 【总体层】知识体系架构
```

┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘

```

### 【决策层】学习路径选择
| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

---

## 🔗 知识关联网络

### 1. 全局导航
| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [../../readme.md](../../readme.md) | 模块总览与导航 |
| 学习路径 | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 推荐学习路线 |

### 2. 前置知识依赖
| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md](../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联
| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 技术扩展 | 并发编程技术 |
| [../../02_Formal_Semantics_and_Physics/readme.md](../../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联
| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义基础 | 操作语义、类型理论 |
| [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸
| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统技术 | 系统级开发 |
| [../../01_Core_Knowledge_System/09_Safety_Standards/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全标准 | 安全编码规范 |
| [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位
```

当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器

```

### 7. 局部索引
本文件所属模块的详细内容：
- 参见本模块 [readme.md](../../readme.md)
- 相关子目录文档

session:{jwt_token} -> {user_id, expires_at}
TTL: 7 days

# 用户信息缓存
user:{user_id} -> JSON
TTL: 1 hour

# 题目列表 (分页缓存)
problems:list:{filter_hash}:{page}:{size} -> JSON
TTL: 10 minutes

# 题目详情
problem:{problem_id} -> JSON
TTL: 1 hour

# 提交结果
submission:{submission_id} -> JSON
TTL: 1 hour

# 排行榜
leaderboard:daily:{date} -> Sorted Set (user_id -> score)
leaderboard:weekly:{week} -> Sorted Set
leaderboard:overall -> Sorted Set
TTL: 1 hour

# 用户做题状态缓存
user_status:{user_id}:{problem_id} -> JSON
TTL: 30 minutes

# 统计计数 (使用 HyperLogLog 或精确计数)
count:problems:solved:{user_id} -> Integer
count:submissions:daily:{date} -> Integer
```

---

## 6. 索引策略

### 6.1 常用查询优化

```sql
-- 查询用户的所有通过题目
-- 已有: idx_ups_user
-- 优化: 复合索引
CREATE INDEX idx_ups_user_status ON user_problem_status(user_id, status);

-- 查询题目的提交统计
-- 已有: idx_submissions_problem
-- 优化: 覆盖索引
CREATE INDEX idx_submissions_problem_status
ON submissions(problem_id, status)
INCLUDE (runtime_ms, memory_kb);

-- 排行榜查询优化
CREATE INDEX idx_submissions_user_score
ON submissions(user_id, score DESC)
WHERE status = 'accepted';
```

---

## 7. 数据归档

### 7.1 归档策略

```sql
-- 将超过90天的提交数据归档到冷存储
-- 保留最近提交用于快速查询

CREATE TABLE submissions_archive (LIKE submissions INCLUDING ALL);

-- 定期归档任务
INSERT INTO submissions_archive
SELECT * FROM submissions
WHERE created_at < NOW() - INTERVAL '90 days'
  AND id NOT IN (
      SELECT DISTINCT ON (user_id, problem_id) id
      FROM submissions
      WHERE status = 'accepted'
      ORDER BY user_id, problem_id, created_at DESC
  );

DELETE FROM submissions
WHERE created_at < NOW() - INTERVAL '90 days'
  AND id NOT IN (
      SELECT DISTINCT ON (user_id, problem_id) id
      FROM submissions
      WHERE status = 'accepted'
      ORDER BY user_id, problem_id, created_at DESC
  );
```

---

## 8. 安全考虑

### 8.1 数据脱敏

```sql
-- 导出数据时脱敏敏感信息
CREATE VIEW submissions_safe AS
SELECT
    id,
    problem_id,
    language,
    status,
    score,
    runtime_ms,
    memory_kb,
    created_at
FROM submissions;
-- 不包含 code, ip_address, user_agent
```

### 8.2 审计日志

```sql
CREATE TABLE audit_log (
    id BIGSERIAL PRIMARY KEY,
    table_name VARCHAR(50) NOT NULL,
    record_id BIGINT NOT NULL,
    action VARCHAR(20) NOT NULL,  -- INSERT, UPDATE, DELETE
    old_data JSONB,
    new_data JSONB,
    changed_by BIGINT REFERENCES users(id),
    changed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

## 9. 扩展性设计

### 9.1 分库分表策略

**水平分表 - submissions**:

- 按 user_id % 16 分表
- 或按时间分区 (每月一表)

**读写分离**:

- 主库: 写操作
- 从库: 读操作

### 9.2 多租户支持 (可选)

```sql
-- 添加租户标识
ALTER TABLE users ADD COLUMN tenant_id INT DEFAULT 0;
ALTER TABLE problems ADD COLUMN tenant_id INT DEFAULT 0;

-- 查询时自动过滤
CREATE POLICY tenant_isolation ON users
    USING (tenant_id = current_setting('app.current_tenant')::INT);
```

---

## 10. 附录

### 10.1 命名规范

- 表名: 小写，复数，下划线分隔
- 列名: 小写，下划线分隔
- 索引: idx_{table}_{columns}
- 约束: {table}_{column}_{type}

### 10.2 数据类型选择

| 场景 | 推荐类型 | 说明 |
|-----|---------|------|
| 主键 | BIGINT | 支持分布式ID |
| 枚举 | VARCHAR + CHECK | 或自定义TYPE |
| JSON | JSONB | PostgreSQL |
| 时间戳 | TIMESTAMP WITH TIME ZONE | 统一存储UTC |
| IP地址 | INET | PostgreSQL特有 |
| 大文本 | TEXT | 限制应用层处理 |

### 10.3 参考

- PostgreSQL 14 文档
- ClickHouse 文档
- Redis 最佳实践


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
