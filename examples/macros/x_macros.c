/*
 * X-Macro 元编程示例
 * 编译: gcc -Wall -Wextra -o x_macros x_macros.c
 */
#include <stdio.h>

/* 定义颜色列表 */
#define COLOR_LIST \
    X(RED,    0xFF0000) \
    X(GREEN,  0x00FF00) \
    X(BLUE,   0x0000FF) \
    X(YELLOW, 0xFFFF00) \
    X(WHITE,  0xFFFFFF)

/* 生成枚举 */
#define X(name, value) COLOR_##name,
typedef enum {
    COLOR_LIST
    COLOR_COUNT
} Color;
#undef X

/* 生成名称字符串数组 */
#define X(name, value) #name,
static const char *color_names[] = {
    COLOR_LIST
};
#undef X

/* 生成值数组 */
#define X(name, value) value,
static const unsigned color_values[] = {
    COLOR_LIST
};
#undef X

/* 生成 switch case */
const char *color_name(Color c) {
    switch (c) {
        #define X(name, value) case COLOR_##name: return #name;
        COLOR_LIST
        #undef X
        default: return "UNKNOWN";
    }
}

int main(void) {
    printf("Color count: %d\n", COLOR_COUNT);
    for (int i = 0; i < COLOR_COUNT; i++) {
        printf("%s = 0x%06X (verify: %s)\n",
               color_names[i], color_values[i], color_name(i));
    }
    return 0;
}
