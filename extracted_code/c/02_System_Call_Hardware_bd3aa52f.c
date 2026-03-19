/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 511
 * Language: c
 * Block ID: bd3aa52f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核系统调用安全检查机制
 * arch/x86/include/asm/uaccess.h
 */

// 1. 地址空间隔离检查
#define __user          __attribute__((noderef, address_space(1)))
#define __kernel        __attribute__((address_space(0)))

// 2. 用户指针验证
static inline bool user_access_begin(const void __user *ptr, size_t len)
{
    if (unlikely(!access_ok(ptr, len)))
        return false;

    // 启用SMAP (Supervisor Mode Access Prevention) 绕过
    // 或手动验证
    return true;
}

// 3. 安全内存复制
static __must_check unsigned long
_copy_from_user(void *to, const void __user *from, unsigned long n)
{
    unsigned long res = n;

    // 检查源地址是否在用户空间
    if (likely(access_ok(from, n))) {
        // 使用特殊的FS/GS段覆盖进行复制
        // 或临时启用SMAP绕过
        res = raw_copy_from_user(to, from, n);
    }

    // 如果有未复制的字节，清零
    if (unlikely(res))
        memset(to + (n - res), 0, res);

    return res;
}

#define copy_from_user(to, from, n) \
    (_copy_from_user((to), (from), (n)))

// 4. 安全字符串复制
long strncpy_from_user(char *dst, const char __user *src, long count)
{
    long res = -EFAULT;

    if (likely(access_ok(src, 1))) {
        // 逐字节复制并检查终止符
        // 同时检查页面边界
        res = do_strncpy_from_user(dst, src, count);
    }

    return res;
}
