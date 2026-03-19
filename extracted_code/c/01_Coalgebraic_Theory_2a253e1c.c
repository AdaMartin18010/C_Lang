/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 366
 * Language: c
 * Block ID: 2a253e1c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：用代数方式处理无限结构
int *construct_all_nats(void) {
    int *nats = malloc(sizeof(int) * INFINITY);  // 不可能！
    for (int i = 0; i < INFINITY; i++) {
        nats[i] = i;
    }
    return nats;
}

// 正确：用余代数方式观察无限结构
int stream_head(Stream *s) {
    return s->head;  // 观察当前值
}

Stream *stream_tail(Stream *s) {
    return s->tail(s);  // 观察下一个
}
