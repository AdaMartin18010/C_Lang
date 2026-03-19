/*
 * Auto-generated from: 12_Practice_Exercises\07_Algorithm_Challenges.md
 * Line: 64
 * Language: c
 * Block ID: fcc16022
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    const int *data;
    size_t size;
    size_t pos;
} SortedStream;

typedef struct {
    SortedStream *streams;
    int k;
    // 最小堆
    struct HeapNode *heap;
} KWayMerger;

KWayMerger* kway_create(SortedStream *streams, int k);
void kway_destroy(KWayMerger *merger);
bool kway_next(KWayMerger *merger, int *value);
