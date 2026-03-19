/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 462
 * Language: c
 * Block ID: 800fdbf9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GNU哈希优化（O(1)符号查找）

// GNU哈希表结构
typedef struct {
    uint32_t nbuckets;
    uint32_t symoffset;
    uint32_t bloom_size;
    uint32_t bloom_shift;
    uint64_t bloom[bloom_size];  // 布隆过滤器
    uint32_t buckets[nbuckets];
    uint32_t chain[...];
} GnuHash;

// 快速符号查找
Elf64_Sym *gnu_hash_lookup(GnuHash *hash, const char *name,
                           const char *strtab, Elf64_Sym *symtab) {
    uint32_t h = gnu_hash(name);

    // 布隆过滤器快速排除
    uint64_t word = hash->bloom[(h / 64) % hash->bloom_size];
    uint64_t mask = (1ULL << (h % 64)) |
                    (1ULL << ((h >> hash->bloom_shift) % 64));
    if ((word & mask) != mask) {
        return NULL;  // 肯定不存在
    }

    // 实际查找
    uint32_t idx = hash->buckets[h % hash->nbuckets];
    if (idx < hash->symoffset) return NULL;

    // 遍历链
    for (;;) {
        const char *symname = strtab + symtab[idx].st_name;
        uint32_t symhash = hash->chain[idx - hash->symoffset];

        if ((h|1) == (symhash|1) && strcmp(name, symname) == 0) {
            return &symtab[idx];
        }

        if (symhash & 1) break;  // 链结束
        idx++;
    }

    return NULL;
}

// DJB2哈希
uint32_t gnu_hash(const char *name) {
    uint32_t h = 5381;
    for (unsigned char c = *name; c; c = *++name) {
        h = (h << 5) + h + c;
    }
    return h;
}
