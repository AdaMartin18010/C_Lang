/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\02_DO_178C.md
 * Line: 722
 * Language: c
 * Block ID: 9c4f8434
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file test_case_manager.c
 * @brief DO-178C测试用例管理实现
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 测试类型定义 */
typedef enum {
    TEST_TYPE_REQUIREMENT = 0,      /* 需求测试 */
    TEST_TYPE_ROBUSTNESS = 1,       /* 健壮性测试 */
    TEST_TYPE_STRESS = 2,           /* 压力测试 */
    TEST_TYPE_BOUNDARY = 3,         /* 边界测试 */
    TEST_TYPE_ERROR = 4             /* 错误处理测试 */
} TestType_t;

/* 测试状态 */
typedef enum {
    TEST_STATUS_NOT_RUN = 0,
    TEST_STATUS_PASSED = 1,
    TEST_STATUS_FAILED = 2,
    TEST_STATUS_BLOCKED = 3,
    TEST_STATUS_NOT_APPLICABLE = 4
} TestStatus_t;

#define TEST_ID_MAX_LEN             (32U)
#define TEST_DESC_MAX_LEN           (512U)
#define MAX_TEST_PROCEDURES         (256U)
#define MAX_TEST_RESULTS            (1024U)

/* 测试过程 */
typedef struct {
    char id[TEST_ID_MAX_LEN];
    char description[TEST_DESC_MAX_LEN];
    char requirement_ids[8][REQ_ID_MAX_LEN];
    uint32_t req_count;
    TestType_t type;
    void (*setup)(void);
    bool (*execute)(void);
    void (*teardown)(void);
    bool (*expected_result_check)(void);
} TestProcedure_t;

/* 测试结果 */
typedef struct {
    char test_id[TEST_ID_MAX_LEN];
    TestStatus_t status;
    uint32_t execution_time_ms;
    char failure_reason[256];
    uint32_t timestamp;
    char executed_by[64];
} TestResult_t;

/* 测试管理器 */
typedef struct {
    TestProcedure_t procedures[MAX_TEST_PROCEDURES];
    uint32_t procedure_count;
    TestResult_t results[MAX_TEST_RESULTS];
    uint32_t result_count;
    bool is_initialized;
} TestManager_t;

static TestManager_t g_test_mgr;

/**
 * @brief 初始化测试管理器
 */
bool TestManager_Init(void) {
    (void)memset(&g_test_mgr, 0, sizeof(g_test_mgr));
    g_test_mgr.is_initialized = true;
    return true;
}

/**
 * @brief 注册测试过程
 */
bool TestManager_RegisterProcedure(const char *id, const char *description,
                                    TestType_t type) {
    TestProcedure_t *proc;

    if ((!g_test_mgr.is_initialized) ||
        (g_test_mgr.procedure_count >= MAX_TEST_PROCEDURES)) {
        return false;
    }

    proc = &g_test_mgr.procedures[g_test_mgr.procedure_count];

    (void)strncpy(proc->id, id, TEST_ID_MAX_LEN - 1U);
    proc->id[TEST_ID_MAX_LEN - 1U] = '\0';

    (void)strncpy(proc->description, description, TEST_DESC_MAX_LEN - 1U);
    proc->description[TEST_DESC_MAX_LEN - 1U] = '\0';

    proc->type = type;
    g_test_mgr.procedure_count++;

    return true;
}

/**
 * @brief 链接需求到测试
 */
bool TestManager_LinkRequirement(const char *test_id, const char *req_id) {
    uint32_t i;

    for (i = 0U; i < g_test_mgr.procedure_count; i++) {
        if (0 == strncmp(g_test_mgr.procedures[i].id, test_id, TEST_ID_MAX_LEN)) {
            TestProcedure_t *proc = &g_test_mgr.procedures[i];

            if (proc->req_count < 8U) {
                (void)strncpy(proc->requirement_ids[proc->req_count],
                               req_id, REQ_ID_MAX_LEN - 1U);
                proc->requirement_ids[proc->req_count][REQ_ID_MAX_LEN - 1U] = '\0';
                proc->req_count++;
                return true;
            }
            break;
        }
    }
    return false;
}

/**
 * @brief 执行测试
 */
bool TestManager_Execute(const char *test_id, const char *executor) {
    TestProcedure_t *proc = NULL;
    TestResult_t *result;
    uint32_t i;
    bool passed = false;

    /* 查找测试过程 */
    for (i = 0U; i < g_test_mgr.procedure_count; i++) {
        if (0 == strncmp(g_test_mgr.procedures[i].id, test_id, TEST_ID_MAX_LEN)) {
            proc = &g_test_mgr.procedures[i];
            break;
        }
    }

    if (NULL == proc) {
        return false;
    }

    /* 记录结果 */
    if (g_test_mgr.result_count >= MAX_TEST_RESULTS) {
        return false;
    }

    result = &g_test_mgr.results[g_test_mgr.result_count];
    (void)strncpy(result->test_id, test_id, TEST_ID_MAX_LEN - 1U);
    result->test_id[TEST_ID_MAX_LEN - 1U] = '\0';

    (void)strncpy(result->executed_by, executor, 63U);
    result->executed_by[63U] = '\0';

    /* 执行测试 */
    if (NULL != proc->setup) {
        proc->setup();
    }

    if (NULL != proc->execute) {
        passed = proc->execute();
    }

    if (NULL != proc->teardown) {
        proc->teardown();
    }

    /* 验证预期结果 */
    if ((NULL != proc->expected_result_check) && passed) {
        passed = proc->expected_result_check();
    }

    result->status = passed ? TEST_STATUS_PASSED : TEST_STATUS_FAILED;
    g_test_mgr.result_count++;

    return passed;
}

/**
 * @brief 生成测试摘要报告
 */
void TestManager_GenerateSummary(uint32_t *total, uint32_t *passed,
                                  uint32_t *failed) {
    uint32_t i;
    uint32_t p = 0U, f = 0U;

    for (i = 0U; i < g_test_mgr.result_count; i++) {
        if (TEST_STATUS_PASSED == g_test_mgr.results[i].status) {
            p++;
        } else if (TEST_STATUS_FAILED == g_test_mgr.results[i].status) {
            f++;
        }
    }

    if (NULL != total) {
        *total = g_test_mgr.result_count;
    }
    if (NULL != passed) {
        *passed = p;
    }
    if (NULL != failed) {
        *failed = f;
    }
}
