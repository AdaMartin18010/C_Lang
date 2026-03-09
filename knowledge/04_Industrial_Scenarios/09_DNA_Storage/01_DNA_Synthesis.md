# DNA存储合成与编码

> **层级定位**: 04 Industrial Scenarios / 09 DNA Storage
> **对应标准**: Nature DNA Storage, Microsoft Research
> **难度级别**: L5 综合
> **预估学习时间**: 6-8 小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | DNA编码, 碱基转换, 错误纠正, 合成分块 |
| **前置知识** | 信息论, 纠错编码, 生物信息学基础 |
| **后续延伸** | DNA测序, 随机存取, 长期存档 |
| **权威来源** | Church et al. 2012, Goldman et al. 2013 |

---

## 📖 核心实现

### 1. DNA编码基础

```c
// DNA碱基: A(腺嘌呤), C(胞嘧啶), G(鸟嘌呤), T(胸腺嘧啶)
// 每个碱基可存储 ~1.8 bits (考虑生化约束后)

// 简单编码: 2 bits -> 1碱基
// 00 -> A, 01 -> C, 10 -> G, 11 -> T

// 游程限制编码 (Run-length Limited)
// 约束: 避免 >3个连续相同碱基 (合成误差)
//       避免高GC含量 (>60%或<40%, 影响测序)

#define MIN_GC 0.40
#define MAX_GC 0.60
#define MAX_RUN 3

// 旋转编码
typedef enum {
    MAP_AC_GT,
    MAP_AG_CT,
    MAP_AT_CG,
} DNAMapping;

static const char map_tables[3][4] = {
    {'A', 'C', 'G', 'T'},
    {'A', 'G', 'C', 'T'},
    {'A', 'T', 'C', 'G'}
};

// 带游程限制的编码
int encode_with_rll(const uint8_t *data, size_t len, char *dna) {
    int dna_pos = 0;
    DNAMapping current_map = MAP_AC_GT;
    int run_count = 0;
    char last_base = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        for (int j = 0; j < 4; j++) {
            uint8_t bits = (byte >> (6 - j*2)) & 0x03;
            char base = map_tables[current_map][bits];

            if (base == last_base) {
                run_count++;
                if (run_count >= MAX_RUN) {
                    current_map = (current_map + 1) % 3;
                    base = map_tables[current_map][bits];
                    run_count = 1;
                }
            } else {
                run_count = 1;
            }

            dna[dna_pos++] = base;
            last_base = base;
        }
    }

    dna[dna_pos] = '\0';
    return dna_pos;
}
```

### 2. 分块与索引策略

```c
#define BLOCK_DATA_SIZE  20
#define BLOCK_TOTAL_LEN  150
#define INDEX_SIZE       4
#define RS_PARITY_SIZE   6

typedef struct {
    uint32_t index;
    uint8_t data[BLOCK_DATA_SIZE];
    uint8_t rs_parity[RS_PARITY_SIZE];
} DNABlock;

void encode_index(uint32_t index, char *dna) {
    for (int i = 0; i < 16; i++) {
        uint8_t bits = (index >> (30 - i*2)) & 0x03;
        switch (bits) {
            case 0: dna[i] = (i % 2 == 0) ? 'A' : 'C'; break;
            case 1: dna[i] = (i % 2 == 0) ? 'C' : 'G'; break;
            case 2: dna[i] = (i % 2 == 0) ? 'G' : 'T'; break;
            case 3: dna[i] = (i % 2 == 0) ? 'T' : 'A'; break;
        }
    }
}
```

### 3. Reed-Solomon纠错编码

```c
#define RS_N 255
#define RS_K 249
#define RS_T 3

typedef struct {
    uint8_t generator[RS_T*2+1];
} RSContext;

void rs_encode(RSContext *ctx, const uint8_t *data, int len, uint8_t *parity) {
    memset(parity, 0, RS_T * 2);

    for (int i = 0; i < len; i++) {
        uint8_t feedback = data[i] ^ parity[0];

        for (int j = 0; j < RS_T * 2 - 1; j++) {
            parity[j] = parity[j + 1] ^ (feedback * ctx->generator[RS_T*2 - j]);
        }
        parity[RS_T*2 - 1] = feedback * ctx->generator[0];
    }
}
```

### 4. 完整编码流程

```c
int encode_file(const char *input_path, const char *output_path) {
    FILE *fp = fopen(input_path, "rb");
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *data = malloc(file_size);
    fread(data, 1, file_size, fp);
    fclose(fp);

    int blocks_needed = (file_size + BLOCK_DATA_SIZE - 1) / BLOCK_DATA_SIZE;

    fp = fopen(output_path, "w");
    for (int i = 0; i < blocks_needed; i++) {
        fprintf(fp, ">block_%d\n", i);

        DNABlock block;
        block.index = i;

        int data_len = (i == blocks_needed - 1) ?
                       (file_size % BLOCK_DATA_SIZE) : BLOCK_DATA_SIZE;
        if (data_len == 0) data_len = BLOCK_DATA_SIZE;

        memset(block.data, 0, BLOCK_DATA_SIZE);
        memcpy(block.data, data + i*BLOCK_DATA_SIZE, data_len);

        char dna[200];
        int pos = 0;

        encode_index(block.index, dna + pos);
        pos += 16;

        for (int j = 0; j < BLOCK_DATA_SIZE; j++) {
            dna[pos++] = map_tables[0][(block.data[j] >> 6) & 0x03];
            dna[pos++] = map_tables[0][(block.data[j] >> 4) & 0x03];
            dna[pos++] = map_tables[0][(block.data[j] >> 2) & 0x03];
            dna[pos++] = map_tables[0][block.data[j] & 0x03];
        }

        dna[pos] = '\0';
        fprintf(fp, "%s\n", dna);
    }
    fclose(fp);

    free(data);

    printf("Encoded %zu bytes into %d DNA blocks\n", file_size, blocks_needed);

    return 0;
}
```

---

## 参考研究

- Church et al. (2012) - Next-Generation Digital Information Storage in DNA
- Goldman et al. (2013) - Towards practical DNA storage

---

## ✅ 质量验收清单

- [x] DNA碱基编码
- [x] 游程限制编码
- [x] 分块与索引
- [x] Reed-Solomon纠错
- [x] 完整编码流程

---

> **更新记录**
>
> - 2025-03-09: 初版创建
