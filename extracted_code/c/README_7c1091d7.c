/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 888
 * Language: c
 * Block ID: 7c1091d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 网络连接状态机
typedef struct Connection Connection;

// 状态接口
typedef struct {
    void (*open)(Connection* conn);
    void (*close)(Connection* conn);
    void (*send)(Connection* conn, const char* data);
    void (*receive)(Connection* conn);
    const char* name;
} ConnectionState;

struct Connection {
    const ConnectionState* state;
    int socket_fd;
    char buffer[1024];
};

// 状态切换宏
#define CHANGE_STATE(conn, new_state) do { \
    printf("[%s] -> [%s]\n", (conn)->state->name, (new_state).name); \
    (conn)->state = &(new_state); \
} while(0)

// 具体状态: 关闭
static void closed_open(Connection* conn);
static void closed_close(Connection* conn);
static void closed_send(Connection* conn, const char* data);
static void closed_receive(Connection* conn);

static const ConnectionState CLOSED_STATE = {
    .open = closed_open,
    .close = closed_close,
    .send = closed_send,
    .receive = closed_receive,
    .name = "CLOSED"
};

static void closed_open(Connection* conn) {
    // 模拟连接建立
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created: %d\n", conn->socket_fd);
    extern const ConnectionState ESTABLISHED_STATE;
    CHANGE_STATE(conn, ESTABLISHED_STATE);
}

static void closed_close(Connection* conn) {
    printf("Already closed\n");
}

static void closed_send(Connection* conn, const char* data) {
    (void)conn; (void)data;
    printf("ERROR: Cannot send in CLOSED state\n");
}

static void closed_receive(Connection* conn) {
    (void)conn;
    printf("ERROR: Cannot receive in CLOSED state\n");
}

// 具体状态: 已建立
static void established_open(Connection* conn);
static void established_close(Connection* conn);
static void established_send(Connection* conn, const char* data);
static void established_receive(Connection* conn);

const ConnectionState ESTABLISHED_STATE = {
    .open = established_open,
    .close = established_close,
    .send = established_send,
    .receive = established_receive,
    .name = "ESTABLISHED"
};

static void established_open(Connection* conn) {
    (void)conn;
    printf("Already connected\n");
}

static void established_close(Connection* conn) {
    close(conn->socket_fd);
    conn->socket_fd = -1;
    CHANGE_STATE(conn, CLOSED_STATE);
}

static void established_send(Connection* conn, const char* data) {
    send(conn->socket_fd, data, strlen(data), 0);
    printf("Sent: %s\n", data);
}

static void established_receive(Connection* conn) {
    int n = recv(conn->socket_fd, conn->buffer, sizeof(conn->buffer) - 1, 0);
    if (n > 0) {
        conn->buffer[n] = '\0';
        printf("Received: %s\n", conn->buffer);
    }
}

// 使用
void state_machine_example(void) {
    Connection conn = { &CLOSED_STATE, -1, {0} };

    conn.state->send(&conn, "Hello");  // ERROR
    conn.state->open(&conn);            // CLOSED -> ESTABLISHED
    conn.state->send(&conn, "Hello");  // 发送成功
    conn.state->close(&conn);           // ESTABLISHED -> CLOSED
}
