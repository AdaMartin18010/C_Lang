/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\README.md
 * Line: 259
 * Language: c
 * Block ID: e2f1fa9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// cppcheck 能检测的问题

// 1. 内存泄漏
detect_memory_leak() {
    char* ptr = malloc(100);
    // 忘记释放
}  // cppcheck: Memory leak: ptr

// 2. 缓冲区溢出
void buffer_overflow() {
    char buf[10];
    strcpy(buf, "this is too long");  // cppcheck: Buffer is accessed out of bounds
}

// 3. 无效迭代器
void invalid_iterator() {
    int arr[5];
    for (int i = 0; i <= 5; i++) {  // cppcheck: Array index out of bounds
        arr[i] = i;
    }
}

// 4. 除零错误
int divide_by_zero(int x) {
    return 100 / x;  // cppcheck: Division by zero
}

// 5. 未初始化变量
int uninitialized() {
    int x;
    return x;  // cppcheck: Uninitialized variable: x
}
