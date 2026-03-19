/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 231
 * Language: c
 * Block ID: c41636bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型特性
#if defined(__GNUC__)
    #define IS_SIGNED_TYPE(T) __builtin_types_compatible_p(typeof(T), signed typeof(T))
    #define TYPE_MAX(T) __builtin_choose_expr(IS_SIGNED_TYPE(T), (T)~((T)1 << (sizeof(T)*8-1)), (T)-1)
#endif

// 分支预测提示
#if defined(__GNUC__)
    #define likely(x) __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

// 使用
if (unlikely(ptr == NULL)) {
    handle_error();
}

// 编译时常量检测
#if defined(__GNUC__)
    #define IS_CONSTANT(x) __builtin_constant_p(x)
#endif

// 安全溢出检测
#if defined(__GNUC__)
    int result;
    if (__builtin_add_overflow(a, b, &result)) {
        // 溢出处理
    }
#endif

// 内存操作内建
void *ptr = __builtin_alloca(1024);  // 栈分配
__builtin_memcpy(dst, src, n);       // 可能优化为内联
__builtin_memset(ptr, 0, n);
__builtin_memcmp(p1, p2, n);

// 帧地址
void *frame = __builtin_frame_address(0);
void *ret_addr = __builtin_return_address(0);

// _unreachable
void unreachable(void) {
    __builtin_unreachable();  // 告诉编译器此行不会执行
}
