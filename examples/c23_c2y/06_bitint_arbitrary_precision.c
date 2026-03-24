/*
 * C23 _BitInt(N) 任意精度整数示例
 * 编译: gcc -std=c23 -o 06_bitint_arbitrary_precision 06_bitint_arbitrary_precision.c
 *       clang -std=c23 -o 06_bitint_arbitrary_precision 06_bitint_arbitrary_precision.c
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

// _BitInt(N) 可以创建任意宽度的整数类型
// N 可以是 1 到 BITINT_MAXWIDTH（实现定义，通常非常大）

// 128位整数
_BitInt(128) large_number = 0;

// 256位整数
_BitInt(256) very_large_number;

// 64位，与 int64_t 等价
_BitInt(64) exact_64bit;

// 33位整数（独特的宽度）
_BitInt(33) unique_width;

// 打印 _BitInt 的辅助函数（简化版）
// 注意：完整的打印实现需要大数运算
void print_bitint128(_BitInt(128) value) {
    // 简化为打印高低64位
    uint64_t high = (uint64_t)(value >> 64);
    uint64_t low = (uint64_t)value;
    
    if (high != 0) {
        printf("0x%016llX%016llX", (unsigned long long)high, (unsigned long long)low);
    } else {
        printf("0x%016llX", (unsigned long long)low);
    }
}

// 大数乘法示例
_BitInt(128) multiply_64to128(uint64_t a, uint64_t b) {
    return (_BitInt(128))a * (_BitInt(128))b;
}

// 金融计算示例：精确的货币计算
// 使用 96位整数表示大金额（避免浮点精度问题）
_BitInt(96) calculate_interest(_BitInt(96) principal, int rate, int periods) {
    // 复利计算：principal * (1 + rate/100)^periods
    _BitInt(96) result = principal;
    for (int i = 0; i < periods; i++) {
        result = result * (100 + rate) / 100;
    }
    return result;
}

// 密码学示例：大数模运算
_BitInt(256) modular_exponentiation(_BitInt(256) base, 
                                     _BitInt(256) exp, 
                                     _BitInt(256) mod) {
    _BitInt(256) result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    
    return result;
}

int main(void) {
    printf("C23 _BitInt Arbitrary Precision Demo\n\n");
    
    // 基本用法
    printf("Basic _BitInt usage:\n");
    _BitInt(128) a = 12345678901234567890ULL;
    _BitInt(128) b = 98765432109876543210ULL;
    _BitInt(128) sum = a + b;
    
    printf("a = "); print_bitint128(a); printf("\n");
    printf("b = "); print_bitint128(b); printf("\n");
    printf("sum = "); print_bitint128(sum); printf("\n");
    
    // 大数乘法
    printf("\nLarge number multiplication:\n");
    _BitInt(128) product = multiply_64to128(UINT64_MAX, UINT64_MAX);
    printf("UINT64_MAX * UINT64_MAX = ");
    print_bitint128(product);
    printf("\n");
    
    // 使用 sizeof
    printf("\nSizeof _BitInt types:\n");
    printf("sizeof(_BitInt(8)) = %zu\n", sizeof(_BitInt(8)));
    printf("sizeof(_BitInt(16)) = %zu\n", sizeof(_BitInt(16)));
    printf("sizeof(_BitInt(32)) = %zu\n", sizeof(_BitInt(32)));
    printf("sizeof(_BitInt(64)) = %zu\n", sizeof(_BitInt(64)));
    printf("sizeof(_BitInt(128)) = %zu\n", sizeof(_BitInt(128)));
    printf("sizeof(_BitInt(256)) = %zu\n", sizeof(_BitInt(256)));
    
    // 独特的位宽
    printf("\nUnique bit widths:\n");
    printf("sizeof(_BitInt(33)) = %zu (rounds up to 64 bits)\n", sizeof(_BitInt(33)));
    printf("sizeof(_BitInt(65)) = %zu (rounds up to 128 bits)\n", sizeof(_BitInt(65)));
    
    printf("\n_BitInt demo complete!\n");
    
    return 0;
}
