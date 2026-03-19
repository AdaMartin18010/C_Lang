/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 690
 * Language: c
 * Block ID: 41ca36d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 重构后：通用配置系统

// 1. 统一的配置项抽象
typedef enum {
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_ARRAY,
    CONFIG_TYPE_OBJECT
} config_value_type_t;

typedef struct config_value {
    config_value_type_t type;
    union {
        char* string_val;
        int64_t int_val;
        bool bool_val;
        double float_val;
        struct { struct config_value* items; size_t count; } array_val;
        struct { char** keys; struct config_value* values; size_t count; } object_val;
    };
} config_value_t;

// 2. 解析器接口
typedef struct {
    const char* name;
    const char** extensions;  // 支持的文件扩展名
    config_value_t* (*parse)(const char* content, size_t len, char** error);
    void (*free)(config_value_t* value);
} config_parser_t;

// 3. 解析器注册表
static config_parser_t* parsers[16];
static int parser_count = 0;

void config_register_parser(config_parser_t* parser) {
    if (parser_count < 16) {
        parsers[parser_count++] = parser;
    }
}

// 4. 统一加载函数
config_value_t* config_load(const char* filename, char** error) {
    // 读取文件
    FILE* f = fopen(filename, "rb");
    if (!f) {
        if (error) *error = strdup("Cannot open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* content = malloc(len + 1);
    fread(content, 1, len, f);
    content[len] = '\0';
    fclose(f);

    // 根据扩展名选择解析器
    const char* ext = strrchr(filename, '.');
    config_parser_t* parser = NULL;

    for (int i = 0; i < parser_count; i++) {
        for (const char** p = parsers[i]->extensions; *p; p++) {
            if (strcasecmp(ext, *p) == 0) {
                parser = parsers[i];
                break;
            }
        }
        if (parser) break;
    }

    if (!parser) {
        free(content);
        if (error) *error = strdup("No parser for file type");
        return NULL;
    }

    // 解析
    config_value_t* result = parser->parse(content, len, error);
    free(content);
    return result;
}

// 5. 统一访问 API
const char* config_get_string(config_value_t* root, const char* path, const char* default_val) {
    config_value_t* val = config_resolve_path(root, path);
    if (val && val->type == CONFIG_TYPE_STRING) {
        return val->string_val;
    }
    return default_val;
}

int64_t config_get_int(config_value_t* root, const char* path, int64_t default_val) {
    config_value_t* val = config_resolve_path(root, path);
    if (val && val->type == CONFIG_TYPE_INT) {
        return val->int_val;
    }
    return default_val;
}
