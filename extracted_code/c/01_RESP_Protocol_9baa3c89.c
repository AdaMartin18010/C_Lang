/*
 * Auto-generated from: 03_System_Technology_Domains\11_In_Memory_Database\01_RESP_Protocol.md
 * Line: 578
 * Language: c
 * Block ID: 9baa3c89
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 构建Redis命令数组 */
RESPValue* resp_build_command(int argc, const char **argv) {
    RESPValue *cmd = resp_value_new(RESP_ARRAY);
    cmd->value.array.count = argc;
    cmd->value.array.elements = calloc(argc, sizeof(RESPValue *));

    for (int i = 0; i < argc; i++) {
        cmd->value.array.elements[i] = resp_bulk_string_copy(
            argv[i], strlen(argv[i]));
    }

    return cmd;
}

/* 便捷宏 */
#define RESP_CMD(cmd, ...) ({ \
    const char *args[] = {cmd, ##__VA_ARGS__}; \
    resp_build_command(sizeof(args) / sizeof(args[0]), args); \
})

/* 使用示例 */
void example() {
    /* 构建 SET key value EX 60 */
    RESPValue *set_cmd = RESP_CMD("SET", "mykey", "myvalue", "EX", "60");

    Buffer buf = {malloc(1024), 0, 1024};
    resp_serialize(&buf, set_cmd);

    /* buf.buf 现在包含: *5\r\n$3\r\nSET\r\n$5\r\nmykey\r\n... */

    resp_value_free(set_cmd);
}
