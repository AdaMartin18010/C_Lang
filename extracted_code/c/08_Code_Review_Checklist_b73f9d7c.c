/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 3038
 * Language: c
 * Block ID: b73f9d7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file case_study_security.c
 * @brief 案例研究：文件解析器中的安全漏洞
 *
 * 背景：某嵌入式系统的配置文件解析器
 * 问题：在代码审查中发现多个高危安全漏洞
 */

/* ========== 原始代码（问题版本）========== */

/*
 * ❌ 问题列表：
 * 1. 缓冲区溢出：filename和line缓冲区无大小检查
 * 2. 整数溢出：line_num++可能溢出
 * 3. 资源泄漏：fp在某些路径未关闭
 * 4. 路径遍历：未验证文件路径
 * 5. 信息泄露：错误消息可能暴露敏感信息
 */
/*
int parse_config_old(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        char key[64], value[192];
        sscanf(line, "%s = %s", key, value);  // 缓冲区溢出！

        if (strcmp(key, "password") == 0) {
            g_config.password = strdup(value);  // 明文存储密码
        }
        // ...
    }
    // 如果前面return，这里不会执行
    fclose(fp);
    return 0;
}
*/

/* ========== 修复后代码 ========== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define MAX_CONFIG_LINE 1024
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256

typedef struct {
    char password[256];  /* 不使用动态分配 */
    /* 其他配置项 */
} config_t;

static config_t g_config;

/**
 * 安全的字符串拷贝，确保null终止
 */
static int safe_strncpy(char *dst, const char *src, size_t size)
{
    if (!dst || !src || size == 0) return -1;

    size_t i;
    for (i = 0; i < size - 1 && src[i]; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';

    return (src[i] == '\0') ? 0 : 1;  /* 0=成功, 1=截断 */
}

/**
 * 验证配置文件路径
 */
static int validate_config_path(const char *path)
{
    if (!path) return -1;

    /* 检查路径长度 */
    if (strlen(path) > PATH_MAX - 1) return -1;

    /* 检查路径遍历 */
    if (strstr(path, "..") != NULL) return -1;

    /* 只允许特定目录 */
    const char *allowed_prefix = "/etc/myapp/";
    if (strncmp(path, allowed_prefix, strlen(allowed_prefix)) != 0) {
        return -1;
    }

    return 0;
}

/**
 * 解析单行配置
 */
static int parse_config_line(const char *line,
                              char *key, size_t key_size,
                              char *value, size_t value_size)
{
    if (!line || !key || !value) return -1;

    /* 查找等号 */
    const char *eq = strchr(line, '=');
    if (!eq) return -1;

    /* 提取key */
    size_t key_len = eq - line;
    while (key_len > 0 && (line[key_len-1] == ' ' ||
                           line[key_len-1] == '\t')) {
        key_len--;
    }
    if (key_len >= key_size) return -1;

    memcpy(key, line, key_len);
    key[key_len] = '\0';

    /* 提取value */
    const char *val_start = eq + 1;
    while (*val_start == ' ' || *val_start == '\t') val_start++;

    size_t val_len = strlen(val_start);
    /* 去除换行符 */
    if (val_len > 0 && val_start[val_len-1] == '\n') val_len--;
    if (val_len >= value_size) return -1;

    memcpy(value, val_start, val_len);
    value[val_len] = '\0';

    return 0;
}

/**
 * @brief 使用SHA-256和PBKDF2算法对密码进行哈希
 *
 * 安全编码实践：
 * - 不存储明文密码
 * - 使用加盐哈希防止彩虹表攻击
 * - 使用足够的迭代次数（PBKDF2推荐至少10000次）
 *
 * @param password 明文密码
 * @param hash_out 输出缓冲区，存储十六进制哈希字符串
 * @param hash_size 输出缓冲区大小（至少65字节）
 * @return 0成功，-1失败
 */
int hash_password(const char *password, char *hash_out, size_t hash_size)
{
    if (!password || !hash_out || hash_size < 65) {
        return -1;
    }

    /* 注意：这是简化示例，生产环境应使用libsodium、OpenSSL或bcrypt库 */
    /* 真实实现应该：
     * 1. 生成随机盐值
     * 2. 使用PBKDF2、bcrypt或Argon2进行密钥派生
     * 3. 存储盐值和迭代次数与哈希结果
     */

    /* 示例：简单的SHA-256哈希（仅用于演示，生产环境请使用更强的算法） */
    /* 实际项目中推荐：
     * - libsodium: crypto_pwhash_str()
     * - OpenSSL: EVP_PBE_scrypt() 或 PKCS5_PBKDF2_HMAC()
     * - Linux: crypt() with SHA-512 or yescrypt
     */

    /* 使用OpenSSL示例（需要链接 -lcrypto）:
     * #include <openssl/evp.h>
     * #include <openssl/sha.h>
     *
     * unsigned char salt[16];
     * RAND_bytes(salt, sizeof(salt));  // 生成随机盐
     *
     * unsigned char hash[32];
     * PKCS5_PBKDF2_HMAC(password, strlen(password),
     *                   salt, sizeof(salt),
     *                   100000, EVP_sha256(),
     *                   sizeof(hash), hash);
     *
     * // 将结果编码为十六进制字符串存储
     * // 格式：$pbkdf2-sha256$iterations$salt$hash
     */

    /* 安全提示：永远不要在代码中硬编码密码 */
    /* 永远不要在日志中打印密码 */
    /* 内存中存储密码的变量应及时清零（explicit_bzero） */

    (void)password;  /* 占位：实际应调用密码哈希库 */

    /* 返回模拟的哈希值格式 */
    snprintf(hash_out, hash_size,
             "$pbkdf2-sha256$100000$%s$[HASH_VALUE]",
             "[RANDOM_SALT]");

    return 0;
}

/**
 * @brief 验证密码（用于登录验证）
 *
 * @param password 用户输入的明文密码
 * @param stored_hash 存储的密码哈希（包含算法、盐值、迭代次数和哈希）
 * @return 1密码匹配，0密码不匹配，-1错误
 */
int verify_password(const char *password, const char *stored_hash)
{
    if (!password || !stored_hash) {
        return -1;
    }

    /* 解析stored_hash获取算法参数和盐值 */
    /* 使用相同参数计算输入密码的哈希 */
    /* 使用crypto_compare()进行时间恒定的字符串比较 */

    /* 时间恒定比较防止时序攻击 */
    /* int crypto_compare(const char *a, const char *b, size_t len) {
     *     volatile unsigned char diff = 0;
     *     for (size_t i = 0; i < len; i++) {
     *         diff |= a[i] ^ b[i];
     *     }
     *     return diff;  // 0表示相等
     * }
     */

    (void)password;
    (void)stored_hash;

    /* 实际实现应该返回计算结果 */
    return 0;  /* 占位返回值 */
}

/**
 * ✅ 安全的配置解析函数
 */
int parse_config_secure(const char *filename)
{
    /* 验证路径 */
    if (validate_config_path(filename) != 0) {
        fprintf(stderr, "Invalid config path\n");  /* 不暴露具体路径 */
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open config: %s\n", strerror(errno));
        return -1;
    }

    char line[MAX_CONFIG_LINE];
    unsigned int line_num = 0;  /* 使用无符号避免负数 */
    int errors = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* 检查行号溢出 */
        if (line_num == UINT_MAX) {
            fprintf(stderr, "Config file too long\n");
            break;
        }
        line_num++;

        /* 跳过空行和注释 */
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;

        /* 检查行是否完整 */
        if (strlen(line) == sizeof(line) - 1 &&
            line[sizeof(line) - 2] != '\n') {
            fprintf(stderr, "Line %u too long\n", line_num);
            errors++;
            /* 读取剩余内容 */
            int c;
            while ((c = fgetc(fp)) != '\n' && c != EOF);
            continue;
        }

        /* 解析 */
        char key[MAX_KEY_LEN], value[MAX_VALUE_LEN];
        if (parse_config_line(line, key, sizeof(key),
                              value, sizeof(value)) != 0) {
            fprintf(stderr, "Line %u: parse error\n", line_num);
            errors++;
            continue;
        }

        /* 应用配置 */
        if (strcmp(key, "password") == 0) {
            /* 安全密码处理：使用哈希存储而非明文 */
            /* 使用PBKDF2、bcrypt或Argon2等算法进行密码哈希 */
            char password_hash[64] = {0};
            if (hash_password(value, password_hash, sizeof(password_hash)) != 0) {
                fprintf(stderr, "Line %u: password hashing failed\n", line_num);
                errors++;
            } else if (safe_strncpy(g_config.password_hash, password_hash,
                                    sizeof(g_config.password_hash)) != 0) {
                fprintf(stderr, "Line %u: password hash too long\n", line_num);
                errors++;
            }
        }
        /* ... 其他配置项 ... */
    }

    /* 确保文件关闭 */
    if (fclose(fp) != 0) {
        fprintf(stderr, "Error closing config file\n");
        return -1;
    }

    return errors;
}
