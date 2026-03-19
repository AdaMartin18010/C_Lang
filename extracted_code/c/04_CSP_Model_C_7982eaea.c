/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 57
 * Language: c
 * Block ID: 7982eaea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* CSP的核心操作 */

/* Channel创建 */
chan_t* ch = make_chan(sizeof(int), buffer_size);

/* 发送 (可能阻塞) */
chan_send(ch, &value);

/* 接收 (可能阻塞) */
chan_recv(ch, &value);

/* 关闭 */
chan_close(ch);

/* Select (多路复用) */
select_t cases[] = {
    {ch1, &v1, SELECT_RECV},
    {ch2, &v2, SELECT_SEND},
    {NULL, NULL, SELECT_DEFAULT}  /* 非阻塞 */
};
int chosen = chan_select(cases, 3);
