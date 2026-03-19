/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 1179
 * Language: c
 * Block ID: e8873ac9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 嵌入式C代码安全实践
 * 符合EU CRA要求的安全编码规范
 */

/* 1. 安全内存管理 */

/* 使用安全的内存分配 */
void* secure_malloc(size_t size) {
    void *ptr = calloc(1, size);  /* calloc初始化为0 */
    if (!ptr) {
        /* 记录分配失败 */
        log_security_event(EVENT_MEMORY_ALLOC_FAIL,
                          "Failed to allocate memory");
        return NULL;
    }
    return ptr;
}

/* 安全的内存释放 */
void secure_free(void **ptr, size_t size) {
    if (ptr && *ptr) {
        /* 清零内存 */
        memset(*ptr, 0, size);
        free(*ptr);
        *ptr = NULL;  /* 防止悬挂指针 */
    }
}

/* 2. 缓冲区溢出防护 */

/* 安全的字符串复制 */
int safe_strcpy(char *dst, size_t dst_size, const char *src) {
    if (!dst || !src || dst_size == 0) {
        return -EINVAL;
    }

    size_t i;
    for (i = 0; i < dst_size - 1 && src[i]; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';

    /* 检查是否截断 */
    if (src[i] != '\0') {
        log_security_event(EVENT_STRING_TRUNCATED, src);
        return -E2BIG;
    }

    return 0;
}

/* 安全的内存复制 */
int safe_memcpy(void *dst, size_t dst_size,
                const void *src, size_t src_size) {
    if (!dst || !src) {
        return -EINVAL;
    }

    if (src_size > dst_size) {
        log_security_event(EVENT_BUFFER_OVERFLOW_ATTEMPT, NULL);
        return -E2BIG;
    }

    memcpy(dst, src, src_size);
    return 0;
}

/* 3. 整数溢出检查 */

/* 安全的整数加法 */
int safe_add_u32(uint32_t *result, uint32_t a, uint32_t b) {
    if (a > UINT32_MAX - b) {
        return -EOVERFLOW;
    }
    *result = a + b;
    return 0;
}

/* 安全的整数乘法 */
int safe_mul_u32(uint32_t *result, uint32_t a, uint32_t b) {
    if (a != 0 && b > UINT32_MAX / a) {
        return -EOVERFLOW;
    }
    *result = a * b;
    return 0;
}

/* 4. 输入验证 */

/* 验证整数范围 */
int validate_int_range(int value, int min, int max) {
    if (value < min || value > max) {
        log_security_event(EVENT_INVALID_INPUT,
                          "Value out of range");
        return -ERANGE;
    }
    return 0;
}

/* 验证指针 */
int validate_pointer(const void *ptr, size_t expected_size) {
    if (!ptr) {
        return -EINVAL;
    }

    /* 检查指针是否在有效内存范围 */
    if (!is_valid_memory_range(ptr, expected_size)) {
        log_security_event(EVENT_INVALID_POINTER, NULL);
        return -EFAULT;
    }

    return 0;
}

/* 5. 错误处理 */

typedef enum {
    ERR_NONE = 0,
    ERR_INVALID_PARAM,
    ERR_MEMORY,
    ERR_CRYPTO,
    ERR_NETWORK,
    ERR_AUTH,
    ERR_PERMISSION,
} error_code_t;

/* 安全的错误处理 */
int process_command(const char *cmd) {
    error_code_t err = ERR_NONE;

    if (!cmd) {
        err = ERR_INVALID_PARAM;
        goto cleanup;
    }

    /* 验证命令格式 */
    if (!validate_command_format(cmd)) {
        err = ERR_INVALID_PARAM;
        log_security_event(EVENT_MALFORMED_COMMAND, cmd);
        goto cleanup;
    }

    /* 检查权限 */
    if (!has_permission(cmd)) {
        err = ERR_PERMISSION;
        log_security_event(EVENT_UNAUTHORIZED_COMMAND, cmd);
        goto cleanup;
    }

    /* 执行命令 */
    if (execute_command(cmd) != 0) {
        err = ERR_CRYPTO;  /* 或其他适当错误 */
        goto cleanup;
    }

cleanup:
    /* 统一清理 */
    if (err != ERR_NONE) {
        /* 记录错误但不泄露敏感信息 */
        log_error("Command processing failed: code=%d", err);
    }

    return err;
}

/* 6. 并发安全 */

/* 互斥锁包装 */
typedef struct {
    mutex_t lock;
    const char *name;
} secure_mutex_t;

int secure_lock(secure_mutex_t *m) {
    int ret = mutex_lock(&m->lock, TIMEOUT_MS);
    if (ret != 0) {
        log_security_event(EVENT_LOCK_TIMEOUT, m->name);
    }
    return ret;
}

void secure_unlock(secure_mutex_t *m) {
    mutex_unlock(&m->lock);
}

/* 使用RAII模式 */
#define SECURE_SCOPE_LOCK(mutex) \
    for (int _locked = secure_lock(mutex); \
         _locked == 0; \
         _locked = 1, secure_unlock(mutex))

/* 7. 随机数生成 */

/* 安全的随机数生成 */
int secure_random(uint8_t *buf, size_t len) {
    /* 使用硬件RNG（如果可用） */
    if (has_hardware_rng()) {
        return hw_rng_get(buf, len);
    }

    /* 使用软件CSPRNG */
    if (getrandom(buf, len, GRND_RANDOM) != (ssize_t)len) {
        log_security_event(EVENT_RANDOM_FAIL, NULL);
        return -1;
    }

    return 0;
}

/* 随机数质量检查 */
int check_random_quality(void) {
    uint8_t samples[1024];
    secure_random(samples, sizeof(samples));

    /* 简单熵检查 */
    double entropy = calculate_entropy(samples, sizeof(samples));
    if (entropy < 7.9) {  /* 8位/字节的95% */
        log_security_event(EVENT_LOW_ENTROPY, NULL);
        return -1;
    }

    return 0;
}
