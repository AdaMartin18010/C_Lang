/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 460
 * Language: c
 * Block ID: 8fa20a0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线性通道
typedef struct {
    int fd;
    bool closed;
} LinearChannel;

// 发送后通道类型改变（静态检查）
// send : int.Channel<End> ⊸ Channel<End>
void channel_send_int(LinearChannel *ch, int value) {
    write(ch->fd, &value, sizeof(value));
    // ch类型现在变成"已发送int"
}

// 必须关闭
void channel_close(LinearChannel *ch) {
    close(ch->fd);
    ch->closed = true;
}

// 会话类型示例:
// Server: ?int.?int.!int.End
// 接收int，接收int，发送int，结束
void server_session(LinearChannel *ch) {
    int x = channel_recv_int(ch);  // ?int
    int y = channel_recv_int(ch);  // ?int
    channel_send_int(ch, x + y);   // !int
    channel_close(ch);             // End
}
