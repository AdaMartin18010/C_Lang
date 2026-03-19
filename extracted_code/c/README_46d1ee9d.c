/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 219
 * Language: c
 * Block ID: 46d1ee9d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于 DNA 序列特性的 Huffman 编码
typedef struct HuffmanNode {
    char base;
    int freq;
    struct HuffmanNode *left, *right;
} HuffmanNode;

// 构建频率表
void build_freq_table(const char *dna_seq, size_t len,
                      int freq[256]) {
    memset(freq, 0, 256 * sizeof(int));
    for (size_t i = 0; i < len; i++) {
        freq[(unsigned char)dna_seq[i]]++;
    }
}

// 计算二聚体频率用于优化
void build_dimer_freq(const char *dna_seq, size_t len,
                      int dimer_freq[4][4]) {
    const char bases[] = "ATCG";
    memset(dimer_freq, 0, 16 * sizeof(int));

    for (size_t i = 0; i < len - 1; i++) {
        int b1 = -1, b2 = -1;
        for (int j = 0; j < 4; j++) {
            if (dna_seq[i] == bases[j]) b1 = j;
            if (dna_seq[i+1] == bases[j]) b2 = j;
        }
        if (b1 >= 0 && b2 >= 0) {
            dimer_freq[b1][b2]++;
        }
    }
}
