/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 1082
 * Language: c
 * Block ID: ffe56b76
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23提供的typeof

// typeof: 精确类型
const int x = 42;
typeof(x) y;           // const int

// typeof_unqual: 去除cv限定
typeof_unqual(x) z;    // int (const被移除)

// 应用: 泛型宏
#define swap(a, b) do { \
    typeof(a) _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)
