/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 539
 * Language: c
 * Block ID: 3048d785
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 帮助理解程序状态的工具

// 内存变化追踪
void track_memory_changes(void *addr, size_t size, const char *name) {
    static uint8_t snapshot[1024];
    memcpy(snapshot, addr, size);

    // 后续检查变化
    for (size_t i = 0; i < size; i++) {
        if (snapshot[i] != ((uint8_t *)addr)[i]) {
            printf("%s[%zu]: 0x%02x -> 0x%02x\n",
                   name, i, snapshot[i], ((uint8_t *)addr)[i]);
        }
    }
}

// 执行路径记录
#define MAX_TRACE 10000
typedef struct {
    const char *func;
    const char *file;
    int line;
    double timestamp;
} TraceEntry;

TraceEntry execution_trace[MAX_TRACE];
int trace_index = 0;

#define TRACE() do { \
    if (trace_index < MAX_TRACE) { \
        execution_trace[trace_index++] = (TraceEntry){ \
            __func__, __FILE__, __LINE__, get_timestamp() \
        }; \
    } \
} while(0)
