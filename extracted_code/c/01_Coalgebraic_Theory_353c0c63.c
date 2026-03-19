/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 320
 * Language: c
 * Block ID: 353c0c63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 离散时间马尔可夫链（DTMC）作为余代数
#include <stdlib.h>
#include <math.h>

typedef struct {
    int target_state;
    double probability;
} ProbTransition;

typedef struct {
    int state_id;
    ProbTransition *transitions;
    int num_transitions;
} ProbState;

// 验证概率互模拟
bool prob_bisimilar(ProbState *s1, ProbState *s2, double epsilon) {
    if (s1->num_transitions != s2->num_transitions) {
        return false;
    }

    for (int i = 0; i < s1->num_transitions; i++) {
        bool found = false;
        for (int j = 0; j < s2->num_transitions; j++) {
            if (s1->transitions[i].target_state ==
                s2->transitions[j].target_state &&
                fabs(s1->transitions[i].probability -
                     s2->transitions[j].probability) < epsilon) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }

    return true;
}
