/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_P_vs_NP.md
 * Line: 349
 * Language: c
 * Block ID: f62f52bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// BFS - O(V + E)
void bfs(Graph* g, int start) {
    bool visited[MAX_V] = {false};
    Queue q;
    enqueue(&q, start);
    visited[start] = true;

    while (!is_empty(&q)) {
        int v = dequeue(&q);
        for (Node* u = g->adj[v]; u != NULL; u = u->next) {
            if (!visited[u->vertex]) {
                visited[u->vertex] = true;
                enqueue(&q, u->vertex);
            }
        }
    }
}
