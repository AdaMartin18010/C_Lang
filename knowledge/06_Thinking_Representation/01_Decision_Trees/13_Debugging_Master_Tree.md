# 调试技术主决策树

> **定位**: 调试决策树 | **用途**: 问题诊断与调试方法选择 | **更新**: 2026-03-18

---

## 1. 问题分类与诊断入口

```
程序出现问题?
│
├─▶ 程序崩溃?
│   │
│   ├─▶ 段错误 (Segmentation Fault / SIGSEGV)
│   │   ├─▶ 访问NULL指针?
│   │   │   ├─▶ 是 ──▶ 检查指针初始化
│   │   │   │           ├─▶ 未初始化的局部变量?
│   │   │   │           ├─▶ malloc返回未检查?
│   │   │   │           └─▶ 使用已free的指针?
│   │   │   │
│   │   │   └─▶ 否 ──▶ 越界访问?
│   │   │               ├─▶ 数组越界 ──▶ 检查索引范围
│   │   │               ├─▶ 栈溢出 ──▶ 减少局部数组大小
│   │   │               └─▶ 堆溢出 ──▶ 使用AddressSanitizer
│   │   │
│   │   └─▶ 使用GDB定位
│   │       ├─▶ gdb ./program
│   │       ├─▶ run (重现崩溃)
│   │       ├─▶ bt (查看调用栈)
│   │       └─▶ print 变量 (检查状态)
│   │
│   ├─▶ 总线错误 (Bus Error / SIGBUS)
│   │   ├─▶ 对齐问题 ──▶ 检查结构体对齐
│   │   └─▶ 访问不存在的物理地址
│   │
│   ├─▶ 非法指令 (Illegal Instruction / SIGILL)
│   │   ├─▶ 执行数据段 ──▶ NX保护触发
│   │   └─▶ 损坏的函数指针
│   │
│   └─▶ 浮点异常 (SIGFPE)
│       ├─▶ 除零错误 ──▶ 检查除数
│       ├─▶ 溢出 ──▶ 检查浮点运算范围
│       └─▶ NaN传播 ──▶ 检查输入有效性
│
├─▶ 程序挂起/无响应?
│   │
│   ├─▶ 死锁?
│   │   ├─▶ 症状: 所有线程等待
│   │   ├─▶ 检测: pstack / gdb thread apply all bt
│   │   └─▶ 解决: 统一锁顺序, 使用trylock
│   │
│   ├─▶ 无限循环?
│   │   ├─▶ 条件判断错误 ──▶ 检查循环条件
│   │   ├─▶ 缺少递增 ──▶ 检查循环变量
│   │   └─▶ 检测: GDB打断, 检查PC位置
│   │
│   └─▶ IO阻塞?
│       ├─▶ 阻塞IO ──▶ 改为非阻塞/超时
│       └─▶ 网络等待 ──▶ 检查连接状态
│
├─▶ 结果错误?
│   │
│   ├─▶ 逻辑错误?
│   │   ├─▶ 条件判断错误 ──▶ 单步调试
│   │   ├─▶ 算法实现错误 ──▶ 对比伪代码
│   │   └─▶ 边界条件处理 ──▶ 检查边界值
│   │
│   ├─▶ 数据损坏?
│   │   ├─▶ 内存越界 ──▶ AddressSanitizer
│   │   ├─▶ 类型双关 ──▶ 检查union使用
│   │   └─▶ 并发修改 ──▶ ThreadSanitizer
│   │
│   └─▶ 精度问题?
│       ├─▶ 整数溢出 ──▶ 使用更大类型
│       ├─▶ 浮点精度 ──▶ 使用double
│       └─▶ 舍入误差 ──▶ Kahan求和算法
│
└─▶ 性能问题?
    ├─▶ 使用perf识别热点
    ├─▶ 使用Valgrind检查缓存
    └─▶ 参考性能优化决策树
```

---

## 2. 调试工具选择矩阵

| 问题类型 | 首选工具 | 备用工具 | 使用方法 |
|:---------|:---------|:---------|:---------|
| 崩溃定位 | GDB | lldb | `gdb ./prog core`, `bt` |
| 内存错误 | AddressSanitizer | Valgrind Memcheck | `-fsanitize=address` |
| 内存泄漏 | Valgrind Memcheck | LeakSanitizer | `valgrind --leak-check=full` |
| 竞态条件 | ThreadSanitizer | Helgrind | `-fsanitize=thread` |
| 未定义行为 | UBSan | 代码审查 | `-fsanitize=undefined` |
| 性能热点 | perf | gprof | `perf record -g ./prog` |
| 系统调用 | strace | dtruss | `strace -e trace=file ./prog` |
| 堆分析 | Heaptrack | Massif | `heaptrack ./prog` |

---

## 3. GDB调试速查

```
GDB常用命令
────────────────────────────────────────────────────────────────

启动与运行:
├── gdb ./program           # 启动调试
├── gdb ./program core      # 分析core dump
├── run [args]              # 运行程序
├── start                   # 停在main开始
├── continue (c)            # 继续运行
├── quit (q)                # 退出GDB

断点管理:
├── break main              # 在main设断点
├── break file.c:123        # 在指定行设断点
├── break func if x>10      # 条件断点
├── info breakpoints        # 查看断点
├── delete 1                # 删除断点1
├── disable 1               # 禁用断点
└── enable 1                # 启用断点

执行控制:
├── step (s)                # 单步进入
├── next (n)                # 单步跳过
├── finish                  # 运行到函数返回
├── until                   # 运行到下一行
├── return [value]          # 强制返回

栈帧操作:
├── backtrace (bt)          # 查看调用栈
├── frame 2                 # 切换到帧2
├── up / down               # 上下移动帧
├── info locals             # 查看局部变量
└── info args               # 查看参数

变量检查:
├── print var               # 打印变量
├── print/x var             # 十六进制打印
├── print *ptr@10           # 打印数组前10个
├── display var             # 每次停自动显示
├── undisplay               # 取消自动显示
├── x/10x ptr               # 检查内存
└── info registers          # 查看寄存器

线程调试:
├── info threads            # 查看所有线程
├── thread 2                # 切换到线程2
├── thread apply all bt     # 所有线程backtrace
└── set scheduler-locking on # 锁定调度

高级功能:
├── watch var               # 监视变量变化
├── rwatch var              # 读监视
├── awatch var              # 读写监视
├── catch throw             # 捕获异常
├── catch syscall           # 捕获系统调用
├── reverse-step            # 反向执行(需要录制)
└── record                  # 开始录制执行
```

---

## 4. 常见错误模式诊断

```
错误: 程序随机崩溃
│
├─▶ 可能是内存损坏
│   ├─▶ 工具: AddressSanitizer
│   ├─▶ 编译: -fsanitize=address -g
│   └─▶ 运行: 会报告越界位置
│
├─▶ 可能是Use-After-Free
│   ├─▶ 工具: AddressSanitizer
│   ├─▶ 症状: 崩溃在free后的内存地址
│   └─▶ 解决: ptr = NULL after free
│
└─▶ 可能是栈溢出
    ├─▶ 工具: GDB + bt
    ├─▶ 症状: 崩溃在栈帧深处
    └─▶ 解决: 减少局部数组, 使用堆分配


错误: 多线程结果不一致
│
├─▶ 可能是竞态条件
│   ├─▶ 工具: ThreadSanitizer
│   ├─▶ 编译: -fsanitize=thread
│   └─▶ 报告: 会显示数据竞争位置
│
├─▶ 可能是缺少同步
│   ├─▶ 检查: 共享变量访问是否加锁
│   └─▶ 解决: mutex / atomic
│
└─▶ 可能是死锁
    ├─▶ 工具: GDB thread apply all bt
    ├─▶ 症状: 所有线程等待锁
    └─▶ 解决: 统一锁顺序


错误: 内存持续增长
│
├─▶ 内存泄漏检测
│   ├─▶ 工具: Valgrind --leak-check=full
│   ├─▶ 报告: 显示泄漏位置和大小
│   └─▶ 解决: 确保每个malloc有free
│
└─▶ 内存碎片
    ├─▶ 工具: Massif
    └─▶ 解决: 使用内存池
```

---

## 5. 调试策略选择

```
调试策略决策
│
├─▶ 可重现的问题?
│   ├─▶ 是 ──▶ 交互式调试
│   │           ├─▶ GDB单步
│   │           └─▶ 设置断点观察
│   │
│   └─▶ 否 ──▶ 事后分析
│               ├─▶ 生成core dump
│               ├─▶ 日志分析
│               └─▶ 插入诊断代码
│
├─▶ 生产环境问题?
│   ├─▶ 是 ──▶ 非侵入式调试
│   │           ├─▶ 日志记录
│   │           ├─▶ 远程attach
│   │           └─▶ 采样分析 (perf)
│   │
│   └─▶ 否 ──▶ 侵入式调试
│               ├─▶ 断点调试
│               ├─▶ 插桩 (sanitizers)
│               └─▶ 修改重编译
│
└─▶ 性能问题?
    ├─▶ 采样分析 (perf)
    ├─▶ 火焰图可视化
    └─▶ 针对性优化验证
```

---

**返回**: [决策树总览](./README.md) | [思维表征总览](../README.md) | [调试技术](../../01_Core_Knowledge_System/05_Engineering_Layer/02_Debug_Techniques.md)


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
