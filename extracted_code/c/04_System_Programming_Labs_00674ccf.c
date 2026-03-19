/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 178
 * Language: c
 * Block ID: 00674ccf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void show_progress(off_t current, off_t total) {
    int percent = (int)(current * 100 / total);
    printf("\r[%3d%%] ", percent);
    int pos = percent / 2;
    for (int i = 0; i < 50; i++) {
        putchar(i < pos ? '=' : ' ');
    }
    printf(" %ld/%ld MB", current / 1024 / 1024, total / 1024 / 1024);
    fflush(stdout);
}
