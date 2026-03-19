/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 179
 * Language: c
 * Block ID: cbc9f4db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* UI组件工厂 */

typedef struct button button_t;
typedef struct checkbox checkbox_t;

typedef struct {
    button_t* (*create_button)(void);
    checkbox_t* (*create_checkbox)(void);
} ui_factory_t;

/* Windows实现 */
typedef struct {
    void (*render)(void);
} win_button_t;

button_t* win_create_button(void) {
    win_button_t* btn = malloc(sizeof(win_button_t));
    btn->render = win_button_render;
    return (button_t*)btn;
}

ui_factory_t* create_windows_factory(void) {
    static ui_factory_t factory = {
        .create_button = win_create_button,
        .create_checkbox = win_create_checkbox
    };
    return &factory;
}

/* Linux实现 */
ui_factory_t* create_linux_factory(void) {
    static ui_factory_t factory = {
        .create_button = linux_create_button,
        .create_checkbox = linux_create_checkbox
    };
    return &factory;
}
