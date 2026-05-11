# 协议序列化引擎（工业实战项目）

二进制序列化引擎，实现变长整数（VarInt）和 TLV（Tag-Length-Value）编码，零外部依赖。

## 特性

- **VarInt 编码**：7 位一组 + 延续位，类似 Protocol Buffers
- **TLV 格式**：Tag(1B) + Type(1B) + Length(4B) + Value
- **零拷贝解析**：解析器返回指向原始缓冲区的指针
- **大端序网络字节序**：跨平台兼容

## 设计决策

| 决策 | 选择 | 理由 |
|------|------|------|
| 整数编码 | VarInt | 小值节省空间，天然支持大值 |
| 字段头 | 固定 6 字节 | 简单、边界安全、O(1) 跳过 |
| 字节序 | 大端序 | 网络协议标准 |
| 字符串 | 无 NUL 终止符 | 二进制安全，支持任意字节 |

## 编译运行

```bash
make
./test_serializer
```

## 格式示例

```
[Tag:1][Type:INT32][Length:4][Value:4B]    = 1+1+4+4 = 10 bytes
[Tag:2][Type:STRING][Length:5][Value:hello] = 1+1+4+5 = 11 bytes
```

## 扩展方向

1. **Schema 编译器**：从 .proto 风格定义生成 C 结构体 + 序列化代码
2. **CRC 校验**：每个消息尾部追加校验和
3. **压缩**：对字符串/字节字段应用 Snappy/LZ4
4. **流式解析**：支持从网络 socket 增量读取和解析
