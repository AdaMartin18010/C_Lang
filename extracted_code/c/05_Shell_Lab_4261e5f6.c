/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 213
 * Language: c
 * Block ID: 4261e5f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 解析命令行
int parseline(const char *cmdline, char **argv) {
    static char array[MAXLINE];
    char *buf = array;
    char *delim;
    int argc;
    int bg;

    strcpy(buf, cmdline);
    buf[strlen(buf) - 1] = ' ';  // 替换末尾换行符为空格

    // 跳过前导空格
    while (*buf && (*buf == ' ')) {
        buf++;
    }

    // 解析参数
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;

        // 跳过空格
        while (*buf && (*buf == ' ')) {
            buf++;
        }
    }
    argv[argc] = NULL;

    // 空命令行
    if (argc == 0) {
        return 1;
    }

    // 检查是否是后台作业
    bg = (*argv[argc - 1] == '&');
    if (bg) {
        argv[--argc] = NULL;
    }

    return bg;
}

// 解析重定向
void parse_redirects(char **argv, char **infile, char **outfile) {
    *infile = NULL;
    *outfile = NULL;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            *infile = argv[i + 1];
            argv[i] = NULL;
            i++;
        } else if (strcmp(argv[i], ">") == 0) {
            *outfile = argv[i + 1];
            argv[i] = NULL;
            i++;
        }
    }
}
