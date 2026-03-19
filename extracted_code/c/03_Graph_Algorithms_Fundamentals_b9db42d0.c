/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\03_Graph_Algorithms_Fundamentals.md
 * Line: 439
 * Language: c
 * Block ID: b9db42d0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 邻接矩阵转邻接表
 */
void matrix_to_list(const AdjMatrixGraph *mat, AdjListGraph *list) {
    init_adj_list_graph(list, mat->num_vertices);

    for (int i = 0; i < mat->num_vertices; i++) {
        for (int j = 0; j < mat->num_vertices; j++) {
            if (mat->adj_matrix[i][j] != INF && mat->adj_matrix[i][j] != 0) {
                // 为避免重复添加无向边，只处理i < j的情况
                if (i < j) {
                    add_undirected_edge_list(list, i, j, mat->adj_matrix[i][j]);
                }
            }
        }
    }
}

/**
 * @brief 邻接表转邻接矩阵
 */
void list_to_matrix(const AdjListGraph *list, AdjMatrixGraph *mat) {
    init_adj_matrix_graph(mat, list->num_vertices);

    for (int i = 0; i < list->num_vertices; i++) {
        EdgeNode *curr = list->vertices[i].head;
        while (curr) {
            mat->adj_matrix[i][curr->dest] = curr->weight;
            curr = curr->next;
        }
    }

    mat->num_edges = list->num_edges;
}
