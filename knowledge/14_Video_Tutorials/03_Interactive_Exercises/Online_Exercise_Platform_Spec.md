# 在线练习平台规格说明

> 版本: 1.0
> 日期: 2026-03-19
> 状态: 设计文档

## 1. 项目概述

### 1.1 目标

构建一个面向C语言学习者的在线练习平台，提供：

- 交互式编程练习
- 实时代码评测
- 个性化学习路径
- 社区讨论功能

### 1.2 目标用户

- 编程初学者
- 计算机专业学生
- 准备技术面试的开发者
- 嵌入式系统学习者

### 1.3 核心价值

1. **即时反馈**: 代码提交后立即获得结果和反馈
2. **循序渐进**: 从基础到高级的系统化学习路径
3. **实战导向**: 贴近实际工程的编程练习
4. **数据驱动**: 学习进度和弱点的可视化分析

---

## 2. 功能需求

### 2.1 用户系统

#### 2.1.1 认证与授权

- 邮箱/手机号注册
- 第三方登录 (GitHub, Google)
- 密码重置
- 双因素认证 (可选)

#### 2.1.2 用户档案

- 基本信息管理
- 学习偏好设置
- 技能评估
- 学习历史记录

#### 2.1.3 权限管理

- 普通用户
- 高级会员
- 管理员
- 内容贡献者

### 2.2 练习系统

#### 2.2.1 题库管理

- 多级分类 (知识点/难度/类型)
- 标签系统
- 题目搜索和过滤
- 题目收藏

#### 2.2.2 练习模式

| 模式 | 说明 | 适用场景 |
|-----|------|---------|
| 练习模式 | 不限时，可查看提示 | 学习新知识 |
| 测试模式 | 限时，独立完成 | 自我检测 |
| 挑战模式 | 与其他用户竞争 | 激发动力 |
| 考试模式 | 模拟真实考试 | 考前准备 |

#### 2.2.3 题目类型

1. **代码编写题**
   - 函数实现
   - 完整程序
   - 项目级别

2. **选择题**
   - 单选
   - 多选
   - 判断

3. **填空题**
   - 代码补全
   - 概念填空

4. **改错题**
   - 找出代码中的错误
   - 修复bug

5. **阅读题**
   - 分析代码输出
   - 理解程序行为

### 2.3 代码评测系统

#### 2.3.1 评测流程

```
用户提交代码
      ↓
[语法检查] → 失败 → 返回编译错误
      ↓ 通过
[静态分析] → 警告 → 返回代码质量报告
      ↓
[编译] → 失败 → 返回编译错误
      ↓ 通过
[运行测试用例]
      ↓
[对比输出] → 不匹配 → 返回失败用例
      ↓ 全部通过
[性能测试] → 超时/超内存 → 返回资源限制
      ↓ 通过
[返回通过 + 性能报告]
```

#### 2.3.2 安全沙箱

- 容器隔离 (Docker/Kata Containers)
- 资源限制 (CPU时间、内存、磁盘)
- 系统调用过滤 (seccomp-bpf)
- 网络隔离

#### 2.3.3 支持的编译器

- GCC 11/12/13
- Clang 14/15/16
- MSVC (Windows)
- 嵌入式编译器 (arm-none-eabi-gcc)

#### 2.3.4 评测结果

- 编译信息
- 运行结果
- 测试用例详情
- 时间和内存使用
- 代码质量报告
- 与最优解的对比

### 2.4 学习系统

#### 2.4.1 学习路径

- 预设学习路线 (入门 → 进阶 → 专家)
- 自定义学习路径
- 知识点依赖图
- 学习进度追踪

#### 2.4.2 自适应学习

- 根据表现调整难度
- 薄弱环节强化
- 智能推荐题目
- 遗忘曲线复习

#### 2.4.3 成就系统

- 徽章 (首次通过、连续正确、全星通过)
- 积分和等级
- 排行榜
- 学习 streak

### 2.5 社区功能

#### 2.5.1 讨论区

- 题目讨论
- 题解分享
- 技术问答
- 经验分享

#### 2.5.2 代码分享

- 公开题解
- 代码片段
- 学习笔记

#### 2.5.3 协作功能

- 学习小组
- 编程竞赛
- 结对编程 (可选)

### 2.6 管理后台

#### 2.6.1 内容管理

- 题目CRUD
- 测试用例管理
- 分类和标签管理
- 内容审核

#### 2.6.2 用户管理

- 用户信息查看
- 权限管理
- 行为分析

#### 2.6.3 系统监控

- 评测队列状态
- 系统资源使用
- 错误日志
- 性能指标

---

## 3. 技术架构

### 3.1 系统架构图

```
┌─────────────────────────────────────────────────────────────┐
│                        前端层                                │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   Web App   │  │   Mobile    │  │    IDE      │          │
│  │  (React)    │  │   (PWA)     │  │   Plugin    │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
└─────────────────────────┬───────────────────────────────────┘
                          │ HTTPS/HTTP2
┌─────────────────────────┴───────────────────────────────────┐
│                      API 网关层                              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │    Nginx    │  │  Rate Limit │  │  Auth/JWT   │          │
│  │   (LB)      │  │             │  │             │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────┴───────────────────────────────────┐
│                     微服务层                                 │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │  User Svc   │  │ Problem Svc │  │  Judge Svc  │          │
│  │  (Go/Rust)  │  │  (Go)       │  │  (Go)       │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │  Learn Svc  │  │  Forum Svc  │  │  Notif Svc  │          │
│  │  (Python)   │  │  (Go)       │  │  (Go)       │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────┴───────────────────────────────────┐
│                      数据层                                  │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │ PostgreSQL  │  │    Redis    │  │ Elasticsearch│          │
│  │  (主数据库)  │  │   (缓存)     │  │   (搜索)     │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   MinIO     │  │  RabbitMQ   │  │  ClickHouse  │          │
│  │  (对象存储)  │  │   (消息队列) │  │  (时序数据)  │          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                    评测执行层                                │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐          │
│  │   Worker    │  │   Worker    │  │   Worker    │          │
│  │   Node 1    │  │   Node 2    │  │   Node N    │          │
│  │ (Kubernetes)│  │ (Kubernetes)│  │ (Kubernetes)│          │
│  └─────────────┘  └─────────────┘  └─────────────┘          │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 技术栈选择

#### 3.2.1 后端服务

- **语言**: Go (主要), Rust (评测), Python (ML/分析)
- **框架**: Gin / Echo (Go), FastAPI (Python)
- **RPC**: gRPC + Protocol Buffers
- **API**: REST + GraphQL

#### 3.2.2 前端

- **框架**: React 18 + TypeScript
- **状态管理**: Zustand / Redux Toolkit
- **UI组件**: Ant Design / Chakra UI
- **代码编辑**: Monaco Editor (VS Code同款)

#### 3.2.3 数据库

- **主库**: PostgreSQL 14+
- **缓存**: Redis Cluster
- **搜索**: Elasticsearch
- **时序**: ClickHouse

#### 3.2.4 基础设施

- **容器**: Docker + Kubernetes
- **CI/CD**: GitHub Actions / GitLab CI
- **监控**: Prometheus + Grafana
- **日志**: ELK Stack / Loki

### 3.3 评测系统设计

#### 3.3.1 架构

```
Judge Service Architecture:

┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   API       │────►│   Queue     │────►│   Worker    │
│  Gateway    │     │  (Redis/    │     │   Pool      │
│             │     │ RabbitMQ)   │     │             │
└─────────────┘     └─────────────┘     └──────┬──────┘
       ▲                                       │
       │                                       ▼
       │                              ┌─────────────┐
       │                              │   Sandbox   │
       │                              │  (Container) │
       │                              └──────┬──────┘
       │                                     │
       │                              ┌──────┴──────┐
       │                              │  Test Runner │
       │                              │  (Compiler/  │
       │                              │  Executor)   │
       │                              └─────────────┘
       │
       └─────────────────────────────────────────────┘
```

#### 3.3.2 安全沙箱实现

```go
// 评测沙箱配置
type SandboxConfig struct {
    // 资源限制
    MaxCPU    time.Duration  // CPU时间限制
    MaxMemory int64          // 内存限制 (bytes)
    MaxOutput int64          // 输出限制

    // 文件系统
    ReadOnlyDirs  []string   // 只读目录
    WritableDirs  []string   // 可写目录

    // 网络
    AllowNetwork  bool       // 是否允许网络

    // 系统调用
    SeccompProfile string    // seccomp配置
}

// 使用Kata Containers实现强隔离
// 或使用runsc (gVisor) 作为轻量替代
```

---

## 4. 数据库设计

### 4.1 核心表结构

#### 4.1.1 用户相关

```sql
-- 用户表
CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    avatar_url VARCHAR(500),
    status VARCHAR(20) DEFAULT 'active',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 用户学习档案
CREATE TABLE user_profiles (
    user_id BIGINT PRIMARY KEY REFERENCES users(id),
    level INT DEFAULT 1,
    experience INT DEFAULT 0,
    preferred_language VARCHAR(20),
    learning_streak INT DEFAULT 0,
    last_active_at TIMESTAMP
);
```

#### 4.1.2 题目相关

```sql
-- 题目表
CREATE TABLE problems (
    id BIGSERIAL PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    slug VARCHAR(100) UNIQUE NOT NULL,
    description TEXT NOT NULL,
    difficulty SMALLINT NOT NULL,  -- 1-5
    type VARCHAR(50) NOT NULL,     -- code, choice, fill
    category_id BIGINT REFERENCES categories(id),
    time_limit_ms INT DEFAULT 1000,
    memory_limit_mb INT DEFAULT 256,
    author_id BIGINT REFERENCES users(id),
    is_public BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 测试用例表
CREATE TABLE test_cases (
    id BIGSERIAL PRIMARY KEY,
    problem_id BIGINT NOT NULL REFERENCES problems(id),
    input TEXT,
    expected_output TEXT NOT NULL,
    is_sample BOOLEAN DEFAULT false,
    weight INT DEFAULT 1,
    sort_order INT DEFAULT 0
);

-- 分类表
CREATE TABLE categories (
    id BIGSERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    parent_id BIGINT REFERENCES categories(id),
    sort_order INT DEFAULT 0
);
```

#### 4.1.3 提交记录

```sql
-- 提交表
CREATE TABLE submissions (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id),
    problem_id BIGINT NOT NULL REFERENCES problems(id),
    code TEXT NOT NULL,
    language VARCHAR(50) NOT NULL,
    status VARCHAR(20) NOT NULL,  -- pending, judging, accepted, wrong_answer...
    runtime_ms INT,
    memory_kb INT,
    score INT,
    judged_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 提交详情 (测试结果)
CREATE TABLE submission_results (
    id BIGSERIAL PRIMARY KEY,
    submission_id BIGINT NOT NULL REFERENCES submissions(id),
    test_case_id BIGINT REFERENCES test_cases(id),
    status VARCHAR(20) NOT NULL,
    runtime_ms INT,
    memory_kb INT,
    actual_output TEXT,
    error_message TEXT
);
```

### 4.2 Redis 缓存策略

```
缓存键设计:

# 用户会话
session:{token} -> user_id, expire 7d

# 题目列表缓存
problems:list:{filter}:{page} -> JSON, expire 1h

# 题目详情缓存
problem:{problem_id} -> JSON, expire 24h

# 提交结果缓存
submission:{submission_id} -> JSON, expire 1h

# 排行榜
leaderboard:global -> Sorted Set, expire 1h
leaderboard:weekly -> Sorted Set, expire 7d

# 计数器
user:{user_id}:submissions_count -> Integer
problem:{problem_id}:accepted_count -> Integer
```

---

## 5. API 设计

### 5.1 REST API

#### 5.1.1 题目接口

```http
GET    /api/v1/problems              # 获取题目列表
GET    /api/v1/problems/{slug}       # 获取题目详情
POST   /api/v1/problems              # 创建题目 (管理员)
PUT    /api/v1/problems/{id}         # 更新题目 (管理员)
DELETE /api/v1/problems/{id}         # 删除题目 (管理员)
```

#### 5.1.2 提交接口

```http
POST   /api/v1/submissions            # 提交代码
GET    /api/v1/submissions/{id}       # 获取提交详情
GET    /api/v1/submissions            # 获取提交列表
GET    /api/v1/submissions/{id}/results  # 获取详细结果
```

### 5.2 WebSocket API

用于实时评测状态推送：

```javascript
// 连接
ws://api.example.com/ws/submissions

// 订阅提交状态
{
    "action": "subscribe",
    "submission_id": "12345"
}

// 接收状态更新
{
    "type": "status_update",
    "submission_id": "12345",
    "status": "judging",
    "progress": 50
}
```

---

## 6. 非功能需求

### 6.1 性能要求

| 指标 | 目标 | 说明 |
|-----|------|------|
| 页面加载 | < 2s | 首屏渲染 |
| 代码提交响应 | < 100ms | 接收确认 |
| 评测结果返回 | < 5s | 简单题目 |
| 并发评测 | > 100/s | 峰值 |
| 数据库查询 | < 50ms | P99 |

### 6.2 可用性要求

- 服务可用性: 99.9% SLA
- 数据备份: 每日全量 + 实时增量
- 灾难恢复: RTO < 1h, RPO < 5min

### 6.3 安全要求

- 代码执行沙箱化
- SQL注入防护
- XSS防护
- CSRF防护
- 敏感数据加密
- 审计日志

---

## 7. 项目实施计划

### 7.1 开发阶段

```
Phase 1 (MVP): 2个月
├── 用户系统
├── 基础题目管理
├── 代码评测 (GCC only)
├── 基础Web界面
└── 核心学习路径

Phase 2: 2个月
├── 多编译器支持
├── 社区功能
├── 高级学习功能
├── 移动适配
└── 性能优化

Phase 3: 2个月
├── 高级评测功能
├── 内容贡献系统
├── 数据分析
├── API开放
└── 企业功能
```

### 7.2 技术债务管理

- 持续集成/部署
- 代码审查制度
- 自动化测试覆盖 > 80%
- 定期安全审计

---

## 8. 附录

### 8.1 术语表

| 术语 | 说明 |
|-----|------|
| AC | Accepted，评测通过 |
| WA | Wrong Answer，答案错误 |
| TLE | Time Limit Exceeded，超时 |
| MLE | Memory Limit Exceeded，超内存 |
| RE | Runtime Error，运行时错误 |
| CE | Compile Error，编译错误 |

### 8.2 参考平台

- LeetCode
- HackerRank
- Codeforces
- PTA (浙江大学)
- 洛谷


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
