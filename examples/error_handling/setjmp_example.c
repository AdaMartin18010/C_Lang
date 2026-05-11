/*
 * setjmp/longjmp 异常处理模拟示例
 * 编译: gcc -Wall -Wextra -o setjmp_example setjmp_example.c
 */
#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

typedef enum {
    EXC_NONE = 0,
    EXC_DIV_ZERO,
    EXC_NULL_PTR,
    EXC_OVERFLOW,
} Exception;

static Exception current_exception = EXC_NONE;

#define TRY do { current_exception = setjmp(env); if (current_exception == EXC_NONE) {
#define CATCH(e) } else if (current_exception == e) { current_exception = EXC_NONE;
#define FINALLY } else { current_exception = EXC_NONE;
#define END_TRY } } while (0)

#define THROW(e) do { current_exception = e; longjmp(env, e); } while (0)

int safe_divide(int a, int b) {
    if (b == 0) THROW(EXC_DIV_ZERO);
    return a / b;
}

void process_data(int *data, int divisor) {
    if (!data) THROW(EXC_NULL_PTR);
    int result = safe_divide(*data, divisor);
    printf("Result: %d\n", result);
}

int main(void) {
    int value = 100;
    
    TRY {
        process_data(&value, 0);
    } CATCH(EXC_DIV_ZERO) {
        printf("Caught: Division by zero\n");
    } CATCH(EXC_NULL_PTR) {
        printf("Caught: Null pointer\n");
    } FINALLY {
        printf("Cleanup done\n");
    } END_TRY;
    
    TRY {
        process_data(NULL, 5);
    } CATCH(EXC_NULL_PTR) {
        printf("Caught: Null pointer access\n");
    } END_TRY;
    
    return 0;
}
