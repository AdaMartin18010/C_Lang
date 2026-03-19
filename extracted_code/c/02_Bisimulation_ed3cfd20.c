/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 339
 * Language: c
 * Block ID: ed3cfd20
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于签名的互模拟算法
#include <string.h>
#include <stdio.h>

typedef struct {
    char *signature;
    int state;
} StateSignature;

// 计算状态的签名
char *compute_signature(LTS *lts, int state, char **prev_sigs) {
    // 签名格式: "(action1->sig1,action2->sig2,...)"
    char buffer[4096] = "(";

    char **actions = lts_get_actions(lts);
    int num_actions = lts_num_actions(lts);

    bool first = true;
    for (int a = 0; a < num_actions; a++) {
        char *action = actions[a];
        int *targets = lts_get_targets(lts, state, action);
        int tcount = lts_target_count(lts, state, action);

        for (int i = 0; i < tcount; i++) {
            if (!first) strcat(buffer, ",");
            first = false;

            char entry[256];
            snprintf(entry, sizeof(entry), "%s->%s",
                     action, prev_sigs[targets[i]]);
            strcat(buffer, entry);
        }
    }

    strcat(buffer, ")");
    return strdup(buffer);
}

// 签名算法主函数
Partition *signature_bisimulation(LTS *lts) {
    int n = lts_num_states(lts);
    char **signatures = calloc(n, sizeof(char *));
    char **new_signatures = calloc(n, sizeof(char *));

    // 初始签名（基于输出标签）
    for (int i = 0; i < n; i++) {
        char label[64];
        snprintf(label, sizeof(label), "[%d]",
                 lts_get_output_label(lts, i));
        signatures[i] = strdup(label);
    }

    bool changed;
    do {
        changed = false;

        // 计算新签名
        for (int i = 0; i < n; i++) {
            new_signatures[i] = compute_signature(lts, i, signatures);
            if (strcmp(new_signatures[i], signatures[i]) != 0) {
                changed = true;
            }
            free(signatures[i]);
        }

        // 交换
        char **tmp = signatures;
        signatures = new_signatures;
        new_signatures = tmp;

    } while (changed);

    // 根据签名构建分区
    return partition_from_signatures(signatures, n);
}
