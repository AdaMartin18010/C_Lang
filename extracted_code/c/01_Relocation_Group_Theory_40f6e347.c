/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\01_Relocation_Group_Theory.md
 * Line: 539
 * Language: c
 * Block ID: 40f6e347
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 预链接（Prelinking）
// 在编译时计算重定位，减少启动时间

// 重排序优化
// 将经常一起访问的符号放在一起，提高缓存命中率

// 符号绑定延迟
// RTLD_LAZY vs RTLD_NOW

void *lazy_load(const char *lib) {
    // RTLD_LAZY：只在首次使用时解析符号
    return dlopen(lib, RTLD_LAZY);
}

void *immediate_load(const char *lib) {
    // RTLD_NOW：立即解析所有符号
    return dlopen(lib, RTLD_NOW);
}
