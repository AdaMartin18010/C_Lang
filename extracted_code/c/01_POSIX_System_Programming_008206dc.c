/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 4838
 * Language: c
 * Block ID: 008206dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file test_shell.c
 * @brief Shell功能测试
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/* 测试辅助函数 */

/**
 * @brief 测试命令解析
 */
void test_parse_args(void) {
    printf("测试命令解析...\n");
    
    char line[] = "ls -la /home";
    char *args[256];
    int argc = 0;
    
    char *p = line;
    while (*p && argc < 255) {
        while (*p && *p == ' ') p++;
        if (!*p) break;
        args[argc] = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = '\0';
        argc++;
    }
    args[argc] = NULL;
    
    assert(argc == 3);
    assert(strcmp(args[0], "ls") == 0);
    assert(strcmp(args[1], "-la") == 0);
    assert(strcmp(args[2], "/home") == 0);
    
    printf("命令解析测试通过\n");
}

/**
 * @brief 测试管道解析
 */
void test_parse_pipes(void) {
    printf("测试管道解析...\n");
    
    char line[] = "cat file.txt | grep pattern | wc -l";
    char *commands[16];
    int num = 0;
    
    char *token = strtok(line, "|");
    while (token && num < 16) {
        commands[num++] = token;
        token = strtok(NULL, "|");
    }
    
    assert(num == 3);
    printf("管道解析测试通过\n");
}

/**
 * @brief 测试重定向检测
 */
void test_redirect_detection(void) {
    printf("测试重定向检测...\n");
    
    char line[] = "cat < input.txt > output.txt";
    assert(strstr(line, "<") != NULL);
    assert(strstr(line, ">") != NULL);
    
    printf("重定向检测测试通过\n");
}

/**
 * @brief 测试环境变量处理
 */
void test_environment(void) {
    printf("测试环境变量...\n");
    
    setenv("TEST_VAR", "test_value", 1);
    char *val = getenv("TEST_VAR");
    assert(val != NULL);
    assert(strcmp(val, "test_value") == 0);
    
    unsetenv("TEST_VAR");
    assert(getenv("TEST_VAR") == NULL);
    
    printf("环境变量测试通过\n");
}

/**
 * @brief 运行所有测试
 */
void run_tests(void) {
    printf("\n=== 运行Shell功能测试 ===\n\n");
    
    test_parse_args();
    test_parse_pipes();
    test_redirect_detection();
    test_environment();
    
    printf("\n=== 所有测试通过 ===\n");
}

int main(void) {
    run_tests();
    return 0;
}
