/*
 * Auto-generated from: 00_VERSION_TRACKING\C23_to_C2y_Roadmap.md
 * Line: 84
 * Language: c
 * Block ID: 1afca7c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23标准属性 (取代GCC __attribute__和MSVC __declspec)

// [[nodiscard]]: 忽略返回值警告
[[nodiscard]] int allocate_resource(void);
void test(void) {
    allocate_resource();  // 警告: 忽略返回值
}

// [[maybe_unused]]: 抑制未使用警告
void func([[maybe_unused]] int x) {
    // x可能在某些配置下不使用
}

// [[deprecated]]: 标记废弃接口
[[deprecated("Use new_function() instead")]]
void old_function(void);

// [[fallthrough]]: 显式case穿透
switch (x) {
    case 1:
        do_something();
        [[fallthrough]];  // 故意穿透
    case 2:
        do_more();
        break;
}

// [[noreturn]]: 标记不返回函数
[[noreturn]] void fatal_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

// C23新增属性
// [[unsequenced]]: 函数无副作用，调用顺序无关
[[unsequenced]] int pure_add(int a, int b) {
    return a + b;
}

// [[reproducible]]: 函数可重现(可能有缓存)
[[reproducible]] double cached_sin(double x);
