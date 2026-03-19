/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 41
 * Language: c
 * Block ID: ece1385d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基本块划分算法
typedef struct BasicBlock {
    int id;
    Instruction *first;
    Instruction *last;
    struct BasicBlock **preds;  // 前驱
    struct BasicBlock **succs;  // 后继
    int num_preds, num_succs;
} BasicBlock;

// 识别基本块边界
void identify_leaders(Instruction *code, int len, bool *is_leader) {
    memset(is_leader, 0, len * sizeof(bool));
    is_leader[0] = true;  // 第一条指令是leader

    for (int i = 0; i < len; i++) {
        if (is_branch(code[i])) {
            is_leader[i] = true;           // 分支目标是leader
            if (i + 1 < len) {
                is_leader[i + 1] = true;   // 分支后一条是leader
            }
        }
    }
}

// 构建CFG
BasicBlock** build_cfg(Function *func, int *num_blocks) {
    // 1. 识别所有leader
    // 2. 为每个leader创建基本块
    // 3. 连接基本块（分析跳转目标）
    // 4. 验证图结构
}
