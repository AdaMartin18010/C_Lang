/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 121
 * Language: c
 * Block ID: d9700fa2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例程序
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int result = factorial(5);
    print(result);
    return 0;
}
