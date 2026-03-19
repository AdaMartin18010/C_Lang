/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\README.md
 * Line: 503
 * Language: c
 * Block ID: f3233bf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* graph_export.c - 导出为Graphviz格式 */
#include "knowledge_graph.h"
#include <stdio.h>
#include <string.h>

static const char *entity_type_color(entity_type_t type) {
    switch (type) {
        case ENTITY_CONCEPT: return "lightblue";
        case ENTITY_TECHNIQUE: return "lightgreen";
        case ENTITY_TOOL: return "orange";
        case ENTITY_STANDARD: return "yellow";
        case ENTITY_PROBLEM: return "red";
        default: return "white";
    }
}

static const char *relation_style(relation_type_t type) {
    switch (type) {
        case REL_PREREQUISITE: return "[color=red,penwidth=2]";
        case REL_CONTAINS: return "[color=blue,style=dashed]";
        case REL_RELATED: return "[color=gray]";
        case REL_APPLIES_TO: return "[color=green]";
        default: return "";
    }
}

int kg_export_dot(knowledge_graph_t *kg, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;

    fprintf(fp, "digraph C_Knowledge_Graph {\n");
    fprintf(fp, "  rankdir=TB;\n");
    fprintf(fp, "  node [shape=box,style=filled];\n\n");

    /* 输出节点 */
    for (uint32_t i = 0; i < kg->entity_count; i++) {
        entity_t *e = kg->entities[i];
        fprintf(fp, "  \"%s\" [fillcolor=%s,label=\"%s\\n(%s)\"];\n",
                e->name,
                entity_type_color(e->type),
                e->name,
                e->difficulty == 1 ? "基础" :
                e->difficulty == 2 ? "中级" :
                e->difficulty == 3 ? "高级" : "专家");
    }

    fprintf(fp, "\n");

    /* 输出边 */
    for (uint32_t i = 0; i < kg->relation_count; i++) {
        relation_t *r = kg->relations[i];
        fprintf(fp, "  \"%s\" -> \"%s\" %s [label=\"%s\"];\n",
                r->from->name,
                r->to->name,
                relation_style(r->type),
                r->type == REL_PREREQUISITE ? "需要" :
                r->type == REL_CONTAINS ? "包含" :
                r->type == REL_RELATED ? "相关" : "");
    }

    fprintf(fp, "}\n");
    fclose(fp);
    return 0;
}
