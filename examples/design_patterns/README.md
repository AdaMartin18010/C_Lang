# 设计模式示例

C 语言模拟面向对象设计模式。

## 文件说明

| 文件 | 说明 |
|------|------|
| `vtable_polymorphism.c` | 虚表实现运行时多态（Shape/Circle/Rectangle） |
| `state_machine.c` | 表驱动有限状态机，函数指针数组实现状态转换 |
| `observer.c` | 观察者模式，多订阅者回调通知 |

## 编译运行

```bash
make
./vtable_polymorphism
./state_machine
./observer
```
