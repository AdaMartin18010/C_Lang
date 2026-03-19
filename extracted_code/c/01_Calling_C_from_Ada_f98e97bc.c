/*
 * Auto-generated from: 17_Ada_SPARK\03_Ada_C_Interoperability\01_Calling_C_from_Ada.md
 * Line: 171
 * Language: c
 * Block ID: f98e97bc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int x, y;
    double value;
    char name[32];
} Point;

double calculate_distance(Point* p1, Point* p2);
