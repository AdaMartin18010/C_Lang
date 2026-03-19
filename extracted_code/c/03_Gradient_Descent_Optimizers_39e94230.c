/*
 * Auto-generated from: 11_Machine_Learning_C\03_Gradient_Descent_Optimizers.md
 * Line: 295
 * Language: c
 * Block ID: 39e94230
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

double lr_schedule(int epoch, double initial_lr, int decay_epochs) {
    // 阶梯衰减
    if (epoch % decay_epochs == 0 && epoch > 0) {
        return initial_lr * 0.5;
    }

    // 指数衰减
    // return initial_lr * exp(-0.1 * epoch);

    // 余弦退火
    // return initial_lr * (1 + cos(M_PI * epoch / max_epochs)) / 2;

    return initial_lr;
}
