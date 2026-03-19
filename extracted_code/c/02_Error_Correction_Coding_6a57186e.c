/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\02_Error_Correction_Coding.md
 * Line: 421
 * Language: c
 * Block ID: 6a57186e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 块交织器 - 将突发错误分散
// ============================================================================

#define INTERLEAVE_ROWS     32
#define INTERLEAVE_COLS     32
#define INTERLEAVE_SIZE     (INTERLEAVE_ROWS * INTERLEAVE_COLS)

// 交织器上下文
typedef struct {
    uint8_t buffer[INTERLEAVE_SIZE];
    uint8_t rows;
    uint8_t cols;
} Interleaver;

// 初始化交织器
void interleaver_init(Interleaver *ilv, uint8_t rows, uint8_t cols) {
    ilv->rows = rows;
    ilv->cols = cols;
    memset(ilv->buffer, 0, sizeof(ilv->buffer));
}

// 块交织: 按行写入，按列读出
void interleave_block(Interleaver *ilv, const uint8_t *input, uint8_t *output) {
    // 按行写入缓冲区
    for (int r = 0; r < ilv->rows; r++) {
        for (int c = 0; c < ilv->cols; c++) {
            ilv->buffer[r * ilv->cols + c] = input[r * ilv->cols + c];
        }
    }

    // 按列读出
    for (int c = 0; c < ilv->cols; c++) {
        for (int r = 0; r < ilv->rows; r++) {
            output[c * ilv->rows + r] = ilv->buffer[r * ilv->cols + c];
        }
    }
}

// 解交织: 按列写入，按行读出
void deinterleave_block(Interleaver *ilv, const uint8_t *input, uint8_t *output) {
    // 按列写入缓冲区
    for (int c = 0; c < ilv->cols; c++) {
        for (int r = 0; r < ilv->rows; r++) {
            ilv->buffer[r * ilv->cols + c] = input[c * ilv->rows + r];
        }
    }

    // 按行读出
    for (int r = 0; r < ilv->rows; r++) {
        for (int c = 0; c < ilv->cols; c++) {
            output[r * ilv->cols + c] = ilv->buffer[r * ilv->cols + c];
        }
    }
}

// ============================================================================
// 卷积交织器 (更高效的内存使用)
// ============================================================================

#define CONV_INTERLEAVE_DEPTH   16

typedef struct {
    uint8_t shift_registers[CONV_INTERLEAVE_DEPTH][INTERLEAVE_ROWS];
    uint8_t write_pos[CONV_INTERLEAVE_DEPTH];
} ConvInterleaver;

void conv_interleave_init(ConvInterleaver *cilv) {
    memset(cilv->shift_registers, 0, sizeof(cilv->shift_registers));
    memset(cilv->write_pos, 0, sizeof(cilv->write_pos));
}

uint8_t conv_interleave_byte(ConvInterleaver *cilv, uint8_t input, int branch) {
    // 写入当前位置
    uint8_t *reg = cilv->shift_registers[branch];
    uint8_t pos = cilv->write_pos[branch];

    uint8_t output = reg[pos];
    reg[pos] = input;

    cilv->write_pos[branch] = (pos + 1) % INTERLEAVE_ROWS;

    return output;
}
