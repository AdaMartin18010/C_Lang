/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 892
 * Language: c
 * Block ID: c516eeab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AES-CCM*加密/解密
// CCM*提供: 加密 + 认证 + 完整性检查

typedef struct {
    uint8_t key[16];           // 128位密钥
    uint8_t nonce[13];         // 13字节Nonce
    uint8_t a[256];            // 认证数据 (Adata)
    uint8_t m[128];            // 明文/密文数据
    uint8_t len_a;             // 认证数据长度
    uint8_t len_m;             // 数据长度
    uint8_t mic_len;           // MIC长度 (0, 4, 8, 16)
} ccm_params_t;

// 生成Nonce
void generate_nonce(uint8_t* src_addr, uint32_t frame_counter, uint8_t sec_level,
                    uint8_t* nonce) {
    memcpy(nonce, src_addr, 8);           // 源地址 (8字节)
    nonce[8] = frame_counter >> 24;       // 帧计数器 (4字节)
    nonce[9] = frame_counter >> 16;
    nonce[10] = frame_counter >> 8;
    nonce[11] = frame_counter;
    nonce[12] = sec_level;                // 安全级别
}

// CCM*加密
int aes_ccm_star_encrypt(ccm_params_t* params, uint8_t* ciphertext, uint8_t* mic) {
    // 1. CTR模式加密
    // 2. CBC-MAC计算认证标签
    // 3. 输出密文 + MIC
    // 具体实现使用AES硬件或软件库
    return 0;
}

// CCM*解密和验证
int aes_ccm_star_decrypt(ccm_params_t* params, uint8_t* ciphertext, uint8_t* mic,
                         uint8_t* plaintext) {
    // 1. 验证MIC
    // 2. CTR模式解密
    // 3. 返回明文或验证失败
    return 0;
}

// 安全级别
typedef enum {
    SEC_NONE = 0,           // 无安全
    SEC_MIC32 = 1,          // 仅MIC (32位)
    SEC_MIC64 = 2,          // 仅MIC (64位)
    SEC_MIC128 = 3,         // 仅MIC (128位)
    SEC_ENC = 4,            // 仅加密
    SEC_ENC_MIC32 = 5,      // 加密 + MIC (32位)
    SEC_ENC_MIC64 = 6,      // 加密 + MIC (64位)
    SEC_ENC_MIC128 = 7,     // 加密 + MIC (128位)
} security_level_t;
