/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1440
 * Language: c
 * Block ID: 1ba716fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 断言: 调试时捕获非法状态
// 发布时转为错误记录或安全状态
// ============================================================

#ifdef DEBUG
    #define ASSERT(cond) do { \
        if (!(cond)) { \
            __disable_irq(); \
            fault_handler(__FILE__, __LINE__, #cond); \
        } \
    } while(0)
#else
    #define ASSERT(cond) do { \
        if (!(cond)) { \
            log_fault(FAULT_ASSERT, __LINE__); \
            enter_safe_state(); \
        } \
    } while(0)
#endif

// 故障类型
typedef enum {
    FAULT_ASSERT = 0,
    FAULT_NULL_PTR,
    FAULT_STACK_OVERFLOW,
    FAULT_WDT_TIMEOUT,
    FAULT_BUS_ERROR,
    FAULT_COUNT
} FaultType;

// 故障记录
typedef struct {
    FaultType type;
    uint32_t timestamp;
    uint32_t param;
    const char *file;
    uint32_t line;
} FaultRecord;

#define FAULT_LOG_SIZE 10
static FaultRecord fault_log[FAULT_LOG_SIZE];
static uint8_t fault_count = 0;

void log_fault(FaultType type, uint32_t param) {
    if (fault_count < FAULT_LOG_SIZE) {
        fault_log[fault_count].type = type;
        fault_log[fault_count].timestamp = get_tick();
        fault_log[fault_count].param = param;
        fault_count++;
    }
}

// 进入安全状态
void enter_safe_state(void) {
    // 关闭所有输出
    disable_all_outputs();

    // 记录状态到非易失存储
    save_fault_log();

    // 尝试恢复或等待复位
    if (can_recover()) {
        attempt_recovery();
    } else {
        // 触发复位
        NVIC_SystemReset();
    }
}

// 使用断言检查关键条件
void motor_set_speed(Motor *m, int16_t speed) {
    ASSERT(m != NULL);                    // 空指针检查
    ASSERT(m->state != MOTOR_FAULT);      // 状态检查
    ASSERT(speed >= -MAX_SPEED && speed <= MAX_SPEED);  // 范围检查

    m->target_speed = speed;
}
