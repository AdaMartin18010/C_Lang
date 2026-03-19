/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 521
 * Language: c
 * Block ID: 6b8310db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void block_scope_example(void) {
    printf("Entering function\n");

    {
        FILE *fp = fopen("temp.txt", "w");
        defer fclose(fp);  // 只在当前块结束时执行

        fprintf(fp, "temporary data\n");
        printf("Inside block\n");
    }  // defer在这里执行

    printf("Outside block\n");
    // 继续执行，fp已关闭
}
