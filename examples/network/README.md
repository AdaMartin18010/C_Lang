# 网络编程示例

POSIX Socket 编程可编译示例。

## 文件说明

| 文件 | 说明 |
|------|------|
| `tcp_server.c` | 单线程 `select` 多路复用 Echo 服务器 |
| `tcp_client.c` | TCP 客户端，`gethostbyname` + `connect` |

## 编译运行

```bash
make
./tcp_server        # 终端1
./tcp_client localhost 8080   # 终端2
```

> Windows 用户请使用 WSL 或 MSYS2 编译运行。
