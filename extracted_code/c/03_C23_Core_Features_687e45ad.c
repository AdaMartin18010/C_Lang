/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 365
 * Language: c
 * Block ID: 687e45ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 将二进制文件嵌入源代码
const uint8_t firmware_image[] = {
#embed "firmware.bin"
};

// 等同于（但更高效）:
// const uint8_t firmware_image[] = { 0x7F, 0x45, 0x4C, ... };

// 限制嵌入大小
const uint8_t header[512] = {
#embed <512 "large_file.bin"
};

// 与字符串结合
const char shader_code[] =
#embed "shader.glsl" suffix(, '\0')
;
