/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Sandboxing.md
 * Line: 676
 * Language: c
 * Block ID: 3bf128d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 丢弃所有capabilities
int drop_all_capabilities() {
    cap_t caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc");
        return -1;
    }

    // 清空所有能力集
    cap_clear(caps);

    // 设置空能力集
    if (cap_set_proc(caps) < 0) {
        perror("cap_set_proc");
        cap_free(caps);
        return -1;
    }

    cap_free(caps);
    return 0;
}

// 仅保留特定capabilities
int retain_capabilities_only(const cap_value_t* caps_to_keep, int count) {
    cap_t caps = cap_get_proc();
    if (caps == NULL) {
        return -1;
    }

    // 清空当前能力集
    cap_clear(caps);

    // 设置要保留的能力
    if (cap_set_flag(caps, CAP_EFFECTIVE, count, caps_to_keep, CAP_SET) < 0 ||
        cap_set_flag(caps, CAP_PERMITTED, count, caps_to_keep, CAP_SET) < 0) {
        perror("cap_set_flag");
        cap_free(caps);
        return -1;
    }

    // 应用到进程
    if (cap_set_proc(caps) < 0) {
        perror("cap_set_proc");
        cap_free(caps);
        return -1;
    }

    cap_free(caps);
    return 0;
}

// 沙箱化函数 - 最小权限原则
void sandbox_with_minimal_caps() {
    // 对于网络服务器，只保留绑定特权端口的能力
    cap_value_t allowed_caps[] = {
        CAP_NET_BIND_SERVICE,
    };

    retain_capabilities_only(allowed_caps, 1);

    // 切换到非特权用户
    setuid(1000);

    // 现在即使程序被攻破，攻击者也只有极有限的能力
}

// 完全沙箱化 - 无特权模式
void enter_full_sandbox() {
    // 1. 丢弃所有capabilities
    drop_all_capabilities();

    // 2. 清除环境能力
    for (int i = 0; i <= CAP_LAST_CAP; i++) {
        prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_LOWER, i, 0, 0);
    }

    // 3. 设置no_new_privs
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

    // 4. 进入chroot jail（可选）
    chroot("/empty");
    chdir("/");
}
