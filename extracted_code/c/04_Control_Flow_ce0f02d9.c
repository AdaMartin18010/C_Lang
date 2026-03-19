/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 729
 * Language: c
 * Block ID: ce0f02d9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 编译错误
switch (x) {
    case 1:
        int y = 10;  // 错误：跳过初始化
        break;
    case 2:
        break;
}

// ✅ 加花括号创建作用域
switch (x) {
    case 1: {
        int y = 10;
        use(y);
        break;
    }
    case 2:
        break;
}
