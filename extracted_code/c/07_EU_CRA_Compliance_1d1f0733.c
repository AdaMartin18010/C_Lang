/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 1408
 * Language: c
 * Block ID: 1d1f0733
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA文档要求生成工具
 */

/* 技术文档结构 */
typedef struct {
    char product_name[128];
    char product_version[32];
    char manufacturer[128];
    char contact_email[128];

    /* Article 13(1) - 安全信息 */
    struct {
        char secure_config_guide[4096];
        char security_features[2048];
        char user_responsibilities[2048];
    } security_info;

    /* Article 13(2) - 漏洞处理 */
    struct {
        char reporting_channel[512];
        char response_time[256];
        char disclosure_policy[2048];
    } vulnerability_info;

    /* Article 13(3) - 更新信息 */
    struct {
        char update_method[1024];
        char support_period[256];
        char support_end_notification[1024];
    } update_info;
} cra_technical_documentation_t;

/* 生成技术文档 */
int generate_technical_documentation(cra_technical_documentation_t *doc,
                                      const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "# Technical Documentation\n\n");
    fprintf(fp, "Product: %s v%s\n", doc->product_name, doc->product_version);
    fprintf(fp, "Manufacturer: %s\n", doc->manufacturer);
    fprintf(fp, "Contact: %s\n\n", doc->contact_email);

    /* Article 13(1) */
    fprintf(fp, "## 1. Security Information (Article 13(1))\n\n");
    fprintf(fp, "### 1.1 Secure Configuration Guide\n");
    fprintf(fp, "%s\n\n", doc->security_info.secure_config_guide);

    fprintf(fp, "### 1.2 Security Features\n");
    fprintf(fp, "%s\n\n", doc->security_info.security_features);

    fprintf(fp, "### 1.3 User Responsibilities\n");
    fprintf(fp, "%s\n\n", doc->security_info.user_responsibilities);

    /* Article 13(2) */
    fprintf(fp, "## 2. Vulnerability Handling (Article 13(2))\n\n");
    fprintf(fp, "### 2.1 Reporting Channel\n");
    fprintf(fp, "%s\n\n", doc->vulnerability_info.reporting_channel);

    fprintf(fp, "### 2.2 Response Time\n");
    fprintf(fp, "%s\n\n", doc->vulnerability_info.response_time);

    fprintf(fp, "### 2.3 Disclosure Policy\n");
    fprintf(fp, "%s\n\n", doc->vulnerability_info.disclosure_policy);

    /* Article 13(3) */
    fprintf(fp, "## 3. Update Information (Article 13(3))\n\n");
    fprintf(fp, "### 3.1 Update Method\n");
    fprintf(fp, "%s\n\n", doc->update_info.update_method);

    fprintf(fp, "### 3.2 Support Period\n");
    fprintf(fp, "%s\n\n", doc->update_info.support_period);

    fprintf(fp, "### 3.3 Support End Notification\n");
    fprintf(fp, "%s\n\n", doc->update_info.support_end_notification);

    fclose(fp);
    return 0;
}

/* SBOM生成 */
typedef struct {
    char name[128];
    char version[32];
    char supplier[128];
    char license[64];
    char checksum[64];
    bool is_modified;
} sbom_component_t;

typedef struct {
    char spdx_version[16];
    char document_name[128];
    char creation_time[32];
    sbom_component_t *components;
    size_t num_components;
} sbom_document_t;

int generate_sbom(const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "{\n");
    fprintf(fp, "  \"spdxVersion\": \"SPDX-2.3\",\n");
    fprintf(fp, "  \"dataLicense\": \"CC0-1.0\",\n");
    fprintf(fp, "  \"SPDXID\": \"SPDXRef-DOCUMENT\",\n");
    fprintf(fp, "  \"name\": \"%s-sbom\",\n", product_name);
    fprintf(fp, "  \"documentNamespace\": \"https://%s/sbom/%s\",\n",
            manufacturer_domain, product_name);
    fprintf(fp, "  \"creationInfo\": {\n");
    fprintf(fp, "    \"created\": \"%s\",\n", get_iso_timestamp());
    fprintf(fp, "    \"creators\": [\"Tool: cra-sbom-generator-1.0\"]\n");
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"packages\": [\n");

    /* 遍历所有组件 */
    for (size_t i = 0; i < num_components; i++) {
        sbom_component_t *comp = &components[i];
        fprintf(fp, "    {\n");
        fprintf(fp, "      \"SPDXID\": \"SPDXRef-Package-%zu\",\n", i);
        fprintf(fp, "      \"name\": \"%s\",\n", comp->name);
        fprintf(fp, "      \"versionInfo\": \"%s\",\n", comp->version);
        fprintf(fp, "      \"supplier\": \"Person: %s\",\n", comp->supplier);
        fprintf(fp, "      \"downloadLocation\": \"NOASSERTION\",\n");
        fprintf(fp, "      \"filesAnalyzed\": false,\n");
        fprintf(fp, "      \"verificationCode\": {\n");
        fprintf(fp, "        \"packageVerificationCodeValue\": \"%s\"\n",
                comp->checksum);
        fprintf(fp, "      },\n");
        fprintf(fp, "      \"licenseConcluded\": \"%s\",\n", comp->license);
        fprintf(fp, "      \"licenseDeclared\": \"NOASSERTION\",\n");
        fprintf(fp, "      \"copyrightText\": \"NOASSERTION\"\n");
        fprintf(fp, "    }%s\n", i < num_components - 1 ? "," : "");
    }

    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");

    fclose(fp);
    return 0;
}
