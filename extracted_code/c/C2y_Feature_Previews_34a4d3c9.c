/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 383
 * Language: c
 * Block ID: 34a4d3c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 场景1: 配置解析
void load_config(void) {
    Config cfg = {
        .port = get_env("PORT") ?: "8080",
        .host = get_env("HOST") ?: "localhost",
        .debug = strcmp(get_env("DEBUG") ?: "false", "true") == 0
    };
}

// 场景2: 资源获取
Resource* acquire_resource(int priority) {
    return try_acquire_high(priority)
        ?: try_acquire_medium(priority)
        ?: try_acquire_low(priority)
        ?: create_fallback_resource();
}

// 场景3: 错误处理
ErrorCode do_operation(void) {
    return validate_input()
        ?: check_permissions()
        ?: perform_action()
        ?: notify_completion();
    // 每个函数返回0表示成功，错误码表示失败
}

// 场景4: 字符串处理
char* trim_or_default(char* str, char* def) {
    char* trimmed = trim(str);
    return (trimmed && *trimmed) ?: def;
}
