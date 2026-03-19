/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\02_JIT_Physical_Constraints.md
 * Line: 244
 * Language: c
 * Block ID: 31784356
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * JIT编译面临独特的指令缓存挑战：
 *
 * 1. 自修改代码（SMC）检测
 *    - 写入内存后立即执行
 *    - 需要显式缓存同步
 *
 * 2. 多核一致性
 *    - 一个核心生成代码
 *    - 其他核心执行代码
 *    - 需要跨核缓存同步
 */

// 缓存同步原语
#if defined(__x86_64__)
    // x86-64：使用clflush或序列化指令
    #define CACHE_LINE_SIZE 64

    static inline void clflush(void *addr) {
        __asm__ volatile("clflush %0" :: "m"(*(char*)addr));
    }

    static inline void mfence(void) {
        __asm__ volatile("mfence" ::: "memory");
    }

#elif defined(__aarch64__)
    // ARM64：使用__builtin___clear_cache
    #define CACHE_LINE_SIZE 64

    static inline void clean_dcache(void *addr) {
        __asm__ volatile("dc cvau, %0" :: "r"(addr));
    }

    static inline void invalidate_icache(void *addr) {
        __asm__ volatile("ic ivau, %0" :: "r"(addr));
    }
#endif

// 完整的缓存同步（单核）
void jit_sync_icache_single(void *start, size_t size) {
#if defined(__x86_64__)
    // x86-64：通常不需要显式同步（保证一致性）
    // 但可能需要序列化
    mfence();

#elif defined(__aarch64__)
    // ARM64：需要显式缓存维护
    uintptr_t addr = (uintptr_t)start;
    uintptr_t end = addr + size;

    // 清除数据缓存到一致点
    for (; addr < end; addr += CACHE_LINE_SIZE) {
        clean_dcache((void*)addr);
    }

    __asm__ volatile("dsb ish" ::: "memory");

    // 使指令缓存无效
    addr = (uintptr_t)start;
    for (; addr < end; addr += CACHE_LINE_SIZE) {
        invalidate_icache((void*)addr);
    }

    __asm__ volatile("dsb ish" ::: "memory");
    __asm__ volatile("isb" ::: "memory");

#else
    // 通用方法
    __builtin___clear_cache(start, (char*)start + size);
#endif
}

// 多核缓存同步
void jit_sync_icache_multicore(void *start, size_t size) {
    // 方法1：使用系统调用（Linux）
    // 触发所有CPU的缓存同步

    // 方法2：使用内存屏障和信号
    // 生成代码后，发送信号给所有可能执行此代码的CPU

    // 方法3：使用线程同步
    // 确保所有线程看到最新代码

    // 实际实现（简化）
    jit_sync_icache_single(start, size);

    // 内存屏障确保顺序
    __sync_synchronize();
}
