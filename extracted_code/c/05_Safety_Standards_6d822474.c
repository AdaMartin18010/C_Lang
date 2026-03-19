/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\05_Safety_Standards.md
 * Line: 291
 * Language: c
 * Block ID: 6d822474
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 合规：使用const保护输入 (Rule 8.13)
void process_data(const uint8_t *data, size_t len);

// ❌ 违反规则：可变参数 (Rule 17.1)
void log(const char *fmt, ...);  // 禁止

// ✅ 合规：固定参数
void log_message(const char *msg);
void log_formatted(const char *msg, uint32_t arg1, uint32_t arg2);

// ❌ 违反规则：函数过长 (Rule 15.5)
// 函数超过60行，或圈复杂度过高

// ✅ 合规：分解为小函数
static void helper1(void) { /* ... */ }
static void helper2(void) { /* ... */ }
void main_func(void) {
    helper1();
    helper2();
}
