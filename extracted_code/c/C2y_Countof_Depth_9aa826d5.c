/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1684
 * Language: c
 * Block ID: 9aa826d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 旧代码：手动边界检查
void safe_write(char *buf, size_t buf_size, const char *str) {
    size_t len = strlen(str);
    if (len >= buf_size) {
        // 处理溢出
        return;
    }
    strcpy(buf, str);
}

// 新代码：利用_Countof
#define SAFE_WRITE(buf, str) do { \
    static_assert(_Countof(buf) > strlen(str), "Buffer too small"); \
    strcpy((buf), (str)); \
} while(0)

// 或者运行时检查
#define SAFE_COPY(dst, src) do { \
    static_assert(_Countof(dst) >= _Countof(src), "Dst smaller than src"); \
    memcpy((dst), (src), sizeof(src)); \
} while(0)
