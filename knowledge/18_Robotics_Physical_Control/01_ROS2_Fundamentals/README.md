# ROS2基础

> **层级定位**: 18_Robotics_Physical_Control > 01_ROS2_Fundamentals
> **难度级别**: L4
> **前置知识**: Linux基础、C++或Python、CMake

---

## 目录

1. [ROS2基础](./01_ROS2_Introduction.md)
2. [节点与话题](./02_Nodes_Topics.md)
3. [服务与动作](./03_Services_Actions.md)
4. [TF2坐标变换](./04_TF2_Transforms.md)
5. [RViz与Gazebo](./05_RViz_Gazebo.md)

---

## ROS2核心概念

### 节点 (Node)

ROS2中的最小计算单元，每个节点负责特定功能。

```cpp
// C++节点示例
#include "rclcpp/rclcpp.hpp"

class MinimalNode : public rclcpp::Node {
public:
    MinimalNode() : Node("minimal_node") {
        RCLCPP_INFO(this->get_logger(), "Node started");
    }
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MinimalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

### 话题 (Topic)

发布/订阅模式的异步通信。

```cpp
// 发布者
auto publisher = node->create_publisher<std_msgs::msg::String>("topic", 10);
auto message = std_msgs::msg::String();
message.data = "Hello, ROS2!";
publisher->publish(message);

// 订阅者
auto subscription = node->create_subscription<std_msgs::msg::String>(
    "topic", 10,
    [](const std_msgs::msg::String::SharedPtr msg) {
        RCLCPP_INFO(node->get_logger(), "Received: %s", msg->data.c_str());
    });
```

---

## C语言在ROS2中

虽然ROS2主要使用C++，但核心库(rcl)提供C接口：

```c
#include <rcl/rcl.h>
#include <rclc/rclc.h>

// C语言节点示例
void my_callback(const void* msgin) {
    const std_msgs__msg__String* msg = msgin;
    printf("Received: %s\n", msg->data.data);
}
```

---

> **最后更新**: 2026-03-21


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
