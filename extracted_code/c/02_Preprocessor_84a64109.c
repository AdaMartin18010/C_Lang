/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\02_Preprocessor.md
 * Line: 176
 * Language: c
 * Block ID: 84a64109
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数据定义与使用的解耦

// 1. 定义颜色表（在colors.def中）
// COLOR(name, r, g, b)
COLOR(RED,    255, 0,   0  )
COLOR(GREEN,  0,   255, 0  )
COLOR(BLUE,   0,   0,   255)

// 2. 生成枚举
#define COLOR(name, r, g, b) COLOR_##name,
enum Color {
    #include "colors.def"
    COLOR_COUNT
};
#undef COLOR

// 3. 生成名称字符串
#define COLOR(name, r, g, b) #name,
const char *color_names[] = {
    #include "colors.def"
};
#undef COLOR

// 4. 生成RGB值数组
#define COLOR(name, r, g, b) {r, g, b},
const uint8_t color_values[][3] = {
    #include "colors.def"
};
#undef COLOR

// 使用
void print_color(enum Color c) {
    printf("%s: RGB(%d, %d, %d)\n",
           color_names[c],
           color_values[c][0],
           color_values[c][1],
           color_values[c][2]);
}
