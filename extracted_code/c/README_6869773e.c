/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 309
 * Language: c
 * Block ID: 6869773e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 插件架构示例 - 可扩展的计算器
// calculator.h - 插件接口定义
#ifndef CALCULATOR_H
#define CALCULATOR_H

typedef struct {
    const char* name;
    double (*execute)(double, double);
    int priority;
} OperationPlugin;

// 插件注册系统
void plugin_register(OperationPlugin* plugin);
OperationPlugin* plugin_find(const char* name);
void plugin_execute(const char* op, double a, double b);

#endif
