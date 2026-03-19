/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 1095
 * Language: c
 * Block ID: 759d541a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 4: 链式 if 声明
 * 演示优先级处理和状态机实现
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// 资源优先级枚举
typedef enum {
    PRIORITY_CRITICAL,
    PRIORITY_HIGH,
    PRIORITY_NORMAL,
    PRIORITY_LOW,
    PRIORITY_COUNT
} Priority;

const char* priority_name(Priority p) {
    static const char* names[] = {
        "CRITICAL", "HIGH", "NORMAL", "LOW"
    };
    return (p >= 0 && p < PRIORITY_COUNT) ? names[p] : "UNKNOWN";
}

// 模拟资源
typedef struct {
    int id;
    bool available;
    int data;
} Resource;

Resource resources[PRIORITY_COUNT] = {
    {0, false, 100},  // CRITICAL
    {1, true,  80},   // HIGH
    {2, false, 60},   // NORMAL
    {3, true,  40}    // LOW
};

Resource* try_acquire_resource(Priority p) {
    if (p >= 0 && p < PRIORITY_COUNT && resources[p].available) {
        resources[p].available = false;
        return &resources[p];
    }
    return NULL;
}

void release_resource(Resource* r) {
    if (r != NULL) {
        r->available = true;
        printf("Released resource %d (%s priority)\n",
               r->id, priority_name(r->id));
    }
}

// 链式 if 声明：按优先级获取资源
Resource* acquire_with_fallback(void) {
    // 尝试按优先级获取资源
    if (Resource* r = try_acquire_resource(PRIORITY_CRITICAL); r != NULL) {
        printf("Acquired CRITICAL resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_HIGH); r != NULL) {
        printf("Acquired HIGH resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_NORMAL); r != NULL) {
        printf("Acquired NORMAL resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else if (Resource* r = try_acquire_resource(PRIORITY_LOW); r != NULL) {
        printf("Acquired LOW resource (id=%d, data=%d)\n", r->id, r->data);
        return r;
    } else {
        printf("No resources available!\n");
        return NULL;
    }
}

// 状态机实现：解析器状态
typedef enum {
    STATE_IDLE,
    STATE_READING,
    STATE_PROCESSING,
    STATE_WRITING,
    STATE_DONE,
    STATE_ERROR
} ParserState;

typedef struct {
    ParserState state;
    int data;
    char buffer[256];
} Parser;

// 使用链式 if 声明实现状态机
ParserState run_state_machine(Parser* parser) {
    if (parser->state == STATE_IDLE) {
        printf("State: IDLE -> READING\n");
        parser->state = STATE_READING;
    } else if (parser->state == STATE_READING) {
        printf("State: READING -> PROCESSING\n");
        parser->state = STATE_PROCESSING;
    } else if (parser->state == STATE_PROCESSING) {
        if (parser->data < 0) {
            printf("State: PROCESSING -> ERROR (invalid data)\n");
            parser->state = STATE_ERROR;
        } else {
            printf("State: PROCESSING -> WRITING\n");
            parser->state = STATE_WRITING;
        }
    } else if (parser->state == STATE_WRITING) {
        printf("State: WRITING -> DONE\n");
        parser->state = STATE_DONE;
    }
    return parser->state;
}

// 另一种状态机：使用 if 声明管理状态上下文
bool process_request(int request_id) {
    // 获取请求信息
    if (Request* req = get_request(request_id); req == NULL) {
        printf("Request %d not found\n", request_id);
        return false;
    } else {
        // 验证请求
        if (ValidationResult* vr = validate_request(req); !vr->valid) {
            printf("Request %d invalid: %s\n", request_id, vr->error);
            return false;
        } else {
            // 处理请求
            if (ProcessResult* pr = process_request_data(req); pr->error) {
                printf("Request %d processing failed: %s\n",
                       request_id, pr->error_msg);
                return false;
            } else {
                // 完成
                printf("Request %d processed successfully\n", request_id);
                return true;
            }
        }
    }
}

// 模拟结构（简化）
typedef struct Request { int id; bool valid; } Request;
typedef struct ValidationResult { bool valid; const char* error; } ValidationResult;
typedef struct ProcessResult { bool error; const char* error_msg; } ProcessResult;

Request* get_request(int id) {
    static Request r = {1, true};
    return (id == 1) ? &r : NULL;
}

ValidationResult* validate_request(Request* r) {
    static ValidationResult vr = {true, NULL};
    (void)r;
    return &vr;
}

ProcessResult* process_request_data(Request* r) {
    static ProcessResult pr = {false, NULL};
    (void)r;
    return &pr;
}

int main(void) {
    printf("=== Chained if Declaration Demo ===\n\n");

    printf("1. Resource acquisition with fallback:\n");
    Resource* r1 = acquire_with_fallback();
    Resource* r2 = acquire_with_fallback();
    Resource* r3 = acquire_with_fallback();

    printf("\n2. State machine:\n");
    Parser parser = {STATE_IDLE, 100, ""};
    while (run_state_machine(&parser) != STATE_DONE &&
           parser.state != STATE_ERROR) {
        // 继续状态转换
    }

    printf("\n3. Request processing:\n");
    process_request(1);
    process_request(999);

    // 释放资源
    if (r1) release_resource(r1);
    if (r2) release_resource(r2);
    if (r3) release_resource(r3);

    return 0;
}
