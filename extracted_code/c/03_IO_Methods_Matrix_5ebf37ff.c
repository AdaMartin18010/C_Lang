/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_IO_Methods_Matrix.md
 * Line: 67
 * Language: c
 * Block ID: 5ebf37ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单直接，但线程会阻塞
ssize_t n = read(fd, buf, sizeof(buf));
if (n > 0) {
    process_data(buf, n);
}
