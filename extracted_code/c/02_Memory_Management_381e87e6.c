/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 869
 * Language: c
 * Block ID: 381e87e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 模式4: 结构体数组分配
 */
typedef struct {
    int x, y;
    char label[16];
} Point;

// 正确方式: 使用sizeof对象，非sizeof指针
Point *create_point_array(size_t n) {
    Point *points = calloc(n, sizeof(Point));  // ✅ 正确
    // Point *points = calloc(n, sizeof(Point*)); // ❌ 错误: 只分配指针大小
    return points;
}

// 带初始化的分配
Point *create_initialized_points(size_t n, int default_x, int default_y) {
    Point *points = malloc(n * sizeof(Point));
    if (points == NULL) return NULL;

    for (size_t i = 0; i < n; i++) {
        points[i].x = default_x;
        points[i].y = default_y;
        snprintf(points[i].label, sizeof(points[i].label), "P%zu", i);
    }
    return points;
}
