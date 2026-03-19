/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 155
 * Language: c
 * Block ID: 3d2d0b30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 危险示例：混用标准 I/O 和系统调用 */
void dangerous_io_mix() {
    FILE *fp = fopen("test.txt", "w");
    int fd = fileno(fp);  /* 获取底层文件描述符 */

    fprintf(fp, "Hello, ");    /* 写入缓冲区 */
    write(fd, "World!\n", 7);  /* 直接系统调用 */

    /* 危险：输出顺序不确定！
     * 可能是 "World!\nHello, "
     * 或 "Hello, World!\n" (如果缓冲区先刷新)
     */
    fclose(fp);
}

/* 安全做法：使用 fflush 同步 */
void safe_io_mix() {
    FILE *fp = fopen("test.txt", "w");
    int fd = fileno(fp);

    fprintf(fp, "Hello, ");
    fflush(fp);  /* 确保缓冲区内容写入 */
    write(fd, "World!\n", 7);

    fclose(fp);
}
