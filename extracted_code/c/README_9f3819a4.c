/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 217
 * Language: c
 * Block ID: 9f3819a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* microkernel.h - 微内核架构定义 */
#ifndef MICROKERNEL_H
#define MICROKERNEL_H

#include <stdint.h>
#include <stddef.h>

/* 系统调用号 */
#define SYSCALL_SEND        1
#define SYSCALL_RECEIVE     2
#define SYSCALL_REGISTER    3
#define SYSCALL_UNREGISTER  4

/* 服务ID */
typedef uint32_t service_id_t;
typedef uint32_t message_type_t;

/* 消息结构 */
typedef struct {
    service_id_t sender;
    service_id_t receiver;
    message_type_t type;
    uint32_t payload_size;
    uint8_t payload[];      /* 柔性数组 */
} message_t;

/* 服务描述符 */
typedef struct service {
    service_id_t id;
    const char *name;
    int (*handler)(message_t *msg);
    uint32_t priority;
    struct service *next;
} service_t;

/* 微内核API */
int mk_init(void);
service_id_t mk_register_service(const char *name, int (*handler)(message_t *));
int mk_unregister_service(service_id_t id);
int mk_send_message(service_id_t to, message_t *msg);
int mk_receive_message(message_t *msg, size_t max_size, uint32_t timeout);

/* 消息队列管理 */
typedef struct message_queue {
    message_t *head;
    message_t *tail;
    uint32_t count;
    uint32_t max_count;
} message_queue_t;

int mq_init(message_queue_t *mq, uint32_t max_count);
int mq_send(message_queue_t *mq, message_t *msg);
message_t *mq_receive(message_queue_t *mq);

#endif /* MICROKERNEL_H */
