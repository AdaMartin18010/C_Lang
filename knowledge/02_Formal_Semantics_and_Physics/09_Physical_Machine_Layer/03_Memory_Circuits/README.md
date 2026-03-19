# 存储电路 (Memory Circuits)

> **层级定位**: 02_Formal_Semantics_and_Physics > 09_Physical_Machine_Layer > 03_Memory_Circuits
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: 数字逻辑基础、CMOS晶体管
> **参考标准**: IEEE Solid-State Circuits Society

---

## 目录

- [SR锁存器和D触发器](./01_SR_Latch_and_D_FlipFlop.md)
- [寄存器文件](./02_Register_File.md)
- [SRAM单元](./03_SRAM_Cell.md)
- [缓存阵列](./04_Cache_Array.md)

---

## 概述

存储电路是计算机存储层次的基础，从锁存器到寄存器堆，再到SRAM缓存。理解这些电路的工作原理对于系统性能优化至关重要。

### 存储层次

```
寄存器 (Register)     - 最快, 最小, 在CPU内部
    ↓
SRAM缓存 (L1/L2/L3)   - 高速缓存, 6T单元
    ↓
DRAM内存 (Main Memory) - 大容量, 1T1C单元
    ↓
存储器 (SSD/HDD)      - 永久存储
```

---

## 深入理解

### 1. 锁存器与触发器

#### SR锁存器

```c
// SR锁存器行为模型
typedef struct {
    uint8_t q;      // 输出Q
    uint8_t q_bar;  // 输出Q'
} SRLatch;

void sr_latch_update(SRLatch* latch, uint8_t s, uint8_t r) {
    // S=1, R=0: Set (Q=1)
    // S=0, R=1: Reset (Q=0)
    // S=0, R=0: Hold (保持)
    // S=1, R=1: Forbidden (禁止)

    if (s && !r) {
        latch->q = 1;
        latch->q_bar = 0;
    } else if (!s && r) {
        latch->q = 0;
        latch->q_bar = 1;
    } else if (!s && !r) {
        // 保持状态
    } else {
        // S=1, R=1: 未定义状态
        latch->q = 0;
        latch->q_bar = 0;  // 实际电路中应避免
    }
}

// 门电路实现:
// Q = NOR(S, Q')
// Q' = NOR(R, Q)
```

#### D触发器 (边沿触发)

```c
// D触发器 - 时钟上升沿采样
typedef struct {
    uint8_t q;          // 当前输出
    uint8_t next_q;     // 下一个状态
    uint8_t clk_prev;   // 上一时钟状态
} DFlipFlop;

void dff_update(DFlipFlop* dff, uint8_t d, uint8_t clk) {
    // 检测上升沿
    if (!dff->clk_prev && clk) {
        dff->q = d;  // 在上升沿采样D
    }
    dff->clk_prev = clk;
}

// 建立时间和保持时间检查
void dff_timing_check(uint8_t d, uint8_t clk,
                      float setup_time, float hold_time,
                      float d_last_change, float clk_edge_time) {
    // 建立时间: D必须在时钟沿前稳定
    if (clk_edge_time - d_last_change < setup_time) {
        printf("Setup time violation!\n");
    }

    // 保持时间: D必须在时钟沿后保持稳定
    if (d_last_change - clk_edge_time < hold_time) {
        printf("Hold time violation!\n");
    }
}
```

### 2. 寄存器文件 (Register File)

```c
// 32x32位寄存器文件模型
#define NUM_REGS 32
#define REG_WIDTH 32

typedef struct {
    uint32_t regs[NUM_REGS];  // 寄存器存储

    // 读端口
    uint8_t read_addr1, read_addr2;
    uint32_t read_data1, read_data2;

    // 写端口
    uint8_t write_addr;
    uint32_t write_data;
    uint8_t write_enable;
} RegisterFile32;

// 寄存器文件操作
void regfile_cycle(RegisterFile32* rf) {
    // 读操作 (组合逻辑)
    rf->read_data1 = rf->regs[rf->read_addr1];
    rf->read_data2 = rf->regs[rf->read_addr2];

    // 写操作 (时钟上升沿)
    if (rf->write_enable && rf->write_addr != 0) {
        // R0始终为0
        rf->regs[rf->write_addr] = rf->write_data;
    }
}

// 三态门实现的多端口读
void regfile_read_tri_state(RegisterFile32* rf,
                            uint8_t addr1, uint8_t addr2,
                            uint32_t* data1, uint32_t* data2) {
    // 实际硬件使用字线选择 + 位线读
    for (int i = 0; i < NUM_REGS; i++) {
        if (i == addr1) {
            *data1 = rf->regs[i];
        }
        if (i == addr2) {
            *data2 = rf->regs[i];
        }
    }
}
```

### 3. SRAM单元设计

```c
// 6T SRAM单元行为模型
typedef struct {
    uint8_t bit;        // 存储的数据
    uint8_t bit_bar;    // 互补数据
} SRAMCell6T;

// SRAM读写操作
void sram_cell_access(SRAMCell6T* cell,
                      uint8_t word_line,      // 字线控制
                      uint8_t* bit_line,      // 位线 (双向)
                      uint8_t* bit_line_bar,  // 互补位线
                      uint8_t write_enable) {

    if (!word_line) {
        // 字线无效，单元保持
        return;
    }

    if (write_enable) {
        // 写操作: 驱动位线覆盖存储值
        cell->bit = *bit_line;
        cell->bit_bar = *bit_line_bar;
    } else {
        // 读操作: 感知位线电压差
        *bit_line = cell->bit;
        *bit_line_bar = cell->bit_bar;
    }
}

// SRAM阵列组织
#define SRAM_ROWS 256
#define SRAM_COLS 32  // 32位宽

typedef struct {
    SRAMCell6T cells[SRAM_ROWS][SRAM_COLS];

    // 地址解码
    uint8_t row_addr;       // 8位行地址
    uint8_t col_addr;       // 选择字节

    // 控制信号
    uint8_t chip_enable;
    uint8_t write_enable;
    uint8_t output_enable;
} SRAMArray;

void sram_access(SRAMArray* sram, uint32_t* data) {
    if (!sram->chip_enable) return;

    // 行解码
    uint8_t row = sram->row_addr;

    if (sram->write_enable) {
        // 写操作
        for (int i = 0; i < SRAM_COLS; i++) {
            sram->cells[row][i].bit = (*data >> i) & 1;
            sram->cells[row][i].bit_bar = !((*data >> i) & 1);
        }
    } else if (sram->output_enable) {
        // 读操作
        *data = 0;
        for (int i = 0; i < SRAM_COLS; i++) {
            *data |= (sram->cells[row][i].bit << i);
        }
    }
}
```

### 4. 缓存阵列结构

```c
// 直接映射缓存模型
#define CACHE_SIZE 1024   // 1KB
#define BLOCK_SIZE 32     // 32字节/块
#define NUM_BLOCKS (CACHE_SIZE / BLOCK_SIZE)
#define INDEX_BITS 5      // log2(NUM_BLOCKS)
#define OFFSET_BITS 5     // log2(BLOCK_SIZE)

typedef struct {
    uint8_t valid;        // 有效位
    uint8_t dirty;        // 脏位
    uint32_t tag;         // 标签
    uint8_t data[BLOCK_SIZE];  // 数据块
} CacheLine;

typedef struct {
    CacheLine lines[NUM_BLOCKS];
} DirectMappedCache;

// 地址解析
void parse_address(uint32_t addr,
                   uint32_t* tag,
                   uint32_t* index,
                   uint32_t* offset) {
    *offset = addr & ((1 << OFFSET_BITS) - 1);
    *index = (addr >> OFFSET_BITS) & ((1 << INDEX_BITS) - 1);
    *tag = addr >> (OFFSET_BITS + INDEX_BITS);
}

// 缓存访问
int cache_access(DirectMappedCache* cache,
                 uint32_t addr,
                 uint8_t* data,
                 uint8_t write_enable,
                 uint8_t write_data) {

    uint32_t tag, index, offset;
    parse_address(addr, &tag, &index, &offset);

    CacheLine* line = &cache->lines[index];

    // 检查命中
    if (line->valid && line->tag == tag) {
        // 命中
        if (write_enable) {
            line->data[offset] = write_data;
            line->dirty = 1;
        } else {
            *data = line->data[offset];
        }
        return 1;  // Hit
    }

    // 未命中 - 需要替换
    return 0;  // Miss
}

// 组相联缓存 (2-way)
#define WAYS 2

typedef struct {
    CacheLine sets[NUM_BLOCKS / WAYS][WAYS];  // 组
} SetAssociativeCache;

int set_assoc_access(SetAssociativeCache* cache,
                     uint32_t addr,
                     uint8_t* data,
                     uint8_t write_enable) {

    uint32_t tag, index, offset;
    parse_address(addr, &tag, &index, &offset);

    // 在所有路中查找
    for (int i = 0; i < WAYS; i++) {
        CacheLine* line = &cache->sets[index][i];
        if (line->valid && line->tag == tag) {
            // 命中
            *data = line->data[offset];
            return 1;
        }
    }

    // 未命中 - LRU替换
    return 0;
}
```

---

## 最佳实践

1. **时序约束**: 建立时间和保持时间是存储电路的关键约束
2. **功耗优化**: 时钟门控、电源门控减少动态和静态功耗
3. **可靠性**: ECC校验保护关键数据
4. **面积效率**: 6T SRAM单元在速度和密度间取得平衡

---

## 权威参考

- [1] "CMOS VLSI Design" by Weste & Harris, Chapter 12 (Memory)
- [2] "Memory Systems" by Bruce Jacob
- [3] IEEE Journal of Solid-State Circuits

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
