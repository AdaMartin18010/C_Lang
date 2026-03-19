/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 185
 * Language: c
 * Block ID: e301b36e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y defer
void process_file_c2y(const char *filename) {
    FILE *fp = fopen(filename, "r");
    defer fclose(fp);

    char *buffer = malloc(1024);
    defer free(buffer);

    // 业务逻辑...
}
