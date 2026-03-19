/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 281
 * Language: c
 * Block ID: b5265ce9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单体架构示例 - 简单的计算器
// 所有功能集中在一个文件中

#include <stdio.h>

typedef struct {
    double (*add)(double, double);
    double (*subtract)(double, double);
    double (*multiply)(double, double);
    double (*divide)(double, double);
} Calculator;

double add(double a, double b) { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b) { return b != 0 ? a / b : 0; }

Calculator* calculator_create() {
    Calculator* calc = malloc(sizeof(Calculator));
    calc->add = add;
    calc->subtract = subtract;
    calc->multiply = multiply;
    calc->divide = divide;
    return calc;
}
