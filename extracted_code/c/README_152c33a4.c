/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 1046
 * Language: c
 * Block ID: 152c33a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 事件溯源存储

typedef struct {
    char aggregate_id[64];
    char aggregate_type[64];
    int version;
    event_t* events;
    size_t event_count;
} event_stream_t;

typedef struct {
    // 事件存储
    int (*append)(const char* stream_id, event_t* events, size_t count);
    event_stream_t* (*read)(const char* stream_id, int from_version);

    // 快照
    int (*save_snapshot)(const char* stream_id, void* snapshot, int version);
    void* (*load_snapshot)(const char* stream_id, int* version);
} event_store_t;

// 聚合根
typedef struct {
    char id[64];
    int version;
    void* state;
    event_t* uncommitted_events;
    size_t uncommitted_count;
} aggregate_root_t;

// 应用事件到聚合
void aggregate_apply(aggregate_root_t* aggregate, event_t* event);

// 加载聚合
aggregate_root_t* aggregate_load(event_store_t* store, const char* id,
                                  void (*apply_fn)(void*, event_t*)) {
    // 尝试加载快照
    int snapshot_version = 0;
    void* snapshot = store->load_snapshot(id, &snapshot_version);

    aggregate_root_t* aggregate = calloc(1, sizeof(aggregate_root_t));
    strcpy(aggregate->id, id);
    aggregate->version = snapshot_version;
    aggregate->state = snapshot ? snapshot : create_initial_state();

    // 加载快照后的事件
    event_stream_t* stream = store->read(id, snapshot_version + 1);

    // 重放事件
    for (size_t i = 0; i < stream->event_count; i++) {
        apply_fn(aggregate->state, &stream->events[i]);
        aggregate->version++;
    }

    return aggregate;
}

// 保存聚合
void aggregate_save(event_store_t* store, aggregate_root_t* aggregate) {
    if (aggregate->uncommitted_count == 0) return;

    // 乐观并发控制
    for (size_t i = 0; i < aggregate->uncommitted_count; i++) {
        aggregate->uncommitted_events[i].expected_version = aggregate->version + i;
    }

    // 追加到事件存储
    int result = store->append(aggregate->id, aggregate->uncommitted_events,
                               aggregate->uncommitted_count);

    if (result == CONCURRENCY_CONFLICT) {
        // 处理冲突
        handle_concurrency_conflict(aggregate);
    } else {
        aggregate->version += aggregate->uncommitted_count;
        aggregate->uncommitted_count = 0;

        // 可选：创建快照
        if (aggregate->version % SNAPSHOT_FREQUENCY == 0) {
            store->save_snapshot(aggregate->id, aggregate->state, aggregate->version);
        }
    }
}
