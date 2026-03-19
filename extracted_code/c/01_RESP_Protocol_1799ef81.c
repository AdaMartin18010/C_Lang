/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_RESP_Protocol.md
 * Line: 428
 * Language: c
 * Block ID: 1799ef81
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 完整解析（阻塞式） */
RESPValue* resp_parse_complete(const char *buf, size_t len, size_t *consumed) {
    RESPParser parser = {
        .state = PARSE_STATE_INIT,
        .buf = buf,
        .pos = 0,
        .len = len,
        .stack = malloc(16 * sizeof(RESPValue *)),
        .stack_capacity = 16,
    };

    int rc = resp_parse(&parser);

    if (consumed) {
        *consumed = parser.pos;
    }

    RESPValue *result = NULL;
    if (rc == 0) {
        result = parser.current;
    } else {
        resp_value_free(parser.current);
    }

    free(parser.stack);
    return result;
}
