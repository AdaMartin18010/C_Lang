/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 618
 * Language: c
 * Block ID: 9f666d39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 死代码：永远不会执行的代码
int func(int x) {
    return x * 2;
    printf("This never prints\n");  // 死代码！
}

// ❌ 不可达代码：条件永远为假
int func2(int x) {
    if (0) {
        // 永远不会执行
        do_something();
    }
    return x;
}

// ❌ 条件导致的死代码
int func3(int x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    }
    // 如果x总是非零，下面的代码是死代码
    return 0;
}

// ✅ 使用编译器警告检测
// gcc -Wunreachable-code
