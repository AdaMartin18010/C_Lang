/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 911
 * Language: c
 * Block ID: 7c7a1c9b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA嵌入式C开发安全设计检查清单
 */

/* 安全设计检查项 */
typedef struct {
    const char *id;
    const char *category;
    const char *requirement;
    const char *implementation_example;
    int priority;  /* 1=关键, 2=重要, 3=建议 */
    bool (*check_function)(void);
} security_check_item_t;

static security_check_item_t cra_security_checks[] = {
    /* 身份验证 */
    {
        .id = "CRA-AUTH-001",
        .category = "身份验证",
        .requirement = "实施强密码策略",
        .implementation_example =
            "int validate_password(const char *pwd) {\n"
            "    if (strlen(pwd) < 8) return -1;\n"
            "    if (!has_uppercase(pwd)) return -1;\n"
            "    if (!has_lowercase(pwd)) return -1;\n"
            "    if (!has_digit(pwd)) return -1;\n"
            "    if (!has_special(pwd)) return -1;\n"
            "    return 0;\n"
            "}",
        .priority = 1,
        .check_function = check_password_policy,
    },
    {
        .id = "CRA-AUTH-002",
        .category = "身份验证",
        .requirement = "防止暴力破解攻击",
        .implementation_example =
            "bool check_login_attempt(const char *username) {\n"
            "    login_attempt_t *attempt = get_attempt(username);\n"
            "    if (attempt->count >= MAX_ATTEMPTS) {\n"
            "        if (time(NULL) - attempt->last < LOCKOUT_TIME) {\n"
            "            return false; /* 锁定 */\n"
            "        }\n"
            "        reset_attempts(username);\n"
            "    }\n"
            "    return true;\n"
            "}",
        .priority = 1,
        .check_function = check_brute_force_protection,
    },
    {
        .id = "CRA-AUTH-003",
        .category = "身份验证",
        .requirement = "安全存储凭证",
        .implementation_example =
            "int store_password_hash(const char *username, \n"
            "                        const char *password) {\n"
            "    uint8_t salt[SALT_LEN];\n"
            "    uint8_t hash[HASH_LEN];\n"
            "    randombytes(salt, SALT_LEN);\n"
            "    argon2id_hash(password, salt, hash);\n"
            "    save_to_secure_storage(username, salt, hash);\n"
            "    return 0;\n"
            "}",
        .priority = 1,
        .check_function = check_credential_storage,
    },

    /* 通信安全 */
    {
        .id = "CRA-COMM-001",
        .category = "通信安全",
        .requirement = "使用TLS 1.3或更高版本",
        .implementation_example =
            "SSL_CTX *create_secure_context(void) {\n"
            "    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());\n"
            "    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);\n"
            "    SSL_CTX_set_default_verify_paths(ctx);\n"
            "    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);\n"
            "    return ctx;\n"
            "}",
        .priority = 1,
        .check_function = check_tls_version,
    },
    {
        .id = "CRA-COMM-002",
        .category = "通信安全",
        .requirement = "证书固定（Certificate Pinning）",
        .implementation_example =
            "bool verify_pin(const char *hostname, \n"
            "                X509 *cert) {\n"
            "    uint8_t pin[32];\n"
            "    get_cert_pin(cert, pin);\n"
            "    return memcmp(pin, expected_pins[hostname], 32) == 0;\n"
            "}",
        .priority = 2,
        .check_function = check_certificate_pinning,
    },
    {
        .id = "CRA-COMM-003",
        .category = "通信安全",
        .requirement = "禁用不安全的密码套件",
        .implementation_example =
            "const char *SECURE_CIPHERS = \n"
            "    \"TLS_AES_256_GCM_SHA384:\"\n"
            "    \"TLS_CHACHA20_POLY1305_SHA256:\"\n"
            "    \"TLS_AES_128_GCM_SHA256\";\n"
            "SSL_CTX_set_cipher_list(ctx, SECURE_CIPHERS);",
        .priority = 1,
        .check_function = check_cipher_suites,
    },

    /* 固件安全 */
    {
        .id = "CRA-FIRM-001",
        .category = "固件安全",
        .requirement = "实施安全启动",
        .implementation_example =
            "int secure_boot_verify(const uint8_t *fw, size_t len) {\n"
            "    uint8_t hash[32];\n"
            "    sha256(fw, len, hash);\n"
            "    if (verify_signature(hash, fw_sig) != 0) {\n"
            "        return -1; /* 拒绝启动 */\n"
            "    }\n"
            "    return 0;\n"
            "}",
        .priority = 1,
        .check_function = check_secure_boot,
    },
    {
        .id = "CRA-FIRM-002",
        .category = "固件安全",
        .requirement = "固件更新签名验证",
        .implementation_example =
            "int verify_update(const uint8_t *update, size_t len) {\n"
            "    uint8_t hash[32];\n"
            "    sha256(update, len, hash);\n"
            "    return rsa_verify_pss(root_key, hash, update_sig);\n"
            "}",
        .priority = 1,
        .check_function = check_update_signature,
    },
    {
        .id = "CRA-FIRM-003",
        .category = "固件安全",
        .requirement = "反滚动保护",
        .implementation_example =
            "bool check_rollback(uint32_t new_version) {\n"
            "    uint32_t current = get_current_version();\n"
            "    if (new_version < current) {\n"
            "        return false; /* 拒绝回滚 */\n"
            "    }\n"
            "    return true;\n"
            "}",
        .priority = 2,
        .check_function = check_rollback_protection,
    },

    /* 数据保护 */
    {
        .id = "CRA-DATA-001",
        .category = "数据保护",
        .requirement = "敏感数据加密存储",
        .implementation_example =
            "int encrypt_sensitive_data(const uint8_t *plaintext,\n"
            "                           uint8_t *ciphertext) {\n"
            "    uint8_t key[32], iv[16];\n"
            "    derive_storage_key(key);\n"
            "    randombytes(iv, 16);\n"
            "    aes_gcm_encrypt(key, iv, plaintext, ciphertext);\n"
            "    return 0;\n"
            "}",
        .priority = 1,
        .check_function = check_data_encryption,
    },
    {
        .id = "CRA-DATA-002",
        .category = "数据保护",
        .requirement = "安全擦除敏感数据",
        .implementation_example =
            "void secure_erase(void *ptr, size_t len) {\n"
            "    volatile uint8_t *p = ptr;\n"
            "    while (len--) *p++ = 0;\n"
            "    memory_barrier();\n"
            "}",
        .priority = 1,
        .check_function = check_secure_erase,
    },

    /* 日志记录 */
    {
        .id = "CRA-LOG-001",
        .category = "日志记录",
        .requirement = "记录安全相关事件",
        .implementation_example =
            "void log_security_event(event_type_t type,\n"
            "                        const char *details) {\n"
            "    log_entry_t entry = {\n"
            "        .timestamp = time(NULL),\n"
            "        .type = type,\n"
            "        .severity = get_severity(type)\n"
            "    };\n"
            "    strncpy(entry.details, details, sizeof(entry.details));\n"
            "    write_secure_log(&entry);\n"
            "}",
        .priority = 1,
        .check_function = check_security_logging,
    },
    {
        .id = "CRA-LOG-002",
        .category = "日志记录",
        .requirement = "日志完整性保护",
        .implementation_example =
            "void write_secure_log(log_entry_t *entry) {\n"
            "    uint8_t hash[32];\n"
            "    sha256(entry, sizeof(*entry), hash);\n"
            "    hmac_sha256(log_key, hash, entry->mac);\n"
            "    append_to_log(entry);\n"
            "}",
        .priority = 2,
        .check_function = check_log_integrity,
    },
};

/* 运行检查清单 */
int run_security_checklist(void) {
    int passed = 0;
    int failed = 0;
    int critical_failed = 0;

    printf("=== EU CRA Security Checklist ===\n\n");

    for (size_t i = 0; i < sizeof(cra_security_checks)/sizeof(cra_security_checks[0]); i++) {
        security_check_item_t *item = &cra_security_checks[i];

        printf("[%s] %s - %s\n",
               item->priority == 1 ? "CRITICAL" :
               item->priority == 2 ? "IMPORTANT" : "RECOMMENDED",
               item->id, item->requirement);

        bool result = item->check_function();

        if (result) {
            printf("  ✓ PASSED\n");
            passed++;
        } else {
            printf("  ✗ FAILED\n");
            printf("  Implementation Example:\n%s\n", item->implementation_example);
            failed++;
            if (item->priority == 1) {
                critical_failed++;
            }
        }
        printf("\n");
    }

    printf("=== Summary ===\n");
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    printf("Critical Failed: %d\n", critical_failed);

    return critical_failed > 0 ? -1 : 0;
}
