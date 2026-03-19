/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\README.md
 * Line: 944
 * Language: c
 * Block ID: 54d09141
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* misra_examples.c - 展示 MISRA C 合规代码 */

/* ============================================================================
 * Rule 8.9: 对象定义应在块作用域内，如果仅在该块内访问
 * Rule 8.4: 外部链接对象的兼容声明应在同一文件中
 * ============================================================================ */

/* 全局变量 - 仅在本文件使用，声明为 static */
static uint32_t g_abs_cycle_counter;  /* 符合 Rule 8.8 */

/* 外部接口变量 - 在头文件中声明 */
extern volatile uint32_t g_system_tick;  /* 在头文件中声明 */

/* ============================================================================
 * Rule 17.8: 函数参数不应被修改
 * ============================================================================ */

/* 不好的做法: 修改输入参数 */
void bad_example(uint32_t speed) {
    speed = speed * 1000 / 3600;  /* 违反 Rule 17.8 */
}

/* 好的做法: 使用局部变量 */
uint32_t good_example(uint32_t speed_kmh) {
    uint32_t speed_ms;  /* 局部变量 */

    /* Rule 10.8: 复合表达式的值不应转换为不同的底层类型 */
    speed_ms = (uint32_t)((uint64_t)speed_kmh * 1000U / 3600U);

    return speed_ms;
}

/* ============================================================================
 * Rule 14.4: for 循环的循环计数器不应在循环体中修改
 * Rule 14.5: for 循环应只包含一个循环计数器
 * ============================================================================ */

/* 不好的做法 */
void bad_loop(void) {
    int i, j;
    for (i = 0, j = 10; i < 10; i++, j--) {  /* 违反 Rule 14.5 */
        if (condition) {
            i = 5;  /* 违反 Rule 14.4 */
        }
    }
}

/* 好的做法 */
void good_loop(void) {
    uint32_t i;

    for (i = 0U; i < 10U; i++) {  /* 符合规则 */
        /* 循环体不修改 i */
    }

    /* 需要额外控制时使用 while */
    i = 0U;
    while (i < 10U) {
        if (condition) {
            i = 5U;  /* 允许在 while 中修改 */
        }
        i++;
    }
}

/* ============================================================================
 * Rule 11.3: 指向 void 的指针不应转换为指向对象的指针
 * Rule 11.5: 指向 void 的指针不应转换为指向对象的指针
 * ============================================================================ */

/* 类型安全的内存访问 */
typedef union {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t u8[4];
} register_access_t;

/* 不好的做法 */
void bad_pointer_cast(void* ptr) {
    uint32_t* reg = (uint32_t*)ptr;  /* 违反 Rule 11.5 */
    *reg = 0x12345678U;
}

/* 好的做法 - 使用联合体 */
void good_register_write(volatile register_access_t* reg, uint32_t value) {
    if (reg != NULL) {  /* Rule 17.2: 空指针检查 */
        reg->u32 = value;
    }
}

/* ============================================================================
 * Rule 15.5: 每个 switch 语句应有且只有一个 default 标签
 * Rule 16.1: switch 标签不应嵌套
 * Rule 16.3: 每个 switch 子句都应有无条件 break
 * ============================================================================ */

typedef enum {
    STATE_INIT,
    STATE_RUN,
    STATE_STOP,
    STATE_ERROR
} system_state_t;

void state_machine_handler(system_state_t state) {
    switch (state) {  /* Rule 15.5: 有 default */
        case STATE_INIT:
            init_system();
            break;  /* Rule 16.3: 无条件 break */

        case STATE_RUN:
            run_system();
            break;

        case STATE_STOP:
            stop_system();
            break;

        case STATE_ERROR:
            handle_error();
            break;

        default:  /* 必需 */
            /* 记录非法状态 */
            log_error(ERROR_INVALID_STATE);
            enter_safe_state();
            break;
    }
}

/* ============================================================================
 * Rule 17.7: 非 void 函数的返回值应被使用
 * Rule 17.4: 所有退出路径都应有 return 语句
 * ============================================================================ */

/* 安全关键函数 - 必须检查返回值 */
typedef enum {
    ERR_OK = 0,
    ERR_NULL_PTR,
    ERR_OUT_OF_RANGE,
    ERR_TIMEOUT
} error_code_t;

/* 不好的做法 */
void bad_function_call(void) {
    abs_controller_update(NULL, 10U);  /* 返回值被忽略 */
}

/* 好的做法 */
void good_function_call(abs_controller_t* abs) {
    error_code_t result;

    result = abs_controller_update(abs, 10U);
    if (result != ERR_OK) {
        error_handler(result);
    }
}

/* ============================================================================
 * Rule 21.3: 不应使用堆内存分配
 * Rule 21.6: 不应使用标准输入输出库
 * ============================================================================ */

/* 静态分配代替动态分配 */
#define MAX_ABS_CONTROLLERS 1U

static abs_controller_t g_abs_controllers[MAX_ABS_CONTROLLERS];
static bool g_abs_allocated[MAX_ABS_CONTROLLERS];

abs_controller_t* abs_controller_create(void) {
    uint32_t i;

    for (i = 0U; i < MAX_ABS_CONTROLLERS; i++) {
        if (!g_abs_allocated[i]) {
            g_abs_allocated[i] = true;
            /* 初始化 */
            memset(&g_abs_controllers[i], 0, sizeof(abs_controller_t));
            return &g_abs_controllers[i];
        }
    }

    return NULL;  /* 资源耗尽 */
}

/* 使用安全日志代替 printf */
void safety_log_error(error_code_t error, uint32_t param) {
    log_entry_t entry;

    entry.timestamp = get_system_tick();
    entry.error_code = error;
    entry.parameter = param;

    log_write(&entry);  /* 写入非易失性存储或 CAN */
}
