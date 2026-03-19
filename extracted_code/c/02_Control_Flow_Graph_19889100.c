/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\02_Control_Flow_Graph.md
 * Line: 330
 * Language: c
 * Block ID: 19889100
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 寄存器分配使用活性信息
void compute_liveness(Function *func) {
    // 逆向迭代
    bool changed;
    do {
        changed = false;
        for (B in reverse_postorder) {
            Set old_live_out = B->live_out;

            B->live_out = {};
            for (S in B->succs) {
                B->live_out |= S->live_in;
            }

            B->live_in = B->use | (B->live_out - B->def);

            if (B->live_out != old_live_out) {
                changed = true;
            }
        }
    } while (changed);
}
