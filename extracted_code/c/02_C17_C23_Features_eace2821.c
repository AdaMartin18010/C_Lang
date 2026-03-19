/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\02_C17_C23_Features.md
 * Line: 216
 * Language: c
 * Block ID: eace2821
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: 在编译时嵌入二进制文件

// 嵌入图片作为字节数组
static const unsigned char icon_png[] = {
#embed "icon.png"
};

// 嵌入字体
static const unsigned char font_ttf[] = {
#embed "font.ttf"
};

// 限制大小
static const unsigned char small[] = {
#embed "large.bin" limit(1024)  // 只嵌入前1024字节
};

// 与字符串结合
static const char shader_src[] = {
#embed "shader.glsl"
    , '\0'  // 添加null终止
};
