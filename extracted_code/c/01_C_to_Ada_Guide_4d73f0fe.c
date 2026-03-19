/*
 * Auto-generated from: 17_Ada_SPARK\06_Migration_from_C\01_C_to_Ada_Guide.md
 * Line: 236
 * Language: c
 * Block ID: 4d73f0fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int x;
    int y;
    char name[32];
} Point;

Point p = {10, 20, "origin"};
printf("Point %s at (%d,%d)\n", p.name, p.x, p.y);
