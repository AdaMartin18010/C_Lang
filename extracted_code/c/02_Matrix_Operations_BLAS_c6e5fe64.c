/*
 * Auto-generated from: 11_Machine_Learning_C\02_Matrix_Operations_BLAS.md
 * Line: 356
 * Language: c
 * Block ID: c6e5fe64
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 随机初始化
void mat_random_init(Matrix* m) {
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0;
    }
}

// Xavier初始化(适用于神经网络)
void mat_xavier_init(Matrix* m) {
    double scale = sqrt(6.0 / (m->rows + m->cols));
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0 * scale;
    }
}

// He初始化(ReLU激活函数)
void mat_he_init(Matrix* m) {
    double scale = sqrt(2.0 / m->cols);
    int size = m->rows * m->cols;
    for (int i = 0; i < size; i++) {
        m->data[i] = ((double)rand() / RAND_MAX) * scale;
    }
}

// 打印矩阵
void mat_print(const Matrix* m, const char* name) {
    printf("%s (%dx%d):\n", name, m->rows, m->cols);
    for (int i = 0; i < m->rows && i < 6; i++) {
        for (int j = 0; j < m->cols && j < 6; j++) {
            printf("%8.4f ", mat_get(m, i, j));
        }
        if (m->cols > 6) printf("...");
        printf("\n");
    }
    if (m->rows > 6) printf("...\n");
}
