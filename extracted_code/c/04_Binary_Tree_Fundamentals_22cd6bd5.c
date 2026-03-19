/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\04_Binary_Tree_Fundamentals.md
 * Line: 1203
 * Language: c
 * Block ID: 22cd6bd5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 二叉树的序列化与反序列化
 * 用于存储和网络传输
 */

// 前序序列化
void tree_serialize_preorder(TreeNode* root, char* buffer, int* pos) {
    if (root == nullptr) {
        buffer[(*pos)++] = '#';
        buffer[(*pos)++] = ',';
        return;
    }

    // 写入当前节点值
    int n = sprintf(buffer + *pos, "%d,", root->data);
    *pos += n;

    tree_serialize_preorder(root->left, buffer, pos);
    tree_serialize_preorder(root->right, buffer, pos);
}

// 前序反序列化
TreeNode* tree_deserialize_preorder(char* data, int* pos) {
    if (data[*pos] == '#') {
        *pos += 2;  // 跳过 "#,"
        return nullptr;
    }

    // 读取数值
    int value;
    sscanf(data + *pos, "%d,", &value);

    // 找到逗号位置
    while (data[*pos] != ',') (*pos)++;
    (*pos)++;

    TreeNode* node = tree_node_create(value);
    node->left = tree_deserialize_preorder(data, pos);
    node->right = tree_deserialize_preorder(data, pos);

    return node;
}

// 层序序列化 (LeetCode格式)
void tree_serialize_levelorder(TreeNode* root, char* buffer) {
    if (root == nullptr) {
        strcpy(buffer, "[]");
        return;
    }

    int pos = 0;
    buffer[pos++] = '[';

    TreeNode** queue = (TreeNode**)malloc(1024 * sizeof(TreeNode*));
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        TreeNode* node = queue[front++];

        if (node == nullptr) {
            pos += sprintf(buffer + pos, "null,");
        } else {
            pos += sprintf(buffer + pos, "%d,", node->data);
            queue[rear++] = node->left;
            queue[rear++] = node->right;
        }
    }

    // 移除末尾的null和逗号
    while (pos > 1 && (buffer[pos-1] == ',' || buffer[pos-1] == 'l' ||
           buffer[pos-2] == 'l' && buffer[pos-1] == 'l')) {
        if (buffer[pos-1] == 'l') pos -= 4;  // "null"
        else pos--;
    }

    if (buffer[pos-1] == ',') pos--;
    buffer[pos++] = ']';
    buffer[pos] = '\0';

    free(queue);
}
