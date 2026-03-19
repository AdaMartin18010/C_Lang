/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 2016
 * Language: c
 * Block ID: 0647b9a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误处理最佳实践

typedef enum {
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY = -1,
    ERR_INVALID_ARGUMENT = -2,
    ERR_BUFFER_TOO_SMALL = -3,
    // ...
} ErrorCode;

typedef struct {
    ErrorCode code;
    const char* message;
} Result;

// 返回结果的分配函数
Result allocate_buffer(Buffer** out_buf, int size) {
    if (!out_buf) {
        return (Result){ ERR_INVALID_ARGUMENT, "Output pointer is NULL" };
    }

    if (size <= 0) {
        return (Result){ ERR_INVALID_ARGUMENT, "Invalid size" };
    }

    Buffer* buf = buffer_create(size);
    if (!buf) {
        return (Result){ ERR_OUT_OF_MEMORY, "Failed to allocate buffer" };
    }

    *out_buf = buf;
    return (Result){ ERR_OK, "Success" };
}

// 使用示例
EMSCRIPTEN_KEEPALIVE
int safe_process_data(int size) {
    Buffer* buf = NULL;
    Result result = allocate_buffer(&buf, size);

    if (result.code != ERR_OK) {
        fprintf(stderr, "Error: %s\n", result.message);
        return result.code;
    }

    // 使用 buf...

    buffer_release(buf);
    return ERR_OK;
}
