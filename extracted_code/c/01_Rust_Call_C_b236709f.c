/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\01_Rust_Call_C.md
 * Line: 144
 * Language: c
 * Block ID: b236709f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mathlib.c */
#include "mathlib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int32_t add(int32_t a, int32_t b) {
    return a + b;
}

int32_t subtract(int32_t a, int32_t b) {
    return a - b;
}

Point* point_new(double x, double y) {
    Point *p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    return p;
}

void point_free(Point *p) {
    free(p);
}

double point_distance(const Point *a, const Point *b) {
    double dx = a->x - b->x;
    double dy = a->y - b->y;
    return sqrt(dx * dx + dy * dy);
}

const char* get_version(void) {
    return "mathlib v1.0.0";
}

char* reverse_string(const char *input) {
    size_t len = strlen(input);
    char *result = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        result[i] = input[len - 1 - i];
    }
    result[len] = '\0';
    return result;
}

void free_string(char *s) {
    free(s);
}

IntArray* array_new(size_t capacity) {
    IntArray *arr = malloc(sizeof(IntArray));
    arr->data = malloc(capacity * sizeof(int32_t));
    arr->len = 0;
    arr->capacity = capacity;
    return arr;
}

void array_push(IntArray *arr, int32_t value) {
    if (arr->len < arr->capacity) {
        arr->data[arr->len++] = value;
    }
}

int32_t array_get(const IntArray *arr, size_t index) {
    return arr->data[index];
}

void array_free(IntArray *arr) {
    free(arr->data);
    free(arr);
}
