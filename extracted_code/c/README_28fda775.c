/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 395
 * Language: c
 * Block ID: 28fda775
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void demonstrate_assignment(void)
{
    int a = 10;

    /* ===== 复合赋值运算符 ===== */
    a += 5;   // a = a + 5;   -> 15
    a -= 3;   // a = a - 3;   -> 12
    a *= 2;   // a = a * 2;   -> 24
    a /= 4;   // a = a / 4;   -> 6
    a %= 4;   // a = a % 4;   -> 2

    a &= 0xFF;
    a |= 0x10;
    a ^= 0x01;
    a <<= 2;
    a >>= 1;

    /* ===== 自增自减运算符 ===== */
    int x = 5;
    int y;

    y = ++x;  // 前缀：先增后用，y = 6, x = 6
    x = 5;
    y = x++;  // 后缀：先用后增，y = 5, x = 6

    // 在表达式中谨慎使用
    int arr[] = {1, 2, 3, 4, 5};
    int i = 0;
    // 避免：未定义行为
    // arr[i++] = arr[i++];  // 错误！

    /* ===== 逗号运算符 ===== */
    int m, n;
    m = (n = 3, n + 2);  // n=3，然后 m=5

    for (int i = 0, j = 10; i < j; i++, j--) {
        // 逗号运算符在 for 循环中常见
    }

    /* ===== sizeof 运算符 ===== */
    printf("sizeof(int) = %zu\n", sizeof(int));
    printf("sizeof a    = %zu\n", sizeof a);    // 可省略括号
    printf("sizeof(1+2) = %zu\n", sizeof(1+2)); // 表达式不求值
}
