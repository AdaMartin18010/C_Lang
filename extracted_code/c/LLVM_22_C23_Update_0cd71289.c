/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_22_C23_Update.md
 * Line: 1620
 * Language: c
 * Block ID: 0cd71289
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 步骤 1: 使用 __COUNTER__ 改进宏
// 之前：手动编号
#define COMPONENT_1 1
#define COMPONENT_2 2
#define COMPONENT_3 3

// 之后：自动生成
#define DECLARE_COMPONENT(name) \
    enum { COMPONENT_##name = __COUNTER__ }

DECLARE_COMPONENT(NETWORK);
DECLARE_COMPONENT(STORAGE);
DECLARE_COMPONENT(SECURITY);

// 步骤 2: 使用 Named Loops 简化控制流
// 之前：使用标志变量
int found = 0;
for (int i = 0; i < n && !found; i++) {
    for (int j = 0; j < m && !found; j++) {
        if (condition(i, j)) {
            found = 1;
            break;
        }
    }
}

// 之后：清晰的命名循环
outer: for (int i = 0; i < n; i++) {
    inner: for (int j = 0; j < m; j++) {
        if (condition(i, j)) {
            break outer;
        }
    }
}

// 步骤 3: 利用 static_assert 改进编译时检查
// 之前：运行时检查
void init(void) {
    if (sizeof(buffer) < REQUIRED_SIZE) {
        fprintf(stderr, "Buffer too small\n");
        abort();
    }
}

// 之后：编译时检查
static_assert(sizeof(buffer) >= REQUIRED_SIZE,
              "Buffer must be at least REQUIRED_SIZE bytes");
