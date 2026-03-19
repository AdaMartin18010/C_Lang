/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 192
 * Language: c
 * Block ID: 951b2147
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 代码审查度量指标结构
struct review_metrics {
    int lines_reviewed;          // 审查代码行数
    int issues_found[6];         // 各级别问题数量
    int review_duration_minutes; // 审查耗时
    int defect_density;          // 缺陷密度（每千行）
    char *reviewer;              // 审查者
    char *author;                // 作者
};
