/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 97
 * Language: c
 * Block ID: 80efe7d5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 参数过多 */
void create_window(int x, int y, int width, int height,
                   bool visible, bool resizable, bool fullscreen,
                   const char *title, int style);

/* ✅ 使用结构体 */
typedef struct {
    int x, y;
    int width, height;
    bool visible;
    bool resizable;
    bool fullscreen;
    const char *title;
    int style;
} WindowConfig;

void create_window(const WindowConfig *config);

/* 使用 */
WindowConfig cfg = {
    .x = 100, .y = 100,
    .width = 800, .height = 600,
    .visible = true,
    .title = "My App"
};
create_window(&cfg);
