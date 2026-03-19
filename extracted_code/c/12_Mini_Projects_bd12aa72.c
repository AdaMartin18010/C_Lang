/*
 * Auto-generated from: 12_Practice_Exercises\12_Mini_Projects.md
 * Line: 216
 * Language: c
 * Block ID: bd12aa72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 发送邮件
SMTPSession* smtp_connect(const char *server, int port);
void smtp_auth(SMTPSession *session, const char *user, const char *pass);
void smtp_send(SMTPSession *session, const Email *email);
void smtp_quit(SMTPSession *session);
