/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_C89_Library.md
 * Line: 79
 * Language: c
 * Block ID: 432b03a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ strcpy不安全：可能缓冲区溢出
char dest[10];
strcpy(dest, "this is a very long string");  // 溢出！

// ✅ strncpy的问题：不保证null终止
strncpy(dest, src, sizeof(dest));
dest[sizeof(dest) - 1] = '\0';  // 手动确保终止

// ✅ C11边界检查接口（可选支持）
#ifdef __STDC_LIB_EXT1__
    #define __STDC_WANT_LIB_EXT1__ 1
    #include <string.h>
    errno_t strcpy_s(char *dest, rsize_t destsz, const char *src);
#endif

// ✅ 最佳实践：自己实现安全版本
char *safe_strcpy(char *dest, size_t dest_size, const char *src) {
    if (!dest || !src || dest_size == 0) return NULL;

    size_t i;
    for (i = 0; i < dest_size - 1 && src[i]; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest;
}

// ✅ 更安全的动态分配版本
char *str_dup(const char *src) {
    if (!src) return NULL;
    size_t len = strlen(src) + 1;
    char *dest = malloc(len);
    if (dest) memcpy(dest, src, len);
    return dest;
}
