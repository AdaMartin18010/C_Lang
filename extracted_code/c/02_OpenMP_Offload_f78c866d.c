/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\11_Heterogeneous_Memory\02_OpenMP_Offload.md
 * Line: 714
 * Language: c
 * Block ID: f78c866d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：数组切片范围不正确
void wrong_slice(void) {
    float arr[100][100];

    // ❌ arr[0:50] 只映射前50个float，不是50行
    #pragma omp target map(tofrom: arr[0:50])
    {
        arr[10][20] = 1.0f;  // 越界访问！
    }
}

// 正确：计算总元素数
void correct_slice(void) {
    float arr[100][100];

    // 映射50行：50 * 100 = 5000个元素
    #pragma omp target map(tofrom: arr[0:5000])
    {
        arr[10][20] = 1.0f;  // 正确
    }
}

// 或使用C99 VLA语法（如果编译器支持）
void vla_slice(void) {
    int n = 100, m = 100;
    float arr[n][m];

    #pragma omp target map(tofrom: arr[0:50][0:m])
    {
        arr[10][20] = 1.0f;
    }
}
