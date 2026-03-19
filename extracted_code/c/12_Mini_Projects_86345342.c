/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 380
 * Language: c
 * Block ID: 86345342
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 流式处理大CSV文件
CSVParser* csv_open(const char *path);
void csv_set_delimiter(CSVParser *parser, char delim);

// 逐行读取
while (csv_read_row(parser, &row)) {
    for (int i = 0; i < row.count; i++) {
        printf("%s\t", row.fields[i]);
    }
    printf("\n");
}

// 聚合查询（类似SQL）
CSVResult* csv_query(CSVParser *parser, const char *sql_like_query);
