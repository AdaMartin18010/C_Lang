/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 195
 * Language: c
 * Block ID: 1927ba3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 作用域规则
void scope_rules(void) {
    // 循环标签在声明后、循环结束前可见
    outer: for (int i = 0; i < 10; i++) {
        // outer在此可见
        inner: for (int j = 0; j < 10; j++) {
            // outer和inner都可见
            break outer;  // 合法
        }
        // inner在此不可见(循环已结束)
    }
    // outer在此不可见

    // 不能与goto标签同名(同一命名空间)
    // error: duplicate label 'outer'
    outer:  // goto标签
        goto outer;
    outer: for (;;);  // 错误! 标签冲突
}

// break vs continue 语义
void break_continue(void) {
    loop: for (int i = 0; i < 10; i++) {
        if (condition) {
            break loop;     // 完全退出loop循环
        }
        if (skip) {
            continue loop;  // 跳到loop的下一次迭代
        }
    }
}
