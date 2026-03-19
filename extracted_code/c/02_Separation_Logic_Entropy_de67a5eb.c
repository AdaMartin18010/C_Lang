/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\04_Formal_Verification_Energy\02_Separation_Logic_Entropy.md
 * Line: 93
 * Language: c
 * Block ID: de67a5eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 堆的数学模型
typedef uint64_t Addr;
typedef int64_t Value;

// 堆：地址到值的部分映射
typedef struct {
    GHashTable *mapping;
} Heap;

// 空堆
Heap *heap_empty(void) {
    Heap *h = malloc(sizeof(Heap));
    h->mapping = g_hash_table_new(g_direct_hash, g_direct_equal);
    return h;
}

// 单点堆：l↦v 表示地址l存储值v
Heap *heap_singleton(Addr l, Value v) {
    Heap *h = heap_empty();
    g_hash_table_insert(h->mapping, GUINT_TO_POINTER(l),
                        GINT_TO_POINTER(v));
    return h;
}

// 堆不相交
gboolean heaps_disjoint(const Heap *h1, const Heap *h2) {
    GHashTableIter iter;
    gpointer key;
    g_hash_table_iter_init(&iter, h1->mapping);
    while (g_hash_table_iter_next(&iter, &key, NULL)) {
        if (g_hash_table_contains(h2->mapping, key)) {
            return FALSE;
        }
    }
    return TRUE;
}

// 分离逻辑断言的语义
typedef bool (*SL_Assertion)(Heap *h);

// emp：空堆断言
bool sl_emp(Heap *h) {
    return g_hash_table_size(h->mapping) == 0;
}

// l ↦ v：单点断言
SL_Assertion sl_points_to(Addr l, Value v) {
    return lambda(bool, (Heap *h), {
        return g_hash_table_size(h->mapping) == 1 &&
               GPOINTER_TO_INT(g_hash_table_lookup(h->mapping,
                                                    GUINT_TO_POINTER(l))) == v;
    });
}

// P * Q：分离合取
SL_Assertion sl_sep_conj(SL_Assertion P, SL_Assertion Q) {
    return lambda(bool, (Heap *h), {
        // 存在h1, h2使得 h = h1 ⊎ h2 且 P(h1) 且 Q(h2)
        GList *addrs = g_hash_table_get_keys(h->mapping);
        int n = g_list_length(addrs);

        // 尝试所有可能的分割
        for (int mask = 0; mask < (1 << n); mask++) {
            Heap *h1 = heap_empty();
            Heap *h2 = heap_empty();

            for (int i = 0; i < n; i++) {
                Addr addr = GPOINTER_TO_UINT(g_list_nth_data(addrs, i));
                Value val = GPOINTER_TO_INT(g_hash_table_lookup(h->mapping,
                                                                GUINT_TO_POINTER(addr)));
                if (mask & (1 << i)) {
                    g_hash_table_insert(h1->mapping,
                                       GUINT_TO_POINTER(addr),
                                       GINT_TO_POINTER(val));
                } else {
                    g_hash_table_insert(h2->mapping,
                                       GUINT_TO_POINTER(addr),
                                       GINT_TO_POINTER(val));
                }
            }

            if (P(h1) && Q(h2)) {
                return TRUE;
            }
        }
        return FALSE;
    });
}
