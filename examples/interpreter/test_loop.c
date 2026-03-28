/**
 * C4测试程序: 循环计算
 * 
 * 计算: 1 + 2 + 3 + ... + 100
 */

int main() {
    int sum;
    int i;
    
    sum = 0;
    i = 1;
    
    while (i <= 100) {
        sum = sum + i;
        i = i + 1;
    }
    
    printf("Sum of 1 to 100 = %d\n", sum);
    return 0;
}
