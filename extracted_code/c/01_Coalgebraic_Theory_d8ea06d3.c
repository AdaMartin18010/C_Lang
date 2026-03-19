/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\02_Coalgebraic_Methods\01_Coalgebraic_Theory.md
 * Line: 403
 * Language: c
 * Block ID: d8ea06d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：共归纳定义没有守卫
Stream *bad_stream(void) {
    Stream *s = malloc(sizeof(Stream));
    s->head = 0;
    s->tail = bad_stream;  // 无限递归，无基例！
    return s;
}

// 正确：产品式守卫
Stream *nats_from(int n) {
    Stream *s = malloc(sizeof(Stream));
    s->head = n;
    s->tail = /* 惰性求值 */;
    return s;
}
