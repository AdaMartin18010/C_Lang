
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../README.md](../../README.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 核心概念等价关系图 |

### 【阶段层】学习定位

**当前模块**: 知识库
**难度等级**: L1-L6
**前置依赖**: 核心知识体系
**后续延伸**: 持续学习

```
学习阶段金字塔:
    L6 专家层 [形式验证、编译器]
    L5 高级层 [并发、系统编程] ⬅️ 可能在此
    L4 进阶层 [指针、内存管理]
    L3 基础层 [函数、结构体]
    L2 入门层 [语法、数据类型]
    L1 零基础 [环境搭建]
```

### 【层次层】纵向知识链

| 层级 | 关联文档 | 层次关系 |
|:-----|:---------|:---------|
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/README.md](../../01_Core_Knowledge_System/02_Core_Layer/README.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/README.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/README.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 工业实践场景 |

### 【总体层】知识体系架构

```
┌─────────────────────────────────────────────────────────────┐
│                     总体知识体系架构                          │
├─────────────────────────────────────────────────────────────┤
│  01 Core Knowledge          → 核心概念与机制                  │
│  02 Formal Semantics        → 理论与物理基础                  │
│  03 System Technology       → 系统级技术领域                  │
│  04 Industrial Scenarios    → 工业应用场景                    │
│  05 Deep Structure          → 深层结构与元物理                │
│  06 Thinking Representation → 思维表征与学习                  │
│  07 Modern Toolchain        → 现代工具链                      │
└─────────────────────────────────────────────────────────────┘
```

### 【决策层】学习路径选择

| 目标 | 推荐路径 | 关键文档 |
|:-----|:---------|:---------|
| **系统学习** | 01 → 02 → 03 → 04 | 按顺序阅读各模块 |
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/README.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/README.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/README.md) |

---

# ROS2基础 (ROS2 Fundamentals)

> **层级定位**: 18_Robotics_Physical_Control > 01_ROS2_Fundamentals
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: Linux基础、C++11/14、CMake、发布/订阅模式
> **参考标准**: ROS2 Humble/Iron, DDS Specification

---

## 目录

1. [ROS2架构](./01_ROS2_Introduction.md)
2. [节点与话题](./02_Nodes_Topics.md)
3. [服务与动作](./03_Services_Actions.md)
4. [TF2坐标变换](./04_TF2_Transforms.md)
5. [RViz与Gazebo](./05_RViz_Gazebo.md)

---

## ROS2核心概念

### 节点 (Node)

ROS2中的最小计算单元，每个节点负责特定功能。与ROS1不同，ROS2节点可以跨进程通信，无需ROS Master。

```cpp
// C++节点完整示例
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class MinimalNode : public rclcpp::Node {
public:
    MinimalNode() : Node("minimal_node"), count_(0) {
        RCLCPP_INFO(this->get_logger(), "Node started");

        // 创建定时器
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&MinimalNode::timer_callback, this));
    }

private:
    void timer_callback() {
        RCLCPP_INFO(this->get_logger(), "Hello %d", count_++);
    }

    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MinimalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

### 话题 (Topic) - 发布/订阅模式

```cpp
// 发布者节点
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

class JointStatePublisher : public rclcpp::Node {
public:
    JointStatePublisher() : Node("joint_state_publisher") {
        // QoS配置: 历史策略为KeepLast(10)，可靠性为Reliable
        auto qos = rclcpp::QoS(10)
            .reliable()
            .durability_volatile();

        publisher_ = this->create_publisher<sensor_msgs::msg::JointState>(
            "joint_states", qos);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),  // 100Hz
            std::bind(&JointStatePublisher::publish_callback, this));
    }

private:
    void publish_callback() {
        auto message = sensor_msgs::msg::JointState();
        message.header.stamp = this->now();
        message.name = {"joint1", "joint2", "joint3"};
        message.position = {joint1_pos_, joint2_pos_, joint3_pos_};
        message.velocity = {joint1_vel_, joint2_vel_, joint3_vel_};
        message.effort = {joint1_eff_, joint2_eff_, joint3_eff_};

        publisher_->publish(message);
    }

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    double joint1_pos_ = 0.0, joint2_pos_ = 0.0, joint3_pos_ = 0.0;
    double joint1_vel_ = 0.0, joint2_vel_ = 0.0, joint3_vel_ = 0.0;
    double joint1_eff_ = 0.0, joint2_eff_ = 0.0, joint3_eff_ = 0.0;
};

// 订阅者节点
class JointStateSubscriber : public rclcpp::Node {
public:
    JointStateSubscriber() : Node("joint_state_subscriber") {
        auto qos = rclcpp::QoS(10).best_effort();

        subscription_ = this->create_subscription<sensor_msgs::msg::JointState>(
            "joint_states", qos,
            std::bind(&JointStateSubscriber::topic_callback, this, std::placeholders::_1));
    }

private:
    void topic_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(),
            "Received joint states at %f",
            rclcpp::Time(msg->header.stamp).seconds());

        for (size_t i = 0; i < msg->name.size(); ++i) {
            RCLCPP_INFO(this->get_logger(),
                "%s: pos=%.3f, vel=%.3f",
                msg->name[i].c_str(),
                msg->position[i],
                msg->velocity[i]);
        }
    }

    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscription_;
};
```

### 服务 (Service) - 请求/响应模式

```cpp
// 自定义服务接口 (srv/AddTwoInts.srv)
// int64 a
// int64 b
// ---
// int64 sum

// 服务端
#include "example_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsServer : public rclcpp::Node {
public:
    AddTwoIntsServer() : Node("add_two_ints_server") {
        service_ = this->create_service<example_interfaces::srv::AddTwoInts>(
            "add_two_ints",
            std::bind(&AddTwoIntsServer::handle_service, this,
                      std::placeholders::_1, std::placeholders::_2));
    }

private:
    void handle_service(
        const std::shared_ptr<example_interfaces::srv::AddTwoInts::Request> request,
        std::shared_ptr<example_interfaces::srv::AddTwoInts::Response> response) {

        response->sum = request->a + request->b;
        RCLCPP_INFO(this->get_logger(),
            "Incoming request: %ld + %ld = %ld",
            request->a, request->b, response->sum);
    }

    rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr service_;
};

// 客户端
class AddTwoIntsClient : public rclcpp::Node {
public:
    AddTwoIntsClient() : Node("add_two_ints_client") {
        client_ = this->create_client<example_interfaces::srv::AddTwoInts>("add_two_ints");
    }

    bool call_add(int64_t a, int64_t b) {
        auto request = std::make_shared<example_interfaces::srv::AddTwoInts::Request>();
        request->a = a;
        request->b = b;

        // 等待服务可用
        while (!client_->wait_for_service(std::chrono::seconds(1))) {
            if (!rclcpp::ok()) return false;
            RCLCPP_WARN(this->get_logger(), "Service not available, waiting...");
        }

        // 异步调用
        auto future = client_->async_send_request(request);

        // 等待响应
        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future)
            == rclcpp::FutureReturnCode::SUCCESS) {
            auto response = future.get();
            RCLCPP_INFO(this->get_logger(), "Result: %ld", response->sum);
            return true;
        }
        return false;
    }

private:
    rclcpp::Client<example_interfaces::srv::AddTwoInts>::SharedPtr client_;
};
```

---

## 深入理解

### 1. ROS2 DDS架构

ROS2使用DDS (Data Distribution Service) 作为中间件，替代了ROS1的自定义TCPROS/UDPROS。

#### DDS核心概念

```
┌─────────────────────────────────────────────────────────────┐
│                      ROS2 Application                        │
├─────────────────────────────────────────────────────────────┤
│  rclcpp (C++ Client Library)                                │
├─────────────────────────────────────────────────────────────┤
│  rcl (C Client Library)                                     │
├─────────────────────────────────────────────────────────────┤
│  rmw (ROS Middleware Interface)                             │
├─────────────────────────────────────────────────────────────┤
│  DDS Implementation (FastDDS/CycloneDDS/Connext)           │
├─────────────────────────────────────────────────────────────┤
│  UDP/TCP Transport Layer                                    │
└─────────────────────────────────────────────────────────────┘
```

#### QoS (Quality of Service) 策略

```cpp
// 可靠性策略对比
namespace qos_profiles {
    // 传感器数据: 尽力而为，允许丢包
    auto sensor_data = rclcpp::QoS(5)
        .best_effort()
        .durability_volatile();

    // 参数配置: 可靠传输，持久化
    auto parameters = rclcpp::QoS(10)
        .reliable()
        .transient_local();

    // 默认配置
    auto default_qos = rclcpp::QoS(10)
        .reliable()
        .durability_volatile();
}

// 发布者和订阅者QoS必须兼容
// Reliable + Volatile 可以匹配 Reliable/Any + Volatile/Any
// BestEffort 只能匹配 BestEffort
```

### 2. TF2坐标变换系统

TF2管理机器人各部件之间的坐标变换关系。

```cpp
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

class TFBroadcaster : public rclcpp::Node {
public:
    TFBroadcaster() : Node("tf_broadcaster") {
        broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&TFBroadcaster::broadcast_transform, this));
    }

private:
    void broadcast_transform() {
        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = this->now();
        transform.header.frame_id = "base_link";
        transform.child_frame_id = "end_effector";

        // 设置平移
        transform.transform.translation.x = 0.5;
        transform.transform.translation.y = 0.2;
        transform.transform.translation.z = 0.1;

        // 设置旋转 (四元数)
        tf2::Quaternion q;
        q.setRPY(0, 0, M_PI/4);  // Roll, Pitch, Yaw
        transform.transform.rotation.x = q.x();
        transform.transform.rotation.y = q.y();
        transform.transform.rotation.z = q.z();
        transform.transform.rotation.w = q.w();

        broadcaster_->sendTransform(transform);
    }

    std::shared_ptr<tf2_ros::TransformBroadcaster> broadcaster_;
    rclcpp::TimerBase::SharedPtr timer_;
};

// TF监听器
class TFListener : public rclcpp::Node {
public:
    TFListener() : Node("tf_listener"), tf_buffer_(this->get_clock()),
                   tf_listener_(tf_buffer_) {
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(1000),
            std::bind(&TFListener::lookup_transform, this));
    }

private:
    void lookup_transform() {
        try {
            auto transform = tf_buffer_.lookupTransform(
                "base_link", "end_effector", tf2::TimePointZero);

            RCLCPP_INFO(this->get_logger(),
                "Translation: [%.3f, %.3f, %.3f]",
                transform.transform.translation.x,
                transform.transform.translation.y,
                transform.transform.translation.z);
        } catch (const tf2::TransformException& ex) {
            RCLCPP_WARN(this->get_logger(), "Transform error: %s", ex.what());
        }
    }

    tf2_ros::Buffer tf_buffer_;
    tf2_ros::TransformListener tf_listener_;
    rclcpp::TimerBase::SharedPtr timer_;
};
```

### 3. 机器人控制案例

```cpp
// 机械臂关节位置控制器
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "control_msgs/msg/joint_trajectory_controller_state.hpp"

class ArmController : public rclcpp::Node {
public:
    ArmController() : Node("arm_controller") {
        // 发布关节轨迹
        traj_pub_ = this->create_publisher<trajectory_msgs::msg::JointTrajectory>(
            "arm_controller/joint_trajectory", 10);

        // 订阅当前状态
        state_sub_ = this->create_subscription<control_msgs::msg::JointTrajectoryControllerState>(
            "arm_controller/state", 10,
            std::bind(&ArmController::state_callback, this, std::placeholders::_1));

        // 每5秒发送一个新的轨迹
        timer_ = this->create_wall_timer(
            std::chrono::seconds(5),
            std::bind(&ArmController::send_trajectory, this));
    }

private:
    void send_trajectory() {
        trajectory_msgs::msg::JointTrajectory traj;
        traj.header.stamp = this->now();
        traj.joint_names = {"joint1", "joint2", "joint3", "joint4", "joint5", "joint6"};

        // 设置路点
        trajectory_msgs::msg::JointTrajectoryPoint point;
        point.positions = {0.5, -0.5, 0.0, 0.0, 0.5, 0.0};
        point.velocities = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        point.time_from_start = rclcpp::Duration::from_seconds(3.0);

        traj.points.push_back(point);
        traj_pub_->publish(traj);

        RCLCPP_INFO(this->get_logger(), "Sent trajectory");
    }

    void state_callback(const control_msgs::msg::JointTrajectoryControllerState::SharedPtr msg) {
        RCLCPP_INFO(this->get_logger(), "Current positions: [%f, %f, %f, ...]",
            msg->actual.positions[0], msg->actual.positions[1], msg->actual.positions[2]);
    }

    rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>::SharedPtr traj_pub_;
    rclcpp::Subscription<control_msgs::msg::JointTrajectoryControllerState>::SharedPtr state_sub_;
    rclcpp::TimerBase::SharedPtr timer_;
};
```

### 4. C语言ROS2客户端 (rclc)

对于资源受限的嵌入式系统，使用rclc库:

```c
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>

// 订阅回调
void subscription_callback(const void* msgin) {
    const std_msgs__msg__Int32* msg = (const std_msgs__msg__Int32*)msgin;
    printf("Received: %d\n", msg->data);
}

int main(int argc, const char* const* argv) {
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;

    // 初始化
    rclc_support_init(&support, argc, argv, &allocator);

    // 创建节点
    rcl_node_t node;
    rclc_node_init_default(&node, "c_node", "", &support);

    // 创建订阅者
    rcl_subscription_t subscriber;
    std_msgs__msg__Int32 msg;
    rclc_subscription_init_default(
        &subscriber, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        "topic");

    // 创建执行器
    rclc_executor_t executor;
    rclc_executor_init(&executor, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor, &subscriber, &msg,
                                   &subscription_callback, ON_NEW_DATA);

    // 运行
    rclc_executor_spin(&executor);

    // 清理
    rclc_executor_fini(&executor);
    rcl_subscription_fini(&subscriber, &node);
    rcl_node_fini(&node);
    rclc_support_fini(&support);

    return 0;
}
```

---

## 最佳实践

1. **节点设计**: 一个节点负责一个功能模块，保持单一职责
2. **QoS选择**: 传感器数据用BestEffort，控制指令用Reliable
3. **命名空间**: 使用`<robot_name>/<component>/<topic>`格式
4. **异常处理**: DDS可能抛出异常，需适当处理连接断开
5. **实时性**: 使用rclcpp::WallRate控制循环频率

---

## 权威参考

- [1] ROS2 Documentation: <https://docs.ros.org/en/humble/>
- [2] DDS Specification: <https://www.omg.org/spec/DDS/>
- [3] "ROS2 Design": <https://design.ros2.org/>
- [4] "Programming Robots with ROS" by Quigley et al., O'Reilly

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
