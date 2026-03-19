/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\README.md
 * Line: 425
 * Language: c
 * Block ID: 83b929dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* learning_recommendation.h */
#ifndef LEARNING_RECOMMENDATION_H
#define LEARNING_RECOMMENDATION_H

#include "knowledge_graph.h"

/* 学习者画像 */
typedef struct {
    uint32_t *mastered_concepts;    /* 已掌握概念ID */
    uint32_t mastered_count;

    uint32_t *learning_concepts;    /* 正在学习的概念 */
    uint32_t learning_count;

    float difficulty_preference;    /* 难度偏好 0-1 */
    uint32_t available_hours;       /* 可用学习时间 */

    const char *goal;               /* 学习目标 */
} learner_profile_t;

/* 推荐项 */
typedef struct {
    entity_t *concept;
    float relevance_score;          /* 相关性分数 */
    float difficulty_match;         /* 难度匹配度 */
    float priority_score;           /* 综合优先级 */
    const char *reason;             /* 推荐理由 */
} recommendation_t;

/* 推荐算法 */
recommendation_t *recommend_next_concepts(knowledge_graph_t *kg,
                                           learner_profile_t *profile,
                                           uint32_t *count);

/* 个性化学习路径 */
entity_t **generate_personalized_path(knowledge_graph_t *kg,
                                       learner_profile_t *profile,
                                       entity_t *goal_concept,
                                       uint32_t *path_length);

/* 基于协同过滤的推荐 */
recommendation_t *recommend_by_similar_learners(knowledge_graph_t *kg,
                                                 learner_profile_t *profile,
                                                 learner_profile_t **other_profiles,
                                                 uint32_t profile_count,
                                                 uint32_t *count);

#endif /* LEARNING_RECOMMENDATION_H */
