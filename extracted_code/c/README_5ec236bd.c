/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\README.md
 * Line: 515
 * Language: c
 * Block ID: 5ec236bd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 处理 HTTP 请求
 *
 * @param req 请求对象，必须由 http_request_init 初始化
 * @param resp 响应对象，将被填充
 * @param timeout_ms 超时时间（毫秒），0 表示无超时
 *
 * @return 0 成功，负值为错误码
 * @retval -EINVAL 无效参数
 * @retval -ETIME  超时
 *
 * @pre req != NULL && resp != NULL
 * @post resp->status 已设置
 *
 * @note 线程安全，可被多个线程并发调用
 * @warning timeout_ms > 10000 可能导致连接池耗尽
 *
 * @code
 *   http_request_t req;
 *   http_response_t resp;
 *   http_request_init(&req, "GET", "/api/data");
 *   int rc = http_handle_request(&req, &resp, 5000);
 *   if (rc == 0) {
 *       process_response(&resp);
 *   }
 *   http_response_cleanup(&resp);
 * @endcode
 */
int http_handle_request(const http_request_t *req,
                        http_response_t *resp,
                        int timeout_ms);
