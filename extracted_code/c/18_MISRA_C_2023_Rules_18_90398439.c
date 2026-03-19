/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\18_MISRA_C_2023_Rules_18.md
 * Line: 163
 * Language: c
 * Block ID: 90398439
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 同一数组内比较 */
int arr[100];
int *p = find_element(arr, 100, target);
if (p != NULL && p >= &arr[0] && p < &arr[100]) {
    /* p在有效范围内 */
}

/* ✅ 合规 - NULL检查 */
int *p = get_pointer();
if (p != NULL) {
    use(p);
}

/* ✅ 合规 - 使用索引而非指针比较 */
size_t index1 = 10;
size_t index2 = 20;
if (index1 < index2) {  /* 安全 */ }
