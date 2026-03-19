/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\README.md
 * Line: 661
 * Language: c
 * Block ID: 8c8cef81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* progress_tracking.h */
#ifndef PROGRESS_TRACKING_H
#define PROGRESS_TRACKING_H

#include <stdint.h>
#include <time.h>

/* 学习记录 */
typedef struct {
    uint32_t skill_id;
    time_t start_time;
    time_t completion_time;
    uint32_t actual_hours;
    skill_level_t level_achieved;
    uint32_t assessment_score;  /* 测试分数 */
    const char *notes;
} learning_record_t;

/* 进度统计 */
typedef struct {
    uint32_t total_skills;
    uint32_t completed_skills;
    uint32_t in_progress_skills;
    uint32_t total_hours_planned;
    uint32_t total_hours_actual;
    float completion_rate;
    float average_score;
    time_t start_date;
    time_t projected_completion;
} progress_stats_t;

/* 进度追踪器 */
typedef struct {
    learning_record_t *records;
    uint32_t record_count;
    progress_stats_t stats;
} progress_tracker_t;

/* API */
progress_tracker_t *pt_create(void);
void pt_record_start(progress_tracker_t *pt, uint32_t skill_id);
void pt_record_completion(progress_tracker_t *pt, uint32_t skill_id,
                          skill_level_t achieved, uint32_t score);
progress_stats_t pt_calculate_stats(progress_tracker_t *pt);

/* 生成进度报告 */
int pt_generate_report(progress_tracker_t *pt, char *buffer, size_t size);

#endif /* PROGRESS_TRACKING_H */
