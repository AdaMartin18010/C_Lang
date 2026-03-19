/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 324
 * Language: c
 * Block ID: 4a0f274c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// U-Boot FIT镜像 (Flattened Image Tree)

// FIT镜像结构
// /images {
//     kernel@1 {
//         data = /incbin/("zImage");
//         hash@1 { algo = "sha256"; };
//         signature@1 {
//             algo = "rsa,sha256";
//             key-name-hint = "dev";
//         };
//     };
//     fdt@1 {
//         data = /incbin/("board.dtb");
//         hash@1 { algo = "sha256"; };
//     };
// };
// /configurations {
//     default = "conf@1";
//     conf@1 {
//         kernel = "kernel@1";
//         fdt = "fdt@1";
//         signature@1 {
//             algo = "rsa,sha256";
//             key-name-hint = "dev";
//             sign-images = "kernel", "fdt";
//         };
//     };
// };

// U-Boot验证回调
int fit_image_verify(void *fit, int noffset, const void *data,
                     size_t size) {
    // 获取配置的公钥
    const char *key_name = fdt_getprop(fit, noffset, "key-name-hint", NULL);

    // 从DTB或存储加载公钥
    RSA *pubkey = load_public_key(key_name);

    // 验证哈希
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256(data, size, hash);

    // 验证签名
    const uint8_t *sig = fdt_getprop(fit, noffset, "value", NULL);
    int sig_len = fdt_getprop_u32(fit, noffset, "value-len", 0);

    return RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                      sig, sig_len, pubkey);
}

// U-Boot启动命令
// setenv verify on
// bootm ${kernel_addr} - ${fdt_addr}
