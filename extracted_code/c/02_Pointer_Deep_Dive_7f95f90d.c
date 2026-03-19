/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 293
 * Language: c
 * Block ID: 7f95f90d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 下标方式
for (int i = 0; i < n; i++) {
    sum += arr[i];      // 编译为: *(arr + i * sizeof(int))
}

// 指针方式
for (int *p = arr; p < arr + n; p++) {
    sum += *p;          // 直接递增指针
}
