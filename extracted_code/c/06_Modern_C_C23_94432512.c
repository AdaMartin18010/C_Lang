/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 438
 * Language: c
 * Block ID: 94432512
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C99/C11: 空参数时会有问题
#define debug(fmt, ...) printf(fmt, __VA_ARGS__)
debug("hello");  // 错误：尾部逗号

// C23: 使用__VA_OPT__
#define debug(fmt, ...) \
    printf(fmt __VA_OPT__(,) __VA_ARGS__)

debug("hello");          // 展开: printf("hello")
debug("value: %d", 42);  // 展开: printf("value: %d", 42)

// 更复杂的用法
#define log(level, fmt, ...) \
    do { \
        if (level >= current_level) { \
            fprintf(stderr, "[%s] " fmt "\n", \
                    #level __VA_OPT__(,) __VA_ARGS__); \
        } \
    } while(0)
