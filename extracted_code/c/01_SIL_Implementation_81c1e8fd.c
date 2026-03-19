/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\IEC_61508\01_SIL_Implementation.md
 * Line: 49
 * Language: c
 * Block ID: 81c1e8fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SIL确定流程
 */

typedef struct {
    int severity;      /* 后果严重程度 1-4 */
    int exposure;      /* 暴露频率 1-4 */
    int avoidance;     /* 避免可能性 1-4 */
} RiskParameters;

typedef enum {
    SIL_NONE = 0,  /* QM */
    SIL_1 = 1,
    SIL_2 = 2,
    SIL_3 = 3,
    SIL_4 = 4
} SIL_Level;

/* 风险矩阵简化示例 */
SIL_Level determine_sil(const RiskParameters *risk)
{
    int risk_score = risk->severity + risk->exposure + risk->avoidance;

    if (risk_score >= 10) return SIL_4;
    if (risk_score >= 8)  return SIL_3;
    if (risk_score >= 6)  return SIL_2;
    if (risk_score >= 4)  return SIL_1;
    return SIL_NONE;
}
