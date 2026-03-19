/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 235
 * Language: c
 * Block ID: 86a7860d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 并发程序的博弈语义
 *
 * 并行组合：多个博弈同时进行
 * 同步：博弈之间的握手点
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

// 通道作为博弈之间的连接
typedef struct {
    sem_t ready;    // P 发送信号
    sem_t ack;      // O 确认接收
    int value;      // 传递的数据
} Channel;

// 发送动作 (Player 移动)
void send(Channel *ch, int value)
{
    // P: "我有值要发送"
    ch->value = value;
    sem_post(&ch->ready);   // 通知就绪
    sem_wait(&ch->ack);     // 等待确认
}

// 接收动作 (Opponent 移动)
int receive(Channel *ch)
{
    // O: "等待接收值"
    sem_wait(&ch->ready);   // 等待发送者
    int v = ch->value;
    sem_post(&ch->ack);     // 确认接收
    return v;
}

// 生产者博弈
void *producer_game(void *arg)
{
    Channel *ch = arg;
    for (int i = 0; i < 5; i++) {
        printf("P: Sending %d\n", i);
        send(ch, i);  // Player 移动
    }
    return NULL;
}

// 消费者博弈
void *consumer_game(void *arg)
{
    Channel *ch = arg;
    for (int i = 0; i < 5; i++) {
        int v = receive(ch);  // Opponent 响应
        printf("O: Received %d\n", v);
    }
    return NULL;
}

// 并行组合运行
void parallel_composition(void)
{
    Channel ch;
    sem_init(&ch.ready, 0, 0);
    sem_init(&ch.ack, 0, 0);

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer_game, &ch);
    pthread_create(&cons, NULL, consumer_game, &ch);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&ch.ready);
    sem_destroy(&ch.ack);
}
