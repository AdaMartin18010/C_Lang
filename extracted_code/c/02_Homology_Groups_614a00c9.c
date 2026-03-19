/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\09_Linking_Algebraic_Topology\02_Homology_Groups.md
 * Line: 95
 * Language: c
 * Block ID: 614a00c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 链群的代数表示
typedef struct {
    int dimension;
    Element *generators;    // 生成元集合
    int n_generators;
} ChainGroup;

// 0-链：符号
// C₀ = ⊕ ℤ·sᵢ, 其中 sᵢ 是符号
typedef struct {
    char *symbol_name;
    uint64_t address;
    enum { FUNC_SYM, VAR_SYM, SECTION_SYM } kind;
} ZeroChain;  // 0-链元素

// 1-链：引用
// C₁ = ⊕ ℤ·rᵢ, 其中 rᵢ = (source → target) 是引用
typedef struct {
    ZeroChain *source;      // 源符号
    ZeroChain *target;      // 目标符号
    enum { ABS_REF, REL_REF, GOT_REF } kind;
    int64_t addend;         // 加数
} OneChain;   // 1-链元素

// 2-链：依赖环
// C₂ = ⊕ ℤ·dᵢ, 其中 dᵢ 是依赖三角形 (A→B→C→A)
typedef struct {
    OneChain *edges[3];     // 三条边构成三角形
    enum { CALL_CYCLE, DATA_CYCLE, MIXED_CYCLE } kind;
} TwoChain;   // 2-链元素
