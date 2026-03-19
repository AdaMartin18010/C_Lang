/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 382
 * Language: c
 * Block ID: df33a451
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 遍历CFG
void visit_cfg(Function *F) {
    for (BasicBlock &BB : *F) {
        errs() << "BasicBlock: " << BB.getName() << "\n";

        // 前驱
        for (BasicBlock *Pred : predecessors(&BB)) {
            errs() << "  Pred: " << Pred->getName() << "\n";
        }

        // 后继
        for (BasicBlock *Succ : successors(&BB)) {
            errs() << "  Succ: " << Succ->getName() << "\n";
        }
    }
}

// 支配树
void analyze_dominance(Function *F) {
    DominatorTree DT(*F);

    for (BasicBlock &BB : *F) {
        if (DomTreeNode *Node = DT.getNode(&BB)) {
            if (BasicBlock *IDom = Node->getIDom()) {
                errs() << BB.getName() << " dominated by "
                       << IDom->getName() << "\n";
            }
        }
    }
}
