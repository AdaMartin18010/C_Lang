/*
 * Auto-generated from: 11_Machine_Learning_C\01_Neural_Network_Basics.md
 * Line: 2143
 * Language: c
 * Block ID: d7402a6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    LR_CONSTANT,
    LR_STEP_DECAY,
    LR_EXPONENTIAL,
    LR_COSINE_ANNEALING,
    LR_REDUCE_ON_PLATEAU
} LRScheduleType;

typedef struct {
    LRScheduleType type;
    double initial_lr;
    double current_lr;

    // Step decay参数
    int step_size;
    double gamma;

    // Exponential参数
    double decay_rate;

    // Cosine annealing参数
    double lr_min;
    int T_max;  // 半周期

    // Plateau参数
    double patience;
    double factor;
    double best_loss;
    int wait;
} LRScheduler;

// 更新学习率
double scheduler_step(LRScheduler* sched, int epoch, double current_loss) {
    switch (sched->type) {
        case LR_CONSTANT:
            return sched->initial_lr;

        case LR_STEP_DECAY:
            sched->current_lr = sched->initial_lr *
                pow(sched->gamma, epoch / sched->step_size);
            break;

        case LR_EXPONENTIAL:
            sched->current_lr = sched->initial_lr *
                exp(-sched->decay_rate * epoch);
            break;

        case LR_COSINE_ANNEALING:
            sched->current_lr = sched->lr_min + 0.5 * (sched->initial_lr - sched->lr_min) *
                (1 + cos(M_PI * epoch / sched->T_max));
            break;

        case LR_REDUCE_ON_PLATEAU:
            if (current_loss < sched->best_loss) {
                sched->best_loss = current_loss;
                sched->wait = 0;
            } else {
                sched->wait++;
                if (sched->wait >= sched->patience) {
                    sched->current_lr *= sched->factor;
                    sched->wait = 0;
                    printf("Reducing LR to %.6f\n", sched->current_lr);
                }
            }
            break;
    }

    return sched->current_lr;
}
