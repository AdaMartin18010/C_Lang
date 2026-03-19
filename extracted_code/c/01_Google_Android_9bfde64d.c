/*
 * Auto-generated from: 15_Memory_Safe_Languages\03_Industry_Cases\01_Google_Android.md
 * Line: 353
 * Language: c
 * Block ID: 9bfde64d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始C实现 - 手动边界检查
int parse_dns_name(const uint8_t* data, size_t len,
                   char* out, size_t out_len) {
    size_t pos = 0;
    size_t out_pos = 0;

    while (pos < len) {
        uint8_t label_len = data[pos];
        if (label_len == 0) break;

        // 风险: 可能忘记检查边界
        if (pos + 1 + label_len > len) return -1;
        if (out_pos + label_len + 1 > out_len) return -1; // 容易遗漏

        memcpy(out + out_pos, data + pos + 1, label_len);
        out_pos += label_len;
        out[out_pos++] = '.';
        pos += 1 + label_len;
    }

    if (out_pos > 0) out[out_pos - 1] = '\0';
    return 0;
}
