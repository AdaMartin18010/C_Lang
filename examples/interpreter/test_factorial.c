/**
 * C4测试程序: 阶乘计算
 * 
 * 编译运行: c4 test_factorial.c
 */

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    int result;
    result = factorial(5);
    printf("factorial(5) = %d\n", result);
    return 0;
}
