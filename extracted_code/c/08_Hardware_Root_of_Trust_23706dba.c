/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 2709
 * Language: c
 * Block ID: 23706dba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 硬件信任根与工业标准的关联
 */

typedef struct {
    const char *standard;
    const char *clause;
    const char *requirement;
    const char *implementation;
} standard_mapping_t;

static standard_mapping_t rot_standard_mappings[] = {
    {
        .standard = "NIST SP 800-193",
        .clause = "Section 4.1",
        .requirement = "Root of Trust for Measurement (RTM)",
        .implementation = "Boot ROM with measurement capability",
    },
    {
        .standard = "NIST SP 800-193",
        .clause = "Section 4.2",
        .requirement = "Root of Trust for Storage (RTS)",
        .implementation = "Secure storage with PUF-derived keys",
    },
    {
        .standard = "NIST SP 800-193",
        .clause = "Section 4.3",
        .requirement = "Root of Trust for Reporting (RTR)",
        .implementation = "TPM/TEE for attestation",
    },
    {
        .standard = "IEC 62443-4-2",
        .clause = "CR 3.4",
        .requirement = "Software and information integrity",
        .implementation = "Secure boot with cryptographic verification",
    },
    {
        .standard = "IEC 62443-4-2",
        .clause = "CR 3.8",
        .requirement = "Session authenticity",
        .implementation = "PUF-based device authentication",
    },
    {
        .standard = "GlobalPlatform TEE",
        .clause = "Internal API",
        .requirement = "Secure storage and cryptography",
        .implementation = "TEE with hardware-backed storage",
    },
    {
        .standard = "PSA Certified",
        .clause = "Security Model",
        .requirement = "RoT services",
        .implementation = "PSA RoT with PSA Crypto API",
    },
    {
        .standard = "ETSI EN 303 645",
        .clause = "Provision 5.1-1",
        .requirement = "No universal default passwords",
        .implementation = "PUF-derived unique device credentials",
    },
};
