/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 441
 * Language: c
 * Block ID: e049935f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：验证和使用之间存在时间窗口
verify_image(path);
// <-- 攻击者可能在这里替换文件
load_and_execute(path);

// 正确：原子操作，验证后立即使用
int verify_and_load(const uint8_t *image, size_t size) {
    if (verify_image(image, size) != 0) return -1;
    return execute_in_place(image);  // 直接执行已验证的内存
}
