/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 223
 * Language: c
 * Block ID: 2a6c7534
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分层错误码系统
typedef enum {
    // 通用错误 (0-99)
    ERR_OK = 0,
    ERR_INVALID_PARAM = -1,
    ERR_OUT_OF_MEMORY = -2,
    ERR_NOT_IMPLEMENTED = -3,
    ERR_INTERNAL = -4,

    // IO 错误 (100-199)
    ERR_IO_BASE = -100,
    ERR_FILE_NOT_FOUND = -101,
    ERR_PERMISSION_DENIED = -102,
    ERR_DISK_FULL = -103,

    // 网络错误 (200-299)
    ERR_NET_BASE = -200,
    ERR_CONNECTION_REFUSED = -201,
    ERR_TIMEOUT = -202,
} error_code_t;

// 错误信息映射
const char* error_string(error_code_t code) {
    switch (code) {
        case ERR_OK: return "Success";
        case ERR_INVALID_PARAM: return "Invalid parameter";
        case ERR_OUT_OF_MEMORY: return "Out of memory";
        case ERR_FILE_NOT_FOUND: return "File not found";
        // ...
        default: return "Unknown error";
    }
}
