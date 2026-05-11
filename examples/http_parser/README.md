# HTTP 协议解析器（工业实战项目）

一个零外部依赖的状态机驱动 HTTP/1.1 请求解析器，集成单线程 select 多路复用服务器。

## 特性

- **状态机解析**：METHOD → URI → VERSION → HEADERS → BODY
- **支持方法**：GET / POST / HEAD / PUT / DELETE
- **头部提取**：`Content-Length`、`Connection`、`Host` 等
- **响应构建**：格式化 HTTP/1.1 响应（状态行 + 头部 + 正文）
- **Keep-Alive 检测**：解析 `Connection: close` 头部

## 文件结构

| 文件 | 说明 |
|------|------|
| `http_parser.h` | 解析器 + 响应构建器 API |
| `http_parser.c` | 状态机实现 |
| `http_server.c` | select 多路复用 HTTP 服务器 |
| `test_parser.c` | 单元测试 |

## 快速测试

```bash
make
./test_parser

# 终端1
./http_server 8080

# 终端2
curl http://localhost:8080/
curl http://localhost:8080/api/status
curl -X POST -d "hello world" http://localhost:8080/echo
```

## API 示例

```c
/* 解析 */
HttpRequest req;
http_parse_request(&req, buffer, len);
const char *host = http_get_header(&req, "Host");

/* 响应 */
HttpResponse res;
http_response_init(&res, 200);
http_response_header(&res, "Content-Type", "application/json");
http_response_body(&res, "{\"ok\":true}", 11);

char buf[4096];
http_response_format(&res, buf, sizeof(buf));
send(fd, buf, strlen(buf), 0);
```

## 设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 解析方式 | 状态机 | 清晰、可预测、无回溯 |
| 缓冲区 | 调用方提供 | 零拷贝，嵌入式友好 |
| 头部存储 | 固定数组 | 简单、无堆分配、边界安全 |
| URI 限制 | 2048 字节 | HTTP 标准建议 |
