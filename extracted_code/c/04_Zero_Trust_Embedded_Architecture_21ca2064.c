/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 649
 * Language: c
 * Block ID: 21ca2064
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 基于Linux capabilities的最小权限模型
 */

#include <sys/capability.h>
#include <sys/prctl.h>

/* 应用权限配置文件 */
typedef struct {
    const char *app_name;
    cap_value_t caps[8];        // 所需能力列表
    int cap_count;
    uid_t uid;                  // 运行用户
    gid_t gid;                  // 运行组
    bool no_new_privs;          // 禁止提升权限
    bool seccomp_filter;        // 系统调用过滤
} app_privilege_profile_t;

/* 传感器驱动权限配置 */
app_privilege_profile_t sensor_profile = {
    .app_name = "sensor_driver",
    .caps = {CAP_IPC_LOCK, CAP_SYS_NICE},  // 只需锁定内存和设置优先级
    .cap_count = 2,
    .uid = 1000,  // 非特权用户
    .gid = 1000,
    .no_new_privs = true,
    .seccomp_filter = true,
};

/* 网络服务权限配置 */
app_privilege_profile_t network_profile = {
    .app_name = "network_service",
    .caps = {CAP_NET_BIND_SERVICE, CAP_NET_RAW},  // 仅网络相关
    .cap_count = 2,
    .uid = 1001,
    .gid = 1001,
    .no_new_privs = true,
    .seccomp_filter = true,
};

int apply_privilege_profile(const app_privilege_profile_t *profile) {
    /* 1. 设置no_new_privs（禁止setuid提升） */
    if (profile->no_new_privs) {
        prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
    }

    /* 2. 设置能力集 */
    cap_t caps = cap_init();
    cap_clear(caps);

    /* 只保留必要的能力 */
    for (int i = 0; i < profile->cap_count; i++) {
        cap_set_flag(caps, CAP_EFFECTIVE, 1, &profile->caps[i], CAP_SET);
        cap_set_flag(caps, CAP_PERMITTED, 1, &profile->caps[i], CAP_SET);
    }

    cap_set_proc(caps);
    cap_free(caps);

    /* 3. 降权到普通用户 */
    setgid(profile->gid);
    setuid(profile->uid);

    /* 4. 加载seccomp过滤器 */
    if (profile->seccomp_filter) {
        load_seccomp_filter(profile->app_name);
    }

    /* 此时进程只有最小权限 */
    return 0;
}
