/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 452
 * Language: c
 * Block ID: ff07134d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 组件接口 */
typedef struct component {
    struct component* parent;
    void (*render)(struct component*);
    void (*add)(struct component*, struct component*);
    void (*remove)(struct component*, struct component*);
} component_t;

/* 叶子: 按钮 */
typedef struct {
    component_t base;
    char label[64];
} button_t;

void button_render(component_t* self) {
    button_t* btn = (button_t*)self;
    printf("Button: %s\n", btn->label);
}

/* 组合: 面板 */
typedef struct {
    component_t base;
    component_t** children;
    int child_count;
    int capacity;
} panel_t;

void panel_render(component_t* self) {
    panel_t* panel = (panel_t*)self;
    printf("Panel {\n");
    for (int i = 0; i < panel->child_count; i++) {
        panel->children[i]->render(panel->children[i]);
    }
    printf("}\n");
}

void panel_add(component_t* self, component_t* child) {
    panel_t* panel = (panel_t*)self;
    if (panel->child_count >= panel->capacity) {
        panel->capacity *= 2;
        panel->children = realloc(panel->children,
                                  sizeof(component_t*) * panel->capacity);
    }
    panel->children[panel->child_count++] = child;
    child->parent = self;
}

/* 使用 */
void example_composite(void) {
    panel_t* root = panel_create("root");
    panel_t* sidebar = panel_create("sidebar");
    panel_t* main = panel_create("main");

    button_t* btn1 = button_create("Submit");
    button_t* btn2 = button_create("Cancel");

    sidebar->base.add((component_t*)sidebar, (component_t*)btn1);
    main->base.add((component_t*)main, (component_t*)btn2);

    root->base.add((component_t*)root, (component_t*)sidebar);
    root->base.add((component_t*)root, (component_t*)main);

    /* 统一渲染整棵树 */
    root->base.render((component_t*)root);
}
