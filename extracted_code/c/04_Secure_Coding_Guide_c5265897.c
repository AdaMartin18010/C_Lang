/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 980
 * Language: c
 * Block ID: c5265897
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * UTF-8 安全处理函数
 */

// UTF-8序列有效性验证
bool utf8_is_valid_sequence(const uint8_t *bytes, size_t len) {
    if (len == 0) return false;
    
    uint8_t c0 = bytes[0];
    
    // 1字节: 0xxxxxxx
    if (c0 < 0x80) return len == 1;
    
    // 2字节: 110xxxxx 10xxxxxx
    if ((c0 & 0xE0) == 0xC0) {
        if (len != 2) return false;
        if ((bytes[1] & 0xC0) != 0x80) return false;
        // 检查过度编码 (C0-C1)
        if (c0 < 0xC2) return false;
        return true;
    }
    
    // 3字节: 1110xxxx 10xxxxxx 10xxxxxx
    if ((c0 & 0xF0) == 0xE0) {
        if (len != 3) return false;
        if ((bytes[1] & 0xC0) != 0x80 || (bytes[2] & 0xC0) != 0x80) return false;
        // 检查过度编码 (E0 80-9F)
        if (c0 == 0xE0 && bytes[1] < 0xA0) return false;
        // 检查代理对 (ED A0-BF)
        if (c0 == 0xED && bytes[1] >= 0xA0) return false;
        return true;
    }
    
    // 4字节: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((c0 & 0xF8) == 0xF0) {
        if (len != 4) return false;
        if ((bytes[1] & 0xC0) != 0x80 || 
            (bytes[2] & 0xC0) != 0x80 || 
            (bytes[3] & 0xC0) != 0x80) return false;
        // 检查范围 (F0 90-BF, F4 80-8F)
        if (c0 == 0xF0 && bytes[1] < 0x90) return false;
        if (c0 == 0xF4 && bytes[1] >= 0x90) return false;
        if (c0 > 0xF4) return false;
        return true;
    }
    
    return false;
}

// 安全的字符串长度计算（字符数）
size_t utf8_strlen_safe(const char *str, size_t max_bytes) {
    if (!str) return 0;
    
    size_t char_count = 0;
    size_t bytes = 0;
    
    while (str[bytes] && bytes < max_bytes) {
        int len = utf8_char_len(str[bytes]);
        if (len < 0) len = 1;  // 跳过非法字节
        
        bytes += len;
        char_count++;
    }
    
    return char_count;
}

// 规范化路径（防止UTF-8规范化攻击）
bool normalize_utf8_path(const char *input, char *output, size_t out_size) {
    if (!input || !output || out_size == 0) return false;
    
    size_t j = 0;
    for (size_t i = 0; input[i] && j < out_size - 1; ) {
        // 跳过控制字符
        if ((unsigned char)input[i] < 0x20) {
            i++;
            continue;
        }
        
        // 检查多字节序列有效性
        int char_len = utf8_char_len(input[i]);
        if (char_len > 0 && i + char_len <= strlen(input)) {
            if (utf8_is_valid_sequence((const uint8_t *)&input[i], char_len)) {
                // 复制有效UTF-8字符
                for (int k = 0; k < char_len && j < out_size - 1; k++) {
                    output[j++] = input[i++];
                }
                continue;
            }
        }
        
        // 非法字节：用替换字符或跳过
        i++;
    }
    
    output[j] = '\0';
    return true;
}
