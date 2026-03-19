/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 24
 * Language: c
 * Block ID: ace1665e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int a = 10;
int *p = &a;
int **pp = &p;
printf("%d %d %d\n", a, *p, **pp);
printf("%p %p %p\n", (void*)&a, (void*)p, (void*)*pp);
