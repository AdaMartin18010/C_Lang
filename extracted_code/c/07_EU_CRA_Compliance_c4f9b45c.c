/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 2023
 * Language: c
 * Block ID: c4f9b45c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 软件物料清单（SBOM）生成
 * 符合SPDX和CycloneDX标准
 */

typedef enum {
    SBOM_FORMAT_SPDX,
    SBOM_FORMAT_CYCLONEDX,
} sbom_format_t;

/* 组件类型 */
typedef enum {
    COMP_TYPE_APPLICATION,
    COMP_TYPE_FRAMEWORK,
    COMP_TYPE_LIBRARY,
    COMP_TYPE_DEVICE,
    COMP_TYPE_FIRMWARE,
} component_type_t;

/* 组件信息 */
typedef struct {
    char name[128];
    char version[32];
    char supplier[256];
    char originator[256];
    component_type_t type;
    char checksum_sha256[65];
    char license[64];
    char copyright[256];
    bool is_modified;
    char cpe[256];              /* CPE标识符 */
    char purl[512];             /* Package URL */
    char swid_tag[512];         /* SWID标签 */
} sbom_component_t;

/* 扫描源代码生成SBOM */
int scan_source_for_sbom(const char *source_dir,
                         sbom_component_t **components,
                         size_t *count) {
    /* 1. 扫描包管理器文件 */
    scan_package_lock_files(source_dir, components, count);

    /* 2. 扫描C头文件 */
    scan_c_headers(source_dir, components, count);

    /* 3. 扫描链接库 */
    scan_linked_libraries(source_dir, components, count);

    /* 4. 扫描构建脚本 */
    scan_build_scripts(source_dir, components, count);

    return 0;
}

/* 生成SPDX格式SBOM */
int generate_spdx_sbom(const sbom_component_t *components,
                       size_t count,
                       const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    /* SPDX头部 */
    fprintf(fp, "SPDXVersion: SPDX-2.3\n");
    fprintf(fp, "DataLicense: CC0-1.0\n");
    fprintf(fp, "SPDXID: SPDXRef-DOCUMENT\n");
    fprintf(fp, "DocumentName: %s\n", product_name);
    fprintf(fp, "DocumentNamespace: https://%s/sbom/%s-%s\n",
            manufacturer_domain, product_name, product_version);
    fprintf(fp, "Creator: Tool: cra-sbom-generator-1.0\n");
    fprintf(fp, "Created: %s\n", get_iso_timestamp());
    fprintf(fp, "\n");

    /* 包信息 */
    for (size_t i = 0; i < count; i++) {
        const sbom_component_t *comp = &components[i];

        fprintf(fp, "PackageName: %s\n", comp->name);
        fprintf(fp, "SPDXID: SPDXRef-Package-%zu\n", i);
        fprintf(fp, "PackageVersion: %s\n", comp->version);
        fprintf(fp, "PackageSupplier: %s\n", comp->supplier);
        fprintf(fp, "PackageOriginator: %s\n", comp->originator);
        fprintf(fp, "PackageDownloadLocation: NOASSERTION\n");
        fprintf(fp, "FilesAnalyzed: false\n");
        fprintf(fp, "PackageVerificationCode: %s\n", comp->checksum_sha256);
        fprintf(fp, "PackageLicenseConcluded: %s\n", comp->license);
        fprintf(fp, "PackageLicenseDeclared: NOASSERTION\n");
        fprintf(fp, "PackageCopyrightText: %s\n", comp->copyright);

        /* 外部引用 */
        if (strlen(comp->cpe) > 0) {
            fprintf(fp, "ExternalRef: SECURITY cpe23Type %s\n", comp->cpe);
        }
        if (strlen(comp->purl) > 0) {
            fprintf(fp, "ExternalRef: PACKAGE-MANAGER purl %s\n", comp->purl);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
    return 0;
}

/* 生成CycloneDX格式SBOM */
int generate_cyclonedx_sbom(const sbom_component_t *components,
                            size_t count,
                            const char *output_file) {
    FILE *fp = fopen(output_file, "w");
    if (!fp) return -1;

    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<bom xmlns=\"http://cyclonedx.org/schema/bom/1.4\"\n");
    fprintf(fp, "     serialNumber=\"urn:uuid:%s\"\n", generate_uuid());
    fprintf(fp, "     version=\"1\">\n");

    fprintf(fp, "  <metadata>\n");
    fprintf(fp, "    <timestamp>%s</timestamp>\n", get_iso_timestamp());
    fprintf(fp, "    <tools>\n");
    fprintf(fp, "      <tool>\n");
    fprintf(fp, "        <vendor>Example Corp</vendor>\n");
    fprintf(fp, "        <name>cra-sbom-generator</name>\n");
    fprintf(fp, "        <version>1.0</version>\n");
    fprintf(fp, "      </tool>\n");
    fprintf(fp, "    </tools>\n");
    fprintf(fp, "    <component type=\"application\">\n");
    fprintf(fp, "      <name>%s</name>\n", product_name);
    fprintf(fp, "      <version>%s</version>\n", product_version);
    fprintf(fp, "    </component>\n");
    fprintf(fp, "  </metadata>\n");

    fprintf(fp, "  <components>\n");

    for (size_t i = 0; i < count; i++) {
        const sbom_component_t *comp = &components[i];

        const char *type_str =
            comp->type == COMP_TYPE_APPLICATION ? "application" :
            comp->type == COMP_TYPE_LIBRARY ? "library" :
            comp->type == COMP_TYPE_FRAMEWORK ? "framework" :
            comp->type == COMP_TYPE_FIRMWARE ? "firmware" : "device";

        fprintf(fp, "    <component type=\"%s\">\n", type_str);
        fprintf(fp, "      <name>%s</name>\n", comp->name);
        fprintf(fp, "      <version>%s</version>\n", comp->version);
        fprintf(fp, "      <supplier>\n");
        fprintf(fp, "        <name>%s</name>\n", comp->supplier);
        fprintf(fp, "      </supplier>\n");
        fprintf(fp, "      <hashes>\n");
        fprintf(fp, "        <hash alg=\"SHA-256\">%s</hash>\n", comp->checksum_sha256);
        fprintf(fp, "      </hashes>\n");
        fprintf(fp, "      <licenses>\n");
        fprintf(fp, "        <license>\n");
        fprintf(fp, "          <id>%s</id>\n", comp->license);
        fprintf(fp, "        </license>\n");
        fprintf(fp, "      </licenses>\n");
        fprintf(fp, "      <cpe>%s</cpe>\n", comp->cpe);
        fprintf(fp, "      <purl>%s</purl>\n", comp->purl);
        fprintf(fp, "    </component>\n");
    }

    fprintf(fp, "  </components>\n");
    fprintf(fp, "</bom>\n");

    fclose(fp);
    return 0;
}
