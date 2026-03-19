/*
 * Auto-generated from: 06_Thinking_Representation\02_Multidimensional_Matrix\01_Standards_Comparison_Matrix.md
 * Line: 922
 * Language: c
 * Block ID: 76bc7f02
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C89：需要malloc/free */
void process_c89(int n) {
    int* arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        /* 错误处理 */
        return;
    }
    /* 使用arr... */
    free(arr);
}

/* C99：使用VLA */
void process_c99(int n) {
    int arr[n];  /* VLA：自动分配 */
    /* 使用arr... */
    /* 自动释放，无需free */
}
