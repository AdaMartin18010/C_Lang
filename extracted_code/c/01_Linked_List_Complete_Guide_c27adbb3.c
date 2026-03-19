/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 997
 * Language: c
 * Block ID: c27adbb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 数组遍历 - 缓存友好
int arr[10000];
for (int i = 0; i < 10000; i++) {
    sum += arr[i];  // 顺序访问，预取有效
}

// 链表遍历 - 缓存不友好
Node* p = head;
while (p != nullptr) {
    sum += p->data;  // 随机访问，缓存未命中率高
    p = p->next;
}
