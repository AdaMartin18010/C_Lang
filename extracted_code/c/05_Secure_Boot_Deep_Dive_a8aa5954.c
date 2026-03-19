/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1018
 * Language: c
 * Block ID: a8aa5954
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * X.509证书链验证实现
 * 符合RFC 5280标准
 */

#include <crypto/x509.h>
#include <crypto/rsa.h>
#include <crypto/ecdsa.h>

/* 证书链最大深度 */
#define MAX_CERT_CHAIN_DEPTH    5

/* 证书信任锚 */
typedef struct {
    uint8_t subject_key_id[20];
    uint8_t public_key[520];    /* RSA-4096 */
    uint32_t key_len;
    uint32_t key_alg;
    bool is_ca;
} trust_anchor_t;

/* 验证证书链 */
int verify_certificate_chain(const x509_cert_t *chain[],
                             int chain_len,
                             const trust_anchor_t *trust_anchors,
                             int anchor_count) {

    if (chain_len > MAX_CERT_CHAIN_DEPTH) {
        ERROR("Certificate chain too deep: %d\n", chain_len);
        return -ECHAIN_TOO_DEEP;
    }

    /* 自底向上验证 */
    for (int i = 0; i < chain_len - 1; i++) {
        const x509_cert_t *cert = chain[i];
        const x509_cert_t *issuer = chain[i + 1];

        /* 1. 验证证书有效期 */
        if (!x509_check_validity(cert, get_current_time())) {
            ERROR("Certificate expired or not yet valid\n");
            return -EEXPIRED;
        }

        /* 2. 验证Issuer匹配 */
        if (!x509_name_equal(&cert->issuer, &issuer->subject)) {
            ERROR("Certificate chain broken at level %d\n", i);
            return -ECHAIN_BROKEN;
        }

        /* 3. 验证签名 */
        uint8_t tbs_hash[64];
        hash_x509_tbs(cert, tbs_hash);

        int ret;
        switch (issuer->pubkey_alg) {
            case ALG_RSA_PSS:
                ret = rsa_verify_pss(issuer->public_key,
                                    cert->signature_alg,
                                    tbs_hash, cert->sig_hash_len,
                                    cert->signature, cert->sig_len);
                break;
            case ALG_ECDSA:
                ret = ecdsa_verify(issuer->public_key,
                                  cert->signature_alg,
                                  tbs_hash, cert->sig_hash_len,
                                  cert->signature, cert->sig_len);
                break;
            default:
                return -EUNSUPPORTED_ALG;
        }

        if (ret != 0) {
            ERROR("Signature verification failed at level %d\n", i);
            return -ESIGNATURE_INVALID;
        }

        /* 4. 验证CA约束 */
        if (i < chain_len - 1 && !issuer->is_ca) {
            ERROR("Intermediate certificate is not a CA\n");
            return -ENOT_CA;
        }

        /* 5. 验证路径长度约束 */
        if (issuer->path_len_constraint >= 0 &&
            i > issuer->path_len_constraint) {
            ERROR("Path length constraint violated\n");
            return -EPATHLEN;
        }
    }

    /* 验证根证书 */
    const x509_cert_t *root = chain[chain_len - 1];
    bool root_trusted = false;

    for (int i = 0; i < anchor_count; i++) {
        if (memcmp(root->subject_key_id,
                   trust_anchors[i].subject_key_id, 20) == 0) {

            /* 验证根证书自签名 */
            uint8_t tbs_hash[64];
            hash_x509_tbs(root, tbs_hash);

            int ret;
            switch (root->pubkey_alg) {
                case ALG_RSA_PSS:
                    ret = rsa_verify_pss(root->public_key,
                                        root->signature_alg,
                                        tbs_hash, root->sig_hash_len,
                                        root->signature, root->sig_len);
                    break;
                /* ... */
            }

            if (ret == 0) {
                root_trusted = true;
                break;
            }
        }
    }

    if (!root_trusted) {
        ERROR("Root certificate not trusted\n");
        return -EUNTRUSTED_ROOT;
    }

    return 0;
}

/* 加载证书链从FIP */
int load_cert_chain_from_fip(uint32_t image_id,
                             x509_cert_t *chain[],
                             int *chain_len) {
    fip_toc_entry_t toc;
    int ret = fip_get_image_info(image_id, &toc);
    if (ret) return ret;

    uint8_t *cert_data = malloc(toc.size);
    fip_load_image(&toc, cert_data);

    /* 解析证书链（PKCS#7或裸X.509序列） */
    *chain_len = parse_cert_chain(cert_data, toc.size, chain);

    free(cert_data);
    return (*chain_len > 0) ? 0 : -1;
}
