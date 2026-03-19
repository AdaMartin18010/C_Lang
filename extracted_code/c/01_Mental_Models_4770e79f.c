/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 269
 * Language: c
 * Block ID: 4770e79f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用熟悉的隐喻

// 文件系统作为办公室档案
// - 目录 = 文件柜
// - 文件 = 文档
// - 路径 = 地址

typedef struct FileCabinet Directory;
typedef struct Document File;

Document *file_cabinet_locate(FileCabinet *cabinet,
                               const char *address);
void document_read(Document *doc, void *buffer, size_t len);
void document_close(Document *doc);

// 网络连接作为电话呼叫
// - connect = 拨号
// - accept = 接听
// - send/receive = 通话
// - close = 挂断

Connection *dial(const char *phone_number);
Connection *answer(Phone *phone);
void speak(Connection *conn, const void *message, size_t len);
size_t listen(Connection *conn, void *buffer, size_t max_len);
void hang_up(Connection *conn);
