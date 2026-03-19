/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\01_DNA_Synthesis.md
 * Line: 415
 * Language: c
 * Block ID: e5d6924b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// DNA存储数据分块与索引
// ============================================================================

#define BLOCK_DATA_SIZE     20      // 每块数据字节数
#define BLOCK_PAYLOAD_SIZE  150     // DNA序列长度
#define INDEX_SIZE          16      // 索引碱基数
#define RS_PARITY_SYMBOLS   6       // Reed-Solomon校验符号

// DNA数据块结构
typedef struct {
    uint32_t index;                 // 块索引号
    uint8_t data[BLOCK_DATA_SIZE];  // 实际数据
    uint8_t data_len;               // 数据长度 (最后一块可能不满)
    uint8_t rs_parity[RS_PARITY_SYMBOLS];  // RS校验
} DataBlock;

// 完整的DNA序列块
typedef struct {
    char primer_fwd[20];            // 前向引物
    char index[INDEX_SIZE];         // 块索引 (DNA编码)
    char payload[BLOCK_PAYLOAD_SIZE]; // 数据载荷
    char rs_parity[RS_PARITY_SYMBOLS * 2]; // RS校验 (DNA编码)
    char primer_rev[20];            // 反向引物
} DNAOligo;

// ============================================================================
// 索引编码 (用于随机存取)
// ============================================================================

// 将32位索引编码为DNA序列
void encode_index(uint32_t index, char *dna_index) {
    // 使用旋转编码，确保GC平衡
    int pos = 0;

    for (int i = 0; i < 16; i++) {
        uint8_t bits = (index >> (30 - i * 2)) & 0x03;

        // 根据位置选择编码表，确保多样性
        uint8_t table_id = i % 8;
        DNABase base = encoding_tables[table_id][bits];

        dna_index[pos++] = base_chars[base];
    }
}

// 解码索引
uint32_t decode_index(const char *dna_index) {
    uint32_t index = 0;

    for (int i = 0; i < 16; i++) {
        char base = dna_index[i];
        uint8_t table_id = i % 8;

        // 查找碱基对应的2-bit值
        uint8_t bits = 0;
        for (int k = 0; k < 4; k++) {
            if (base_chars[encoding_tables[table_id][k]] == base) {
                bits = k;
                break;
            }
        }

        index = (index << 2) | bits;
    }

    return index;
}

// ============================================================================
// 文件编码流程
// ============================================================================

typedef struct {
    char *filename;
    uint8_t *data;
    size_t data_size;
    DNAOligo *oligos;
    size_t num_oligos;
} DNAStorageFile;

int encode_file_to_dna(const char *input_path, DNAStorageFile *dsf) {
    // 读取文件
    FILE *fp = fopen(input_path, "rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    dsf->data_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    dsf->data = malloc(dsf->data_size);
    fread(dsf->data, 1, dsf->data_size, fp);
    fclose(fp);

    // 计算需要的块数
    dsf->num_oligos = (dsf->data_size + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;
    dsf->oligos = malloc(dsf->num_oligos * sizeof(DNAOligo));

    // 编码每一块
    for (size_t i = 0; i < dsf->num_oligos; i++) {
        DNAOligo *oligo = &dsf->oligos[i];

        // 1. 准备数据块
        DataBlock block;
        block.index = (uint32_t)i;
        block.data_len = (i == dsf->num_oligos - 1) ?
                         (dsf->data_size % BLOCK_DATA_SIZE) : BLOCK_DATA_SIZE;
        if (block.data_len == 0) block.data_len = BLOCK_DATA_SIZE;

        memcpy(block.data, dsf->data + i * BLOCK_DATA_SIZE, block.data_len);

        // 2. 计算RS校验 (简化)
        // 实际应使用完整的Reed-Solomon编码
        memset(block.rs_parity, 0, RS_PARITY_SYMBOLS);

        // 3. 编码索引
        encode_index(block.index, oligo->index);

        // 4. 编码数据
        char temp_dna[BLOCK_PAYLOAD_SIZE];
        int dna_len = encode_rll(block.data, block.data_len, temp_dna);
        memcpy(oligo->payload, temp_dna, dna_len);

        // 5. 添加引物 (固定序列)
        strcpy(oligo->primer_fwd, "ATGCGATCGATCGATCGATA");
        strcpy(oligo->primer_rev, "TACGCTAGCTAGCTAGCTAT");
    }

    return 0;
}

// ============================================================================
// 引物设计 (用于PCR扩增)
// ============================================================================

#define PRIMER_LENGTH       20
#define PRIMER_MIN_TM       55.0    // 最小熔解温度
#define PRIMER_MAX_TM       65.0    // 最大熔解温度
#define PRIMER_MIN_GC       0.40
#define PRIMER_MAX_GC       0.60

// 简单熔解温度计算 (简化版)
double calculate_tm_simple(const char *primer) {
    int gc_count = 0;
    int at_count = 0;

    for (int i = 0; i < PRIMER_LENGTH && primer[i]; i++) {
        if (primer[i] == 'G' || primer[i] == 'C') {
            gc_count++;
        } else {
            at_count++;
        }
    }

    // Wallace规则
    return 2.0 * at_count + 4.0 * gc_count;
}

// 检查引物是否满足约束
bool check_primer_constraints(const char *primer) {
    // 检查长度
    if (strlen(primer) != PRIMER_LENGTH) return false;

    // 检查熔解温度
    double tm = calculate_tm_simple(primer);
    if (tm < PRIMER_MIN_TM || tm > PRIMER_MAX_TM) return false;

    // 检查GC含量
    int gc_count = 0;
    for (int i = 0; i < PRIMER_LENGTH; i++) {
        if (primer[i] == 'G' || primer[i] == 'C') gc_count++;
    }
    double gc_ratio = (double)gc_count / PRIMER_LENGTH;
    if (gc_ratio < PRIMER_MIN_GC || gc_ratio > PRIMER_MAX_GC) return false;

    // 检查3'端 (最后5个碱基)
    int end_gc = 0;
    for (int i = PRIMER_LENGTH - 5; i < PRIMER_LENGTH; i++) {
        if (primer[i] == 'G' || primer[i] == 'C') end_gc++;
    }
    if (end_gc < 2) return false;  // 3'端GC太少

    return true;
}
