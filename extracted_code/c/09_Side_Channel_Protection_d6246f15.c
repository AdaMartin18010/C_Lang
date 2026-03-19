/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\09_Side_Channel_Protection.md
 * Line: 159
 * Language: c
 * Block ID: d6246f15
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 侧信道攻击模型定义
 */

typedef enum {
    ATTACKER_PROFILE_REMOTE = 0,        /* 远程攻击者 */
    ATTACKER_PROFILE_LOCAL,             /* 本地软件攻击者 */
    ATTACKER_PROFILE_PHYSICAL,          /* 物理访问攻击者 */
    ATTACKER_PROFILE_ADVANCED,          /* 高级攻击者（实验室设备）*/
} attacker_profile_t;

typedef struct {
    attacker_profile_t profile;
    const char *name;
    const char *capabilities;
    const char *equipment;
    const char **attack_types;
} attacker_model_t;

static const attacker_model_t attacker_profiles[] = {
    {
        .profile = ATTACKER_PROFILE_REMOTE,
        .name = "远程攻击者",
        .capabilities = "网络访问，无物理接触",
        .equipment = "标准计算机",
        .attack_types = (const char *[]){
            "时序攻击",
            "缓存侧信道",
            "协议分析",
            NULL
        },
    },
    {
        .profile = ATTACKER_PROFILE_LOCAL,
        .name = "本地攻击者",
        .capabilities = "共享系统资源，代码执行",
        .equipment = "目标系统访问权限",
        .attack_types = (const char *[]){
            "缓存侧信道",
            "分支预测攻击",
            "内存访问模式分析",
            NULL
        },
    },
    {
        .profile = ATTACKER_PROFILE_PHYSICAL,
        .name = "物理攻击者",
        .capabilities = "设备物理访问，中等技能",
        .equipment = "示波器，逻辑分析仪",
        .attack_types = (const char *[]){
            "功耗分析",
            "电磁分析",
            "简单故障注入",
            NULL
        },
    },
    {
        .profile = ATTACKER_PROFILE_ADVANCED,
        .name = "高级攻击者",
        .capabilities = "专业设备，去封装能力",
        .equipment = "高阶示波器，激光器，探针台",
        .attack_types = (const char *[]){
            "差分功耗分析",
            "激光故障注入",
            "探针攻击",
            "反向工程",
            NULL
        },
    },
};

/* 威胁评估 */
typedef struct {
    const char *asset;
    attacker_profile_t min_attacker;
    const char *attack_vectors[10];
    uint32_t impact_score;      /* 1-10 */
    uint32_t likelihood_score;  /* 1-10 */
} threat_assessment_t;

static threat_assessment_t crypto_threats[] = {
    {
        .asset = "AES密钥",
        .min_attacker = ATTACKER_PROFILE_PHYSICAL,
        .attack_vectors = {"DPA", "CPA", "EMA"},
        .impact_score = 10,
        .likelihood_score = 7,
    },
    {
        .asset = "RSA私钥",
        .min_attacker = ATTACKER_PROFILE_REMOTE,
        .attack_vectors = {"时序攻击", "SPA", "缓存攻击"},
        .impact_score = 10,
        .likelihood_score = 8,
    },
    {
        .asset = "ECDSA私钥",
        .min_attacker = ATTACKER_PROFILE_PHYSICAL,
        .attack_vectors = {"DPA", "故障注入"},
        .impact_score = 10,
        .likelihood_score = 6,
    },
};
