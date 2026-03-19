/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\README.md
 * Line: 429
 * Language: c
 * Block ID: 21a9d1e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>

// 安全字符串操作
errno_t safe_strcpy(char *dest, rsize_t destsz, const char *src)
{
    return strcpy_s(dest, destsz, src);
}

errno_t safe_strcat(char *dest, rsize_t destsz, const char *src)
{
    return strcat_s(dest, destsz, src);
}

errno_t safe_gets(char *str, rsize_t n)
{
    return gets_s(str, n);  // 安全版本的 gets
}
#endif

// 可移植的安全包装
#ifndef __STDC_LIB_EXT1__
    #include <string.h>
    #include <stdlib.h>

    typedef int errno_t;
    typedef size_t rsize_t;

    errno_t memcpy_s(void *dest, rsize_t destsz, const void *src, rsize_t count)
    {
        if (!dest || !src) return -1;
        if (destsz < count) return -1;
        memcpy(dest, src, count);
        return 0;
    }
#endif
