/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 211
 * Language: c
 * Block ID: 2e332892
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 规则1: 先连接新节点，再断开旧连接
new_node->next = current->next;  // 先连新
current->next = new_node;         // 再断旧

// 规则2: 删除前先保存下一个节点
Node* temp = current->next;
current->next = temp->next;
free(temp);

// 规则3: 使用虚拟头节点简化边界处理
Node dummy = {0, head};
// 操作完成后返回 dummy.next
