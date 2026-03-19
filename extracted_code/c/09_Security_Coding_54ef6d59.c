/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 144
 * Language: c
 * Block ID: 54ef6d59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 验证用户ID（只能是数字，1-1000000）
int64_t user_id;
ValidationResult r = validate_int(user_input, 1, 1000000, &user_id);
if (r != VAL_OK) {
    log_security_event("Invalid user ID input", user_input);
    return ERROR_INVALID_INPUT;
}
