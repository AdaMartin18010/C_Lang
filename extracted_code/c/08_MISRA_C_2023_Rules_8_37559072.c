/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\08_MISRA_C_2023_Rules_8.md
 * Line: 451
 * Language: c
 * Block ID: 37559072
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 启动时分配 */
#define MAX_CONNECTIONS 16

static connection_t *connections[MAX_CONNECTIONS];
static bool initialized = false;

int system_init(void)
{
    /* 初始化阶段分配 */
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i] = malloc(sizeof(connection_t));
        if (connections[i] == NULL) {
            /* 清理并返回错误 */
            return -1;
        }
    }
    initialized = true;
    return 0;
}

/* 运行时不分配 */
int connection_get(void)
{
    if (!initialized) {
        return -1;
    }
    /* 从预分配池中获取 */
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i]->state == STATE_FREE) {
            connections[i]->state = STATE_USED;
            return i;
        }
    }
    return -1;  /* 资源耗尽 */
}

void connection_release(int id)
{
    if (id >= 0 && id < MAX_CONNECTIONS) {
        connections[id]->state = STATE_FREE;
    }
}
