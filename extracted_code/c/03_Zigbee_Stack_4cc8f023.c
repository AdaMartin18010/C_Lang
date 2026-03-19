/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 1187
 * Language: c
 * Block ID: 4cc8f023
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AES加密/解密操作
#define AES_SET_MODE    0x04
#define AES_ENCRYPT     0x00
#define AES_DECRYPT     0x02
#define AES_LOAD_KEY    0x00
#define AES_LOAD_IV     0x01
#define AES_START       0x02

// AES数据结构
typedef struct {
    uint8_t key[16];    // 128位密钥
    uint8_t data[16];   // 128位数据块
    uint8_t iv[16];     // 初始化向量（CBC模式）
} aes_block_t;

// 使用硬件AES加密
void cc2530_aes_encrypt(aes_block_t* block) {
    // 停止任何正在进行的AES操作
    ENCCS = 0x00;

    // 加载密钥
    ENCCS = AES_SET_MODE | AES_LOAD_KEY;
    for (int i = 0; i < 16; i++) {
        ENCDI = block->key[i];
    }

    // 设置加密模式
    ENCCS = AES_SET_MODE | AES_ENCRYPT;

    // 加载数据并启动加密
    for (int i = 0; i < 16; i++) {
        ENCDI = block->data[i];
    }

    // 等待完成
    while (ENCCS & 0x08);

    // 读取结果
    for (int i = 0; i < 16; i++) {
        block->data[i] = ENCDO;
    }
}

// CCM*模式（使用硬件AES引擎）
void cc2530_ccm_star(uint8_t* key, uint8_t* nonce, uint8_t* a, uint8_t len_a,
                     uint8_t* m, uint8_t len_m, uint8_t* ciphertext,
                     uint8_t* mic, uint8_t sec_level) {
    // CCM*实现，使用硬件AES加速
    // ...
}
