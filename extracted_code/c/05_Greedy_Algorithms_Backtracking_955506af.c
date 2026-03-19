/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\05_Greedy_Algorithms_Backtracking.md
 * Line: 394
 * Language: c
 * Block ID: 955506af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ==================== 霍夫曼编码 ====================

typedef struct HuffmanNode {
    char data;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

HuffmanNode* create_huffman_node(char data, int freq) {
    HuffmanNode *node = malloc(sizeof(HuffmanNode));
    node->data = data;
    node->frequency = freq;
    node->left = node->right = NULL;
    return node;
}

// 最小堆节点比较
int compare_huffman_nodes(const void *a, const void *b) {
    HuffmanNode *n1 = *(HuffmanNode **)a;
    HuffmanNode *n2 = *(HuffmanNode **)b;
    return n1->frequency - n2->frequency;
}

/**
 * @brief 构建霍夫曼树
 * @param data 字符数组
 * @param freq 频率数组
 * @param n 字符数量
 * @return 霍夫曼树根节点
 */
HuffmanNode* build_huffman_tree(const char data[], const int freq[], int n) {
    HuffmanNode **heap = malloc(n * sizeof(HuffmanNode *));

    // 初始化堆
    for (int i = 0; i < n; i++) {
        heap[i] = create_huffman_node(data[i], freq[i]);
    }
    int heap_size = n;

    // 构建霍夫曼树
    while (heap_size > 1) {
        // 排序，取最小的两个
        qsort(heap, heap_size, sizeof(HuffmanNode *), compare_huffman_nodes);

        HuffmanNode *left = heap[0];
        HuffmanNode *right = heap[1];

        // 创建新内部节点
        HuffmanNode *parent = create_huffman_node('\0',
                                                  left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        // 更新堆
        heap[0] = parent;
        heap[1] = heap[--heap_size];
    }

    HuffmanNode *root = heap[0];
    free(heap);
    return root;
}

/**
 * @brief 生成霍夫曼编码
 * @param root 霍夫曼树根
 * @param codes 输出的编码数组
 * @param code 当前编码缓冲区
 * @param depth 当前深度
 */
void generate_huffman_codes(HuffmanNode *root, char codes[256][32],
                            char code[], int depth) {
    if (root == NULL) return;

    // 叶子节点
    if (root->left == NULL && root->right == NULL) {
        code[depth] = '\0';
        strcpy(codes[(unsigned char)root->data], code);
        return;
    }

    // 左子树编码0
    code[depth] = '0';
    generate_huffman_codes(root->left, codes, code, depth + 1);

    // 右子树编码1
    code[depth] = '1';
    generate_huffman_codes(root->right, codes, code, depth + 1);
}

void free_huffman_tree(HuffmanNode *root) {
    if (root == NULL) return;
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}
