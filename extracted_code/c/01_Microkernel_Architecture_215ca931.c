/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 131
 * Language: c
 * Block ID: 215ca931
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 能力 (Capability) */
typedef struct {
    uint64_t object_id;
    uint32_t rights;     /* 读/写/执行 */
    uint32_t issuer;
} capability_t;

/* 检查权限 */
bool check_cap(capability_t* cap, int required_right) {
    return (cap->rights & required_right) == required_right;
}

/* 使用能力访问资源 */
int cap_read(capability_t* cap, void* buf, size_t len) {
    if (!check_cap(cap, CAP_READ)) {
        return -EPERM;
    }

    return kernel_read(cap->object_id, buf, len);
}
