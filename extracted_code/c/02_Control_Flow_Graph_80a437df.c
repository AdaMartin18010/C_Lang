/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 245
 * Language: c
 * Block ID: 80a437df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void optimize_unreachable(BasicBlock *entry) {
    // 1. 从entry可达的块标记
    Set reachable = {};
    Queue worklist = {entry};

    while (!empty(worklist)) {
        BasicBlock *bb = dequeue(worklist);
        if (bb in reachable) continue;
        reachable.add(bb);

        for (succ in bb->succs) {
            enqueue(worklist, succ);
        }
    }

    // 2. 删除不可达块
    for (bb in all_blocks) {
        if (bb not in reachable) {
            delete_basic_block(bb);
        }
    }
}
