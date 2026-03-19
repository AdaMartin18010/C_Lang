/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\03_Memory_Management.md
 * Line: 520
 * Language: c
 * Block ID: 3aacd5cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef DEBUG
    #include <assert.h>

    #define CHECK_NULL(p) \
        do { \
            if ((p) == NULL) { \
                fprintf(stderr, "NULL pointer at %s:%d\n", \
                        __FILE__, __LINE__); \
                abort(); \
            } \
        } while(0)

    #define CHECK_BOUNDS(arr, index, size) \
        assert((index) >= 0 && (index) < (size))
#else
    #define CHECK_NULL(p)
    #define CHECK_BOUNDS(arr, index, size)
#endif
