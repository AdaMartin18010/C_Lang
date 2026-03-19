/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 390
 * Language: c
 * Block ID: 2aa61c22
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 明文传输 */
int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
send(sock, password, strlen(password), 0);  /* 明文发送密码 */

/* 符合规范 - 使用TLS */
SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
SSL* ssl = SSL_new(ctx);
SSL_set_fd(ssl, sock);
SSL_connect(ssl);
SSL_write(ssl, password, strlen(password));  /* 加密传输 */
