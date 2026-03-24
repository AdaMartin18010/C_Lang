/*
 * C2y if 声明语句示例
 * 编译: gcc -std=c2y -o 14_if_declaration 14_if_declaration.c
 *       clang -std=c2y -o 14_if_declaration 14_if_declaration.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 简化错误处理的模式
int process_file_improved(const char *filename) {
    // C2y: if with declaration
    if (FILE *fp = fopen(filename, "r"); fp != NULL) {
        // fp 在此作用域内有效
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), fp)) {
            printf("%s", buffer);
        }
        fclose(fp);
        return 0;
    } else {
        // fp 仍然可见，但为 NULL
        perror("Failed to open file");
        return -1;
    }
    // fp 在这里不可见
}

// 读取配置值
int read_config_value(const char *key, int default_value) {
    // 尝试从环境变量读取
    if (const char *env = getenv(key); env != NULL) {
        // 尝试解析为整数
        char *endptr;
        long val = strtol(env, &endptr, 10);
        if (*endptr == '\0') {
            return (int)val;
        }
    }
    return default_value;
}

// switch with declaration
void classify_character(int c) {
    switch (int upper = toupper(c); upper) {
        case 'A': case 'E': case 'I': case 'O': case 'U':
            printf("'%c' is a vowel\n", c);
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            printf("'%c' is a digit\n", c);
            break;
        default:
            if (isalpha(upper)) {
                printf("'%c' is a consonant\n", c);
            } else {
                printf("'%c' is other\n", c);
            }
    }
}

// 链表查找示例
typedef struct Node {
    int value;
    struct Node *next;
} Node;

Node* find_node(Node *head, int target) {
    // 搜索节点
    if (Node *current = head; current != NULL) {
        for (; current != NULL; current = current->next) {
            if (current->value == target) {
                return current;
            }
        }
    }
    return NULL;
}

// 资源分配模式
void* allocate_buffer(size_t size) {
    if (void *ptr = malloc(size); ptr != NULL) {
        memset(ptr, 0, size);
        return ptr;
    }
    return NULL;
}

// 嵌套 if 声明
void nested_example(void) {
    int x = 10;
    
    if (int a = x + 5; a > 10) {
        printf("Outer: a = %d\n", a);
        
        if (int b = a * 2; b < 50) {
            printf("Inner: b = %d\n", b);
        }
        // b 在这里不可见
    }
    // a 在这里不可见
}

// 与 defer 结合使用
void process_with_defer(const char *filename) {
    if (FILE *fp = fopen(filename, "r"); fp != NULL) {
        defer fclose(fp);
        
        if (char *buffer = malloc(1024); buffer != NULL) {
            defer free(buffer);
            
            while (fgets(buffer, 1024, fp)) {
                printf("%s", buffer);
            }
        }
    }
}

int main(void) {
    printf("C2y if Declaration Demo\n\n");
    
    // 创建测试文件
    FILE *test = fopen("test_if_decl.txt", "w");
    if (test) {
        fprintf(test, "Line 1\nLine 2\nLine 3\n");
        fclose(test);
    }
    
    printf("--- File Processing ---\n");
    process_file_improved("test_if_decl.txt");
    process_file_improved("nonexistent.txt");
    
    printf("\n--- Config Reading ---\n");
    setenv("MY_APP_PORT", "8080", 1);
    int port = read_config_value("MY_APP_PORT", 3000);
    int timeout = read_config_value("MY_APP_TIMEOUT", 30);
    printf("Port: %d\n", port);
    printf("Timeout: %d\n", timeout);
    
    printf("\n--- Character Classification ---\n");
    classify_character('a');
    classify_character('B');
    classify_character('5');
    classify_character('#');
    
    printf("\n--- Linked List ---\n");
    Node nodes[] = {
        {1, &nodes[1]},
        {2, &nodes[2]},
        {3, NULL}
    };
    
    Node *found = find_node(nodes, 2);
    if (found) {
        printf("Found node with value: %d\n", found->value);
    }
    
    printf("\n--- Nested Example ---\n");
    nested_example();
    
    // 清理
    remove("test_if_decl.txt");
    
    printf("\nif declaration demo complete!\n");
    
    return 0;
}
