/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 445
 * Language: c
 * Block ID: 3ad467a8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 旧的数据库接口 (Adaptee)
typedef struct {
    void (*connect_legacy)(const char* host);
    void (*query_legacy)(const char* sql);
} LegacyDatabase;

// 新的统一接口 (Target)
typedef struct {
    int (*connect)(void* self, const char* conn_str);
    int (*execute)(void* self, const char* sql);
    void (*close)(void* self);
} DatabaseInterface;

// 适配器实现
typedef struct {
    DatabaseInterface interface;
    LegacyDatabase* adaptee;
    char host[64];
} DatabaseAdapter;

static int adapter_connect(void* self, const char* conn_str) {
    DatabaseAdapter* adapter = self;
    // 解析 conn_str 提取 host
    adapter->adaptee->connect_legacy(conn_str);
    return 0;
}

static int adapter_execute(void* self, const char* sql) {
    DatabaseAdapter* adapter = self;
    adapter->adaptee->query_legacy(sql);
    return 0;
}

static void adapter_close(void* self) {
    (void)self;
    // 旧接口没有 close，可能什么都不做
}

DatabaseAdapter* database_adapter_create(LegacyDatabase* legacy) {
    DatabaseAdapter* a = malloc(sizeof(DatabaseAdapter));
    a->interface.connect = adapter_connect;
    a->interface.execute = adapter_execute;
    a->interface.close = adapter_close;
    a->adaptee = legacy;
    return a;
}
