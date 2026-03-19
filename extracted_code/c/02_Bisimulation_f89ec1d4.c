/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\02_Bisimulation.md
 * Line: 541
 * Language: c
 * Block ID: f89ec1d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 离散时间马尔可夫链的概率互模拟
# include <math.h>
# include <stdlib.h>

typedef struct {
    int from;
    int to;
    double prob;
} ProbTransition;

typedef struct {
    ProbTransition *trans;
    int count;
} ProbTransitionList;

// 计算状态在动作下的概率分布到分区的映射
double *prob_distribution_to_blocks(Partition *P,
                                     ProbTransitionList *ptl,
                                     int state) {
    int num_blocks = count_blocks(P);
    double *dist = calloc(num_blocks, sizeof(double));

    // 收集从state出发的所有转移
    for (int i = 0; i < ptl->count; i++) {
        if (ptl->trans[i].from == state) {
            int block = P->state_to_block[ptl->trans[i].to];
            dist[block] += ptl->trans[i].prob;
        }
    }

    return dist;
}

// 概率互模拟的分区精化
bool prob_bisim_split(Partition *P, ProbTransitionList *ptl, int state) {
    int num_blocks = count_blocks(P);
    double *dist = prob_distribution_to_blocks(P, ptl, state);

    // 检查同一分区中的其他状态是否有相同分布
    int my_block = P->state_to_block[state];
    Block *block = get_block(P, my_block);

    for (int i = 0; i < block->count; i++) {
        int other = block->states[i];
        if (other == state) continue;

        double *other_dist = prob_distribution_to_blocks(P, ptl, other);

        bool same = true;
        for (int b = 0; b < num_blocks; b++) {
            if (fabs(dist[b] - other_dist[b]) > 1e-9) {
                same = false;
                break;
            }
        }

        free(other_dist);

        if (!same) {
            free(dist);
            return true;  // 需要分割
        }
    }

    free(dist);
    return false;
}
