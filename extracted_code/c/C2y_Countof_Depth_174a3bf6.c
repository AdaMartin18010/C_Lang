/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 465
 * Language: c
 * Block ID: 174a3bf6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 清零数组
#define ARRAY_ZERO(arr) \
    memset((arr), 0, sizeof(arr))

// 复制数组（确保目标足够大）
#define ARRAY_COPY(dst, src) do { \
    static_assert(_Countof(dst) >= _Countof(src), "Destination too small"); \
    memcpy((dst), (src), sizeof(src)); \
} while(0)
