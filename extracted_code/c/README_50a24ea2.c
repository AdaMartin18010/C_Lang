/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 329
 * Language: c
 * Block ID: 50a24ea2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// addition_plugin.c - 加法插件实现
#include "calculator.h"

static double add_impl(double a, double b) {
    return a + b;
}

static OperationPlugin add_plugin = {
    .name = "add",
    .execute = add_impl,
    .priority = 1
};

__attribute__((constructor))
void register_add_plugin() {
    plugin_register(&add_plugin);
}
