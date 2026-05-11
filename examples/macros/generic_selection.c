/*
 * _Generic 泛型选择示例（C11）
 * 编译: gcc -Wall -Wextra -std=c11 -o generic_selection generic_selection.c
 */
#include <stdio.h>
#include <math.h>

#define max(a, b) _Generic((a) + (b), \
    int: max_int, \
    long: max_long, \
    float: max_float, \
    double: max_double \
)(a, b)

static inline int max_int(int a, int b) { return a > b ? a : b; }
static inline long max_long(long a, long b) { return a > b ? a : b; }
static inline float max_float(float a, float b) { return a > b ? a : b; }
static inline double max_double(double a, double b) { return a > b ? a : b; }

/* 类型名称打印 */
#define type_name(x) _Generic((x), \
    int: "int", \
    long: "long", \
    float: "float", \
    double: "double", \
    char *: "char*", \
    const char *: "const char*", \
    default: "unknown" \
)

/* 通用打印 */
#define print_val(x) _Generic((x), \
    int: printf("int: %d\n", x), \
    long: printf("long: %ld\n", x), \
    float: printf("float: %f\n", x), \
    double: printf("double: %f\n", x), \
    char *: printf("string: %s\n", x) \
)

int main(void) {
    int a = 5, b = 10;
    float f1 = 3.14f, f2 = 2.71f;
    double d1 = 1.5, d2 = 2.5;
    
    printf("max(%d, %d) = %d (type: %s)\n", a, b, max(a, b), type_name(max(a,b)));
    printf("max(%f, %f) = %f\n", f1, f2, max(f1, f2));
    printf("max(%f, %f) = %f\n", d1, d2, max(d1, d2));
    
    print_val(42);
    print_val(3.14);
    print_val("hello");
    
    return 0;
}
