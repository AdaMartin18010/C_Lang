/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 2326
 * Language: c
 * Block ID: ed32a904
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 迁移示例：逐步现代化代码 */

/* 迁移前 (C89风格) */
int old_style_function(x, y)
int x;
int y;
{
    int i;
    int result = 0;

    for (i = 0; i < x; i++) {
        result += y;
    }

    return result;
}

/* 迁移后 (C23风格) */
[[nodiscard]]
constexpr int modern_function(int x, int y)
{
    auto result = 0;  /* 类型推导为 int */

    for (auto i = 0; i < x; i++) {  /* 循环变量在for中声明 */
        result += y;
    }

    return result;
}

/* 复杂示例：链表操作 */

/* 迁移前 */
struct Node {
    int data;
    struct Node* next;
};

struct List {
    struct Node* head;
    struct Node* tail;
    int count;
};

void list_init(struct List* list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

/* 迁移后 */
typedef struct Node Node;
struct Node {
    int data;
    Node* next;
};

typedef struct {
    Node* head;
    Node* tail;
    size_t count;
} List;

[[nodiscard]]
List list_create(void) {
    return (List){  /* 复合字面量 + 指定初始化器 */
        .head = nullptr,
        .tail = nullptr,
        .count = 0
    };
}
