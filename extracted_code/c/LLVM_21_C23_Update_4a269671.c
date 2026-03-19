/*
 * Auto-generated from: 00_VERSION_TRACKING\LLVM_21_C23_Update.md
 * Line: 209
 * Language: c
 * Block ID: 4a269671
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 旧
#define max(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})

// 新 (更简洁)
#define max(a, b) ({ \
    typeof(a) _a = (a); \
    typeof(b) _b = (b); \
    _a > _b ? _a : _b; \
})
