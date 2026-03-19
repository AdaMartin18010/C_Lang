/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\README.md
 * Line: 203
 * Language: c
 * Block ID: 2f58ad8e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* c_concepts.c - C语言核心概念实体 */
#include "knowledge_graph.h"

/* 基础概念 */
void create_basic_concepts(knowledge_graph_t *kg) {
    /* 变量与类型 */
    entity_t *variables = kg_add_entity(kg, "变量",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    kg_add_attr(variables, "定义", "命名的内存存储区域");
    kg_add_attr(variables, "关键字", "auto, static, extern, register");

    entity_t *data_types = kg_add_entity(kg, "数据类型",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);

    entity_t *integers = kg_add_entity(kg, "整数类型",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *floats = kg_add_entity(kg, "浮点类型",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *pointers = kg_add_entity(kg, "指针",
        ENTITY_CONCEPT, LEVEL_INTERMEDIATE);
    pointers->importance = 10;
    pointers->estimated_hours = 40;

    /* 建立关系 */
    kg_add_relation(kg, data_types, integers, REL_CONTAINS, 1.0);
    kg_add_relation(kg, data_types, floats, REL_CONTAINS, 1.0);
    kg_add_relation(kg, pointers, data_types, REL_RELATED, 0.8);

    /* 控制流 */
    entity_t *control_flow = kg_add_entity(kg, "控制流",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);

    entity_t *conditionals = kg_add_entity(kg, "条件语句",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *loops = kg_add_entity(kg, "循环语句",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *switches = kg_add_entity(kg, "switch语句",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *gotos = kg_add_entity(kg, "goto语句",
        ENTITY_TECHNIQUE, LEVEL_INTERMEDIATE);

    kg_add_relation(kg, control_flow, conditionals, REL_CONTAINS, 1.0);
    kg_add_relation(kg, control_flow, loops, REL_CONTAINS, 1.0);
    kg_add_relation(kg, control_flow, switches, REL_CONTAINS, 1.0);
    kg_add_relation(kg, control_flow, gotos, REL_CONTAINS, 0.5);

    /* 函数 */
    entity_t *functions = kg_add_entity(kg, "函数",
        ENTITY_CONCEPT, LEVEL_FUNDAMENTAL);
    entity_t *recursion = kg_add_entity(kg, "递归",
        ENTITY_TECHNIQUE, LEVEL_INTERMEDIATE);
    entity_t *callbacks = kg_add_entity(kg, "回调函数",
        ENTITY_TECHNIQUE, LEVEL_INTERMEDIATE);
    entity_t *variadic = kg_add_entity(kg, "可变参数",
        ENTITY_TECHNIQUE, LEVEL_ADVANCED);

    kg_add_relation(kg, functions, recursion, REL_CONTAINS, 1.0);
    kg_add_relation(kg, functions, callbacks, REL_CONTAINS, 1.0);
    kg_add_relation(kg, functions, variadic, REL_CONTAINS, 0.8);
    kg_add_relation(kg, pointers, callbacks, REL_PREREQUISITE, 1.0);
}
