/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2626
 * Language: c
 * Block ID: 6f59a085
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根认证路径
 */

typedef enum {
    CERT_CC = 0,            /* Common Criteria */
    CERT_FIPS_140,          /* FIPS 140-2/3 */
    CERT_GP_TEE,            /* GlobalPlatform TEE */
    CERT_PSA_CERTIFIED,     /* PSA Certified */
    CERT_ISO_15408,         /* ISO/IEC 15408 */
} certification_type_t;

typedef struct {
    certification_type_t type;
    const char *name;
    const char *scope;
    int evaluation_level;   /* EAL1-7 for CC */
    int duration_months;
    int estimated_cost_usd;
    bool required_for_critical;
} certification_info_t;

static certification_info_t certifications[] = {
    {
        .type = CERT_CC,
        .name = "Common Criteria",
        .scope = "通用IT产品安全评估",
        .evaluation_level = 5,  /* EAL5 */
        .duration_months = 12,
        .estimated_cost_usd = 500000,
        .required_for_critical = true,
    },
    {
        .type = CERT_FIPS_140,
        .name = "FIPS 140-3",
        .scope = "加密模块安全",
        .evaluation_level = 3,  /* Level 3 */
        .duration_months = 9,
        .estimated_cost_usd = 300000,
        .required_for_critical = true,
    },
    {
        .type = CERT_GP_TEE,
        .name = "GlobalPlatform TEE",
        .scope = "可信执行环境",
        .evaluation_level = 2,
        .duration_months = 6,
        .estimated_cost_usd = 150000,
        .required_for_critical = false,
    },
    {
        .type = CERT_PSA_CERTIFIED,
        .name = "PSA Certified",
        .scope = "物联网平台安全",
        .evaluation_level = 3,  /* Level 3 */
        .duration_months = 4,
        .estimated_cost_usd = 50000,
        .required_for_critical = false,
    },
};

/* 推荐认证路径 */
void recommend_certification_path(cra_product_class_t product_class) {
    printf("Recommended Certification Path:\n\n");

    if (product_class >= CRA_CLASS_I) {
        printf("For Important/Critical Products:\n");
        printf("  1. Common Criteria EAL4+\n");
        printf("  2. FIPS 140-3 Level 2+\n");
        printf("  3. PSA Certified Level 3\n");
    } else {
        printf("For Default Class Products:\n");
        printf("  1. PSA Certified Level 1/2\n");
        printf("  2. Self-assessment against IEC 62443\n");
    }
}
