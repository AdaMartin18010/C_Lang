/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\01_Linked_List_Complete_Guide.md
 * Line: 584
 * Language: c
 * Block ID: 2d598bd6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 使用快慢指针检测链表是否有环 (Floyd判圈算法)
 * @param head 链表头指针
 * @return 有环返回 true，无环返回 false
 *
 * 算法原理:
 * - 快指针每次走2步，慢指针每次走1步
 * - 如果有环，快指针最终会追上慢指针
 * - 时间复杂度: O(n)，空间复杂度: O(1)
 */
bool list_has_cycle(const Node* head) {
    if (head == nullptr || head->next == nullptr) {
        return false;
    }

    const Node *slow = head;
    const Node *fast = head;

    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;           // 慢指针走1步
        fast = fast->next->next;     // 快指针走2步

        if (slow == fast) {
            return true;  // 相遇，有环
        }
    }

    return false;  // 快指针到达末尾，无环
}

/**
 * @brief 查找环的入口节点
 * @param head 链表头指针
 * @return 环的入口节点，无环返回 nullptr
 *
 * 数学推导:
 * 设头到入口距离为a，入口到相遇点距离为b，环周长为c
 * 慢指针走的距离: a + b
 * 快指针走的距离: a + b + k*c (k为圈数)
 * 由 2*(a+b) = a + b + k*c 得 a = k*c - b
 * 所以从头和相遇点同时走，会在入口相遇
 */
Node* list_find_cycle_entry(Node* head) {
    if (head == nullptr || head->next == nullptr) {
        return nullptr;
    }

    Node *slow = head;
    Node *fast = head;
    bool has_cycle = false;

    // 第一步: 检测是否有环
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            has_cycle = true;
            break;
        }
    }

    if (!has_cycle) return nullptr;

    // 第二步: 找入口
    slow = head;  // 慢指针回到头
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }

    return slow;  // 相遇点即入口
}
