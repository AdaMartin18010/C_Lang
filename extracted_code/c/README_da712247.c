/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\README.md
 * Line: 399
 * Language: c
 * Block ID: da712247
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 基于中断的脉冲处理
volatile uint8_t spike_flag = 0;
volatile int spike_neuron_id = -1;

// 定时器中断处理
void TIM_IRQHandler(void) {
    for (int i = 0; i < num_neurons; i++) {
        if (neurons[i].next_spike_time == current_tick) {
            spike_flag = 1;
            spike_neuron_id = i;
            // 触发输出
            GPIO_SetBits(SPIKE_OUT_PORT, (1 << i));
        }
    }
    current_tick++;
}

// 主循环
int main(void) {
    init_neurons();
    init_timer();

    while (1) {
        if (spike_flag) {
            process_spike(spike_neuron_id);
            spike_flag = 0;

            // 进入低功耗模式直到下一个事件
            __WFI();
        }
    }
}
