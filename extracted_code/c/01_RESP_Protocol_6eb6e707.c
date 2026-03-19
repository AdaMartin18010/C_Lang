/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_RESP_Protocol.md
 * Line: 175
 * Language: c
 * Block ID: 6eb6e707
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建RESP值 */
RESPValue* resp_value_new(RESPType type) {
    RESPValue *v = calloc(1, sizeof(RESPValue));
    v->type = type;
    return v;
}

/* 释放RESP值 */
void resp_value_free(RESPValue *v) {
    if (!v) return;

    switch (v->type) {
    case RESP_BULK_STRING:
        if (v->value.bulk.owned && v->value.bulk.data) {
            free(v->value.bulk.data);
        }
        break;

    case RESP_ARRAY:
        for (size_t i = 0; i < v->value.array.count; i++) {
            resp_value_free(v->value.array.elements[i]);
        }
        free(v->value.array.elements);
        break;

    default:
        break;
    }

    free(v);
}

/* 创建Bulk String（零拷贝视图） */
RESPValue* resp_bulk_string_view(const char *data, size_t len) {
    RESPValue *v = resp_value_new(RESP_BULK_STRING);
    v->value.bulk.data = (char *)data;
    v->value.bulk.len = len;
    v->value.bulk.owned = false;
    return v;
}

/* 创建Owned Bulk String（复制数据） */
RESPValue* resp_bulk_string_copy(const char *data, size_t len) {
    RESPValue *v = resp_value_new(RESP_BULK_STRING);
    v->value.bulk.data = malloc(len + 1);
    memcpy(v->value.bulk.data, data, len);
    v->value.bulk.data[len] = '\0';
    v->value.bulk.len = len;
    v->value.bulk.owned = true;
    return v;
}
