/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 84
 * Language: c
 * Block ID: db6016c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C语言推荐: 模块级别的单例 */
/* module.c */

static struct {
    int config_value;
    void* internal_state;
} module_state = {
    .config_value = 42,
    .internal_state = NULL
};

/* 不需要显式单例，静态变量本身就是 */
int module_get_config(void) {
    return module_state.config_value;
}

void module_set_config(int value) {
    module_state.config_value = value;
}
