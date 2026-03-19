/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 413
 * Language: c
 * Block ID: e20d994d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* radar_analysis.h */
#ifndef RADAR_ANALYSIS_H
#define RADAR_ANALYSIS_H

#include "skill_matrix.h"

/* 雷达图数据 */
typedef struct {
    const char *dimension_name;
    float current_score;    /* 0-100 */
    float target_score;
    float weight;           /* 维度权重 */
} radar_dimension_t;

/* 计算各维度得分 */
void calculate_dimension_scores(skill_matrix_t *matrix,
                                 radar_dimension_t *dimensions,
                                 uint32_t dim_count);

/* 生成SVG雷达图 */
int generate_radar_svg(radar_dimension_t *dimensions,
                       uint32_t dim_count,
                       const char *filename);

/* 能力差距分析 */
typedef struct {
    const char *skill_name;
    float gap_score;        /* 目标-当前差距 */
    float impact;           /* 影响度 */
    float effort;           /* 所需投入 */
    float roi;              /* 投资回报率 (impact/effort) */
} gap_analysis_item_t;

gap_analysis_item_t *analyze_gaps(skill_matrix_t *matrix,
                                   uint32_t *count);

#endif /* RADAR_ANALYSIS_H */
