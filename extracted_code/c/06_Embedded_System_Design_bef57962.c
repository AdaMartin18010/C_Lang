/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\06_Embedded_System_Design.md
 * Line: 138
 * Language: c
 * Block ID: bef57962
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ISR尽可能短，只做必要工作
void ADC_IRQHandler(void) {
    uint16_t raw = ADC->DR;
    if (!adc_queue_full()) {
        adc_queue_push(raw);
    }
    ADC->SR &= ~ADC_SR_EOC;
}

// 主循环处理
void main_loop(void) {
    while (1) {
        if (!adc_queue_empty()) {
            process_sensor_data(adc_queue_pop());
        }
        if (can_enter_sleep()) {
            enter_sleep_mode();
        }
    }
}
