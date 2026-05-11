# 库设计示例

C 库接口设计与 ABI 兼容技术。

## 文件说明

| 文件 | 说明 |
|------|------|
| `opaque_pointer.c` | 不透明指针封装内部结构，隐藏实现细节 |
| `api_versioning.c` | API/ABI 版本控制、特性检测、向后兼容 |
| `plugin_system.c` | 跨平台动态加载（`dlopen`/`LoadLibrary`）插件框架 |

## 编译运行

```bash
make
./opaque_pointer
./api_versioning
./plugin_system
```
