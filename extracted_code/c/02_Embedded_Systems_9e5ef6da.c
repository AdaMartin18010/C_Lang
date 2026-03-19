/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Systems.md
 * Line: 396
 * Language: c
 * Block ID: 9e5ef6da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：编译器可能优化掉循环
void wait_for_button(void) {
    while (GPIOA->IDR & (1 << 0)) {
        // 等待按键按下
    }
}
// 编译器可能认为IDR不变，优化为无限循环

// ✅ 正确：使用volatile
void wait_for_button_safe(void) {
    while (((volatile GPIO_TypeDef *)GPIOA)->IDR & (1 << 0)) {
        // 或声明为volatile指针
    }
}
