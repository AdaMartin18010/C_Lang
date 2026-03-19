/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\02_DO_178C.md
 * Line: 237
 * Language: c
 * Block ID: c4c89f63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file do178c_requirement_trace.c
 * @brief DO-178C需求追踪实现
 * @software_level A
 * @verification_method Review, Test
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 需求状态定义 */
typedef enum {
    REQ_STATUS_ALLOCATED = 0,       /* 已分配 */
    REQ_STATUS_IMPLEMENTED = 1,     /* 已实现 */
    REQ_STATUS_VERIFIED = 2,        /* 已验证 */
    REQ_STATUS_VALIDATED = 3        /* 已确认 */
} RequirementStatus_t;

/* 需求类型定义 */
typedef enum {
    REQ_TYPE_HLR = 0,               /* 高层需求 */
    REQ_TYPE_LLR = 1,               /* 低层需求 */
    REQ_TYPE_CODE = 2,              /* 代码需求 */
    REQ_TYPE_TEST = 3               /* 测试需求 */
} RequirementType_t;

/* 需求ID最大长度 */
#define REQ_ID_MAX_LEN              (32U)
#define REQ_DESC_MAX_LEN            (256U)
#define MAX_TRACE_LINKS             (16U)
#define MAX_REQUIREMENTS            (1024U)

/**
 * @brief 需求结构体
 * @requirement DO-178C Table A-1 Objectives 1
 */
typedef struct {
    char id[REQ_ID_MAX_LEN];
    char description[REQ_DESC_MAX_LEN];
    RequirementType_t type;
    RequirementStatus_t status;
    uint32_t parent_count;
    uint32_t child_count;
    char parent_ids[MAX_TRACE_LINKS][REQ_ID_MAX_LEN];
    char child_ids[MAX_TRACE_LINKS][REQ_ID_MAX_LEN];
    uint32_t verification_criteria;
    bool is_safety_related;
} Requirement_t;

/**
 * @brief 需求追踪数据库
 */
typedef struct {
    Requirement_t requirements[MAX_REQUIREMENTS];
    uint32_t count;
    bool is_initialized;
} RequirementDatabase_t;

static RequirementDatabase_t g_req_db;

/**
 * @brief 初始化需求数据库
 * @requirement SYS-REQ-001
 * @design DS-001
 * @return true: 成功, false: 失败
 */
bool RequirementDB_Init(void) {
    (void)memset(&g_req_db, 0, sizeof(g_req_db));
    g_req_db.is_initialized = true;
    return true;
}

/**
 * @brief 添加需求
 * @requirement SYS-REQ-002
 * @param id 需求ID
 * @param description 需求描述
 * @param type 需求类型
 * @return true: 成功, false: 失败
 */
bool Requirement_Add(const char *id, const char *description,
                      RequirementType_t type) {
    Requirement_t *req;

    if ((!g_req_db.is_initialized) ||
        (g_req_db.count >= MAX_REQUIREMENTS) ||
        (NULL == id) || (NULL == description)) {
        return false;
    }

    req = &g_req_db.requirements[g_req_db.count];

    /* 复制ID和描述 */
    (void)strncpy(req->id, id, REQ_ID_MAX_LEN - 1U);
    req->id[REQ_ID_MAX_LEN - 1U] = '\0';

    (void)strncpy(req->description, description, REQ_DESC_MAX_LEN - 1U);
    req->description[REQ_DESC_MAX_LEN - 1U] = '\0';

    req->type = type;
    req->status = REQ_STATUS_ALLOCATED;
    req->parent_count = 0U;
    req->child_count = 0U;
    req->is_safety_related = false;

    g_req_db.count++;
    return true;
}

/**
 * @brief 建立双向追踪链接
 * @requirement SYS-REQ-003
 * @param parent_id 父需求ID
 * @param child_id 子需求ID
 * @return true: 成功, false: 失败
 */
bool Requirement_AddTraceLink(const char *parent_id, const char *child_id) {
    Requirement_t *parent = NULL;
    Requirement_t *child = NULL;
    uint32_t i;

    /* 查找父需求和子需求 */
    for (i = 0U; i < g_req_db.count; i++) {
        if (0 == strncmp(g_req_db.requirements[i].id, parent_id, REQ_ID_MAX_LEN)) {
            parent = &g_req_db.requirements[i];
        }
        if (0 == strncmp(g_req_db.requirements[i].id, child_id, REQ_ID_MAX_LEN)) {
            child = &g_req_db.requirements[i];
        }
    }

    if ((NULL == parent) || (NULL == child)) {
        return false;
    }

    /* 添加子链接到父 */
    if (parent->child_count < MAX_TRACE_LINKS) {
        (void)strncpy(parent->child_ids[parent->child_count],
                       child_id, REQ_ID_MAX_LEN - 1U);
        parent->child_ids[parent->child_count][REQ_ID_MAX_LEN - 1U] = '\0';
        parent->child_count++;
    }

    /* 添加父链接到子 */
    if (child->parent_count < MAX_TRACE_LINKS) {
        (void)strncpy(child->parent_ids[child->parent_count],
                       parent_id, REQ_ID_MAX_LEN - 1U);
        child->parent_ids[child->parent_count][REQ_ID_MAX_LEN - 1U] = '\0';
        child->parent_count++;
    }

    return true;
}

/**
 * @brief 更新需求状态
 * @requirement SYS-REQ-004
 */
bool Requirement_UpdateStatus(const char *id, RequirementStatus_t new_status) {
    uint32_t i;

    for (i = 0U; i < g_req_db.count; i++) {
        if (0 == strncmp(g_req_db.requirements[i].id, id, REQ_ID_MAX_LEN)) {
            /* 状态转换验证 */
            if (new_status < g_req_db.requirements[i].status) {
                return false;  /* 不允许状态回退 */
            }
            g_req_db.requirements[i].status = new_status;
            return true;
        }
    }
    return false;
}

/**
 * @brief 双向追踪性分析
 * @requirement SYS-REQ-005
 * @param orphan_count 输出孤儿需求数量
 * @param missing_count 输出缺失链接数量
 * @return 分析状态
 */
bool Requirement_AnalyzeTraceability(uint32_t *orphan_count,
                                      uint32_t *missing_count) {
    uint32_t i;
    uint32_t orphans = 0U;
    uint32_t missing = 0U;

    if (!g_req_db.is_initialized) {
        return false;
    }

    for (i = 0U; i < g_req_db.count; i++) {
        Requirement_t *req = &g_req_db.requirements[i];

        /* HLR应该有子链接(LLR或测试) */
        if ((REQ_TYPE_HLR == req->type) && (0U == req->child_count)) {
            orphans++;
        }

        /* LLR应该有父链接(HLR)和子链接(代码) */
        if (REQ_TYPE_LLR == req->type) {
            if (0U == req->parent_count) {
                missing++;
            }
            if (0U == req->child_count) {
                orphans++;
            }
        }

        /* 代码需求应该有父链接(LLR) */
        if ((REQ_TYPE_CODE == req->type) && (0U == req->parent_count)) {
            missing++;
        }
    }

    if (NULL != orphan_count) {
        *orphan_count = orphans;
    }
    if (NULL != missing_count) {
        *missing_count = missing;
    }

    return true;
}
