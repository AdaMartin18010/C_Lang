/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 364
 * Language: c
 * Block ID: 02974305
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 抽象泄漏：暴露了不应暴露的实现细节

// 网络库暴露套接字细节
int network_send(SocketHandle sock, void *data, size_t len);
// 问题：调用者需要知道SocketHandle是什么

// 修复：隐藏实现细节
Result network_send(Connection *conn, Message *msg);
// Connection和Message是抽象概念
