/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 3142
 * Language: c
 * Block ID: beccd8c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ========================================
 * C89 版本
 * ======================================== */
#ifdef USE_C89

struct ListNode {
    void* data;
    struct ListNode* next;
};

struct List {
    struct ListNode* head;
    struct ListNode* tail;
    int count;
};

void list_init(struct List* list) {
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

int list_append(struct List* list, void* data) {
    struct ListNode* node = malloc(sizeof(struct ListNode));
    if (node == NULL) return -1;

    node->data = data;
    node->next = NULL;

    if (list->tail == NULL) {
        list->head = node;
    } else {
        list->tail->next = node;
    }
    list->tail = node;
    list->count++;

    return 0;
}

/* ========================================
 * C99 版本
 * ======================================== */
#elif defined(USE_C99)

typedef struct ListNode {
    void* data;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* head;
    ListNode* tail;
    int count;
} List;

List list_new(void) {
    return (List){ NULL, NULL, 0 };  /* 复合字面量 */
}

void list_append(List* list, void* data) {
    ListNode* node = malloc(sizeof(ListNode));
    *node = (ListNode){ data, NULL };  /* 指定初始化器 */

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->count++;
}

/* ========================================
 * C11 版本：泛型选择
 * ======================================== */
#elif defined(USE_C11)

#define list_new() _Generic(0, \
    default: list_new_impl \
)()

#define list_append(list, data) _Generic((data), \
    int*: list_append_int, \
    char*: list_append_string, \
    default: list_append_generic \
)(list, data)

/* ========================================
 * C23 版本
 * ======================================== */
#elif defined(USE_C23)

typedef struct {
    auto data;
    typeof(data) next;
} GenericNode;

#define list_new() (List){ .head = nullptr, .tail = nullptr, .count = 0 }

[[nodiscard]]
int list_append(List* list, auto* data) {
    auto node = malloc(sizeof(typeof(*node)));
    if (node == nullptr) return -1;

    *node = (typeof(*node)){ .data = data, .next = nullptr };

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->count++;

    return 0;
}

#endif
