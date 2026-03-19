/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1563
 * Language: c
 * Block ID: 3b234c1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 优化的系统调用入口 (使用per-CPU缓存)
 */

// per-CPU数据
DEFINE_PER_CPU(struct syscall_stack_cache, syscall_cache);

// 快速路径入口
static __always_inline void syscall_enter_fast(struct pt_regs *regs)
{
    struct syscall_stack_cache *cache;

    // 使用this_cpu读取缓存数据
    cache = this_cpu_ptr(&syscall_cache);

    // 快速检查: 是否可以直接处理
    if (likely(cache->fast_path_enabled)) {
        // 使用预分配的栈帧
        regs->sp = cache->stack_top;

        // 直接调用处理函数，跳过通用entry代码
        cache->handler(regs);

        // 快速返回
        syscall_exit_fast(regs);
        return;
    }

    // 慢路径: 完整处理
    syscall_enter_slow(regs);
}

// 系统调用追踪优化
#ifdef CONFIG_SYSCALL_TRACE
static inline void trace_syscall_enter(struct pt_regs *regs)
{
    // 只在需要时才进行昂贵的追踪操作
    if (static_branch_unlikely(&syscall_trace_enabled)) {
        // 详细的ptrace/seccomp处理
        __trace_syscall_enter(regs);
    }
}
#else
#define trace_syscall_enter(regs) do { } while(0)
#endif

// 分支预测优化
static __always_inline long do_syscall_64_fast(struct pt_regs *regs)
{
    unsigned long nr = regs->orig_ax;

    // 编译器分支预测提示
    if (likely(nr < NR_syscalls)) {
        // 热路径: 直接调用
        return sys_call_table[nr](
            regs->di, regs->si, regs->dx,
            regs->r10, regs->r8, regs->r9
        );
    }

    // 冷路径: 无效系统调用
    return -ENOSYS;
}
