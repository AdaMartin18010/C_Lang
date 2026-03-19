/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 316
 * Language: c
 * Block ID: cd8c6372
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

double warmup_lr(int step, int warmup_steps, double target_lr) {
    if (step < warmup_steps) {
        return target_lr * ((double)step / warmup_steps);
    }
    return target_lr;
}
