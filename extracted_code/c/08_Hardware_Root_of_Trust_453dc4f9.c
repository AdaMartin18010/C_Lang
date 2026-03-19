/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 142
 * Language: c
 * Block ID: 453dc4f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 信任根类型定义和分类
 */

#include <stdint.h>
#include <stdbool.h>

/* 信任根类型 */
typedef enum {
    ROT_TYPE_HARDWARE = 0,      /* 硬件信任根 */
    ROT_TYPE_FIRMWARE,          /* 固件信任根 */
    ROT_TYPE_SOFTWARE,          /* 软件信任根 */
    ROT_TYPE_HYBRID,            /* 混合信任根 */
} rot_type_t;

/* 信任根能力 */
typedef enum {
    ROT_CAP_KEY_GENERATION    = (1 << 0),
    ROT_CAP_KEY_STORAGE       = (1 << 1),
    ROT_CAP_CRYPTO_OPS        = (1 << 2),
    ROT_CAP_SECURE_BOOT       = (1 << 3),
    ROT_CAP_MEASUREMENT       = (1 << 4),
    ROT_CAP_ATTESTATION       = (1 << 5),
    ROT_CAP_SEALING           = (1 << 6),
    ROT_CAP_DEBUG_CONTROL     = (1 << 7),
} rot_capability_t;

/* 信任根描述 */
typedef struct {
    rot_type_t type;
    const char *name;
    const char *implementation;
    uint32_t capabilities;
    bool is_mutable;
    bool is_measurable;
    uint32_t security_level;    /* 1-5，5为最高 */
} rot_descriptor_t;

/* 硬件RoT描述 */
static const rot_descriptor_t hardware_rots[] = {
    {
        .type = ROT_TYPE_HARDWARE,
        .name = "Boot ROM",
        .implementation = "Chip ROM",
        .capabilities = ROT_CAP_SECURE_BOOT | ROT_CAP_MEASUREMENT,
        .is_mutable = false,
        .is_measurable = false,  /* 自我不可测量 */
        .security_level = 5,
    },
    {
        .type = ROT_TYPE_HARDWARE,
        .name = "PUF",
        .implementation = "SRAM/Ring Oscillator",
        .capabilities = ROT_CAP_KEY_GENERATION | ROT_CAP_KEY_STORAGE,
        .is_mutable = false,
        .is_measurable = true,
        .security_level = 5,
    },
    {
        .type = ROT_TYPE_HARDWARE,
        .name = "eFuse",
        .implementation = "One-Time Programmable",
        .capabilities = ROT_CAP_KEY_STORAGE | ROT_CAP_DEBUG_CONTROL,
        .is_mutable = false,
        .is_measurable = true,
        .security_level = 5,
    },
    {
        .type = ROT_TYPE_HARDWARE,
        .name = "Secure Element",
        .implementation = "External HSM/SE",
        .capabilities = ROT_CAP_KEY_GENERATION | ROT_CAP_KEY_STORAGE |
                       ROT_CAP_CRYPTO_OPS | ROT_CAP_ATTESTATION,
        .is_mutable = false,
        .is_measurable = true,
        .security_level = 5,
    },
};

/* 固件RoT描述 */
static const rot_descriptor_t firmware_rots[] = {
    {
        .type = ROT_TYPE_FIRMWARE,
        .name = "BL1 (Trusted Boot ROM)",
        .implementation = "First stage bootloader",
        .capabilities = ROT_CAP_SECURE_BOOT | ROT_CAP_MEASUREMENT,
        .is_mutable = false,
        .is_measurable = true,
        .security_level = 4,
    },
    {
        .type = ROT_TYPE_FIRMWARE,
        .name = "BL2 (Trusted Boot Firmware)",
        .implementation = "Second stage bootloader",
        .capabilities = ROT_CAP_SECURE_BOOT | ROT_CAP_MEASUREMENT,
        .is_mutable = true,  /* 可更新 */
        .is_measurable = true,
        .security_level = 4,
    },
    {
        .type = ROT_TYPE_FIRMWARE,
        .name = "BL31 (EL3 Runtime)",
        .implementation = "Secure monitor",
        .capabilities = ROT_CAP_SECURE_BOOT | ROT_CAP_MEASUREMENT |
                       ROT_CAP_CRYPTO_OPS | ROT_CAP_ATTESTATION,
        .is_mutable = true,
        .is_measurable = true,
        .security_level = 4,
    },
};

/* 信任根评估 */
bool is_stronger_rot(const rot_descriptor_t *rot1, const rot_descriptor_t *rot2) {
    /* 硬件 > 固件 > 软件 */
    if (rot1->type != rot2->type) {
        return rot1->type < rot2->type;
    }

    /* 不可变 > 可变 */
    if (rot1->is_mutable != rot2->is_mutable) {
        return !rot1->is_mutable;
    }

    /* 安全级别 */
    return rot1->security_level > rot2->security_level;
}
