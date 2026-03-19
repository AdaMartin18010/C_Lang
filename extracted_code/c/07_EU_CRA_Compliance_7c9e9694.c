/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 353
 * Language: c
 * Block ID: 7c9e9694
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA安全设计要求实现
 * 基于ETSI EN 303 645和IEC 62443-4-2
 */

typedef enum {
    /* Article 10(1) - 设计和开发 */
    REQUIREMENT_SECURE_BY_DESIGN = 0,
    REQUIREMENT_SECURE_DEFAULT,
    REQUIREMENT_CONFIDENTIALITY,
    REQUIREMENT_INTEGRITY,
    REQUIREMENT_AVAILABILITY,
    REQUIREMENT_ACCESS_CONTROL,
    REQUIREMENT_DATA_PROTECTION,

    /* Article 10(2) - 产品信息 */
    REQUIREMENT_SECURITY_INFO,
    REQUIREMENT_VULNERABILITY_INFO,
    REQUIREMENT_UPDATE_INFO,

    /* Article 10(3) - 漏洞处理 */
    REQUIREMENT_VULNERABILITY_HANDLING,
    REQUIREMENT_SECURITY_UPDATES,
    REQUIREMENT_REPORTING,
} cra_requirement_t;

/* 安全设计要求结构 */
typedef struct {
    cra_requirement_t requirement;
    const char *description;
    const char *implementation_guidance;
    bool (*verify)(void);
} cra_security_requirement_t;

/* 安全设计要求实现 */
static cra_security_requirement_t cra_requirements[] = {
    {
        .requirement = REQUIREMENT_SECURE_BY_DESIGN,
        .description = "产品应以安全方式设计和开发",
        .implementation_guidance = "实施安全开发生命周期(SDLC)，包括威胁建模、代码审查、安全测试",
        .verify = verify_secure_design,
    },
    {
        .requirement = REQUIREMENT_SECURE_DEFAULT,
        .description = "产品应带有安全的默认配置",
        .implementation_guidance = "默认启用安全功能，禁用不安全功能，强密码要求",
        .verify = verify_secure_defaults,
    },
    {
        .requirement = REQUIREMENT_CONFIDENTIALITY,
        .description = "保护数据和功能的机密性",
        .implementation_guidance = "加密传输和存储的敏感数据，实施访问控制",
        .verify = verify_confidentiality,
    },
    {
        .requirement = REQUIREMENT_INTEGRITY,
        .description = "保护数据和功能的完整性",
        .implementation_guidance = "数字签名、哈希验证、防篡改机制",
        .verify = verify_integrity,
    },
    {
        .requirement = REQUIREMENT_AVAILABILITY,
        .description = "确保产品和服务的可用性",
        .implementation_guidance = "冗余设计、故障恢复、DDoS防护",
        .verify = verify_availability,
    },
    {
        .requirement = REQUIREMENT_ACCESS_CONTROL,
        .description = "实施适当的访问控制机制",
        .implementation_guidance = "身份验证、授权、最小权限原则",
        .verify = verify_access_control,
    },
    {
        .requirement = REQUIREMENT_DATA_PROTECTION,
        .description = "保护个人数据",
        .implementation_guidance = "数据最小化、目的限制、存储限制、加密",
        .verify = verify_data_protection,
    },
};

/* 验证安全设计 */
bool verify_secure_design(void) {
    /* 检查是否存在安全设计文档 */
    if (!file_exists("SECURITY_DESIGN.md")) {
        return false;
    }

    /* 检查威胁建模文档 */
    if (!file_exists("THREAT_MODEL.md")) {
        return false;
    }

    /* 检查安全测试报告 */
    if (!file_exists("SECURITY_TEST_REPORT.pdf")) {
        return false;
    }

    return true;
}

/* 验证安全默认配置 */
bool verify_secure_defaults(void) {
    config_t *cfg = load_default_config();

    /* 检查默认密码策略 */
    if (cfg->password_policy.min_length < 8) {
        return false;
    }

    /* 检查默认启用加密 */
    if (!cfg->encryption.enabled_by_default) {
        return false;
    }

    /* 检查不安全服务默认禁用 */
    if (cfg->services.telnet.enabled_by_default) {
        return false;
    }

    /* 检查调试接口默认禁用 */
    if (cfg->debug.jtag_enabled_by_default) {
        return false;
    }

    return true;
}

/* 安全设计检查表 */
typedef struct {
    const char *category;
    const char *check_item;
    bool required;
    bool implemented;
    const char *evidence;
} security_design_check_t;

static security_design_check_t design_checks[] = {
    {
        .category = "身份验证",
        .check_item = "实施强密码策略（最小8位，复杂度要求）",
        .required = true,
        .implemented = false,
        .evidence = "password_policy.c",
    },
    {
        .category = "身份验证",
        .check_item = "支持多因素认证",
        .required = false,
        .implemented = false,
        .evidence = "mfa_module.c",
    },
    {
        .category = "通信安全",
        .check_item = "所有网络通信使用TLS 1.3或更高版本",
        .required = true,
        .implemented = false,
        .evidence = "tls_config.h",
    },
    {
        .category = "通信安全",
        .check_item = "禁用不安全的SSL/TLS版本",
        .required = true,
        .implemented = false,
        .evidence = "tls_config.h",
    },
    {
        .category = "固件安全",
        .check_item = "实施安全启动",
        .required = true,
        .implemented = false,
        .evidence = "secure_boot.c",
    },
    {
        .category = "固件安全",
        .check_item = "固件更新必须签名验证",
        .required = true,
        .implemented = false,
        .evidence = "fw_update.c",
    },
    {
        .category = "日志记录",
        .check_item = "记录安全相关事件",
        .required = true,
        .implemented = false,
        .evidence = "security_logger.c",
    },
    {
        .category = "日志记录",
        .check_item = "保护日志完整性",
        .required = true,
        .implemented = false,
        .evidence = "log_integrity.c",
    },
};
