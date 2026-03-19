/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 911
 * Language: c
 * Block ID: 037857bb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：循环内重复计算
void transform_slow(const double *input, double *output, int n, double scale) {
    for (int i = 0; i < n; i++) {
        // log(scale) 每次循环都计算
        output[i] = input[i] * log(scale) + sin(3.14159 / 4);
    }
}

// ✅ 优化后：循环外预计算
void transform_fast(const double *input, double *output, int n, double scale) {
    double log_scale = log(scale);           // 循环外计算
    double sin_val = sin(M_PI / 4);          // 循环外计算

    // 循环展开（进一步性能提升）
    int i = 0;
    // 每次处理4个元素
    for (; i <= n - 4; i += 4) {
        output[i]   = input[i]   * log_scale + sin_val;
        output[i+1] = input[i+1] * log_scale + sin_val;
        output[i+2] = input[i+2] * log_scale + sin_val;
        output[i+3] = input[i+3] * log_scale + sin_val;
    }
    // 处理剩余元素
    for (; i < n; i++) {
        output[i] = input[i] * log_scale + sin_val;
    }
}

// ❌ 错误示例：函数调用在循环内
int sum_values_slow(node_t *head) {
    int sum = 0;
    for (node_t *p = head; p != NULL; p = get_next(p)) {  // 每次调用函数
        sum += get_value(p);  // 每次调用函数
    }
    return sum;
}

// ✅ 优化后：内联热点代码
typedef struct node {
    int value;
    struct node *next;
} node_t;

int sum_values_fast(node_t *head) {
    int sum = 0;
    // 直接访问成员，无函数调用开销
    for (node_t *p = head; p != NULL; p = p->next) {
        sum += p->value;
    }
    return sum;
}
