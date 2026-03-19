/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 301
 * Language: c
 * Block ID: c4860fa0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 配置常量 */
    constexpr struct Config {
        int max_connections;
        int timeout_ms;
        int retry_count;
    } DEFAULT_CONFIG = {
        .max_connections = 100,
        .timeout_ms = 5000,
        .retry_count = 3
    };

    /* ✅ 推荐 [待官方确认] - CRC查找表生成 */
    constexpr uint32_t crc32_table[256] = {
        /* 编译时计算的CRC表 */
        0x00000000, 0x77073096, /* ... */
    };

    /* ✅ 推荐 [待官方确认] - 编译时字符串长度检查 */
    constexpr int check_id_length(const char *id) {
        int len = 0;
        while (id[len] != '\0' && len < 100) {
            len++;
        }
        return len >= 5 && len <= 20;
    }

    /* static_assert(check_id_length("test"), "ID too short"); */

#else
    /* C11/C17兼容 */
    #define MAX_CONNECTIONS 100
    #define TIMEOUT_MS 5000
    #define RETRY_COUNT 3
#endif
