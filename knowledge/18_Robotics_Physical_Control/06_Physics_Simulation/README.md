
---

## 🔗 全面知识关联体系

### 【全局层】知识库导航

| 维度 | 目标文档 | 导航作用 |
|:-----|:---------|:---------|
| **总索引** | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口，全局视角 |
| **本模块** | [../../readme.md](../../readme.md) | 模块总览与目录导航 |
| **学习路径** | [../../06_Thinking_Representation/06_Learning_Paths/readme.md](../../06_Thinking_Representation/06_Learning_Paths/readme.md) | 阶段化学习路线规划 |
| **概念映射** | [../../06_Thinking_Representation/05_Concept_Mappings/readme.md](../../06_Thinking_Representation/05_Concept_Mappings/readme.md) | 核心概念等价关系图 |

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
| **理论基础** | [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/readme.md) | 语义学理论基础 |
| **核心机制** | [../../01_Core_Knowledge_System/02_Core_Layer/readme.md](../../01_Core_Knowledge_System/02_Core_Layer/readme.md) | C语言核心机制 |
| **标准接口** | [../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md](../../01_Core_Knowledge_System/04_Standard_Library_Layer/readme.md) | 标准库API |
| **系统实现** | [../../03_System_Technology_Domains/readme.md](../../03_System_Technology_Domains/readme.md) | 系统级实现 |

### 【局部层】横向关联网

| 关联类型 | 目标文档 | 关联说明 |
|:---------|:---------|:---------|
| **技术扩展** | [../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 并发编程技术 |
| **安全规范** | [../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md](../../01_Core_Knowledge_System/09_Safety_Standards/MISRA_C_2023/readme.md) | 安全编码标准 |
| **工具支持** | [../../07_Modern_Toolchain/readme.md](../../07_Modern_Toolchain/readme.md) | 现代开发工具链 |
| **实践案例** | [../../04_Industrial_Scenarios/readme.md](../../04_Industrial_Scenarios/readme.md) | 工业实践场景 |

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
| **问题导向** | 06决策树 → 相关模块 | [决策树目录](../../06_Thinking_Representation/01_Decision_Trees/readme.md) |
| **项目驱动** | 04案例 → 所需知识 | [工业场景](../../04_Industrial_Scenarios/readme.md) |
| **深入研究** | 02形式语义 → 11CompCert | [形式语义](../../02_Formal_Semantics_and_Physics/readme.md) |

---

# 物理仿真 (Physics Simulation)

> **层级定位**: 18_Robotics_Physical_Control > 06_Physics_Simulation
> **难度级别**: ⭐⭐⭐⭐ L4
> **前置知识**: 刚体动力学、碰撞检测、数值积分、C++
> **参考标准**: MuJoCo, Bullet Physics, Gazebo

---

## 内容

- [MuJoCo集成](./01_MuJoCo_Integration.md)
- [Bullet Physics](./02_Bullet_Physics.md)
- [仿真到现实](./03_Simulation_to_Real.md)

---

## 物理引擎对比

| 引擎 | 特点 | 精度 | 速度 | 许可 |
|:-----|:-----|:-----|:-----|:-----|
| **MuJoCo** | 接触动力学优化 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Apache 2.0 |
| **Bullet** | 广泛使用, 游戏友好 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | zlib |
| **ODE** | 简单稳定, 开源 | ⭐⭐⭐ | ⭐⭐⭐ | BSD |
| **PhysX** | NVIDIA GPU加速 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | BSD |

---

## 深入理解

### 1. 物理仿真基础

#### 刚体动力学方程

```
M(q)q̈ + C(q, q̇) = τ + J^T F_contact

其中:
- M(q): 质量矩阵
- C(q, q̇): 科氏力和离心力
- τ: 广义力
- F_contact: 接触力
- J: 接触雅可比
```

#### 时间积分方法

```cpp
// 显式欧拉 (不稳定, 仅用于教学)
void explicit_euler(State& state, double dt) {
    state.position += state.velocity * dt;
    state.velocity += acceleration(state) * dt;
}

// 隐式欧拉 (稳定但耗散能量)
void implicit_euler(State& state, double dt) {
    // 需要求解非线性方程
    // v_new = v_old + a(q_new, v_new) * dt
}

// 半隐式欧拉 (Symplectic Euler) - 常用
void semi_implicit_euler(State& state, double dt) {
    state.velocity += acceleration(state) * dt;
    state.position += state.velocity * dt;
}

// 4阶龙格-库塔 (高精度)
void runge_kutta4(State& state, double dt) {
    auto k1 = derivative(state);
    auto k2 = derivative(state + k1 * (dt/2));
    auto k3 = derivative(state + k2 * (dt/2));
    auto k4 = derivative(state + k3 * dt);

    state += (k1 + k2*2 + k3*2 + k4) * (dt/6);
}
```

### 2. MuJoCo接触动力学

MuJoCo使用凸优化求解接触力，比惩罚方法更稳定。

```cpp
#include <mujoco/mujoco.h>

// MuJoCo仿真控制器
class MuJoCoSimulator {
public:
    MuJoCoSimulator(const char* xml_path) {
        // 加载模型
        char error[1000];
        model_ = mj_loadXML(xml_path, nullptr, error, 1000);
        if (!model_) {
            throw std::runtime_error(error);
        }

        // 创建数据实例
        data_ = mj_makeData(model_);

        // 初始化状态
        mj_resetData(model_, data_);
    }

    ~MuJoCoSimulator() {
        mj_deleteData(data_);
        mj_deleteModel(model_);
    }

    // 单步仿真
    void step(const std::vector<double>& ctrl) {
        // 设置控制输入
        for (int i = 0; i < model_->nu && i < ctrl.size(); ++i) {
            data_->ctrl[i] = ctrl[i];
        }

        // 前向动力学
        mj_step(model_, data_);
    }

    // 获取关节状态
    void get_joint_states(std::vector<double>& qpos,
                          std::vector<double>& qvel) const {
        qpos.assign(data_->qpos, data_->qpos + model_->nq);
        qvel.assign(data_->qvel, data_->qvel + model_->nv);
    }

    // 获取接触力
    void get_contact_forces(std::vector<double>& forces) const {
        forces.clear();
        for (int i = 0; i < data_->ncon; ++i) {
            // contact[i].frame 包含接触坐标系
            // contact[i].efc_address 包含力索引
            double force[6];
            mj_contactForce(model_, data_, i, force);
            forces.insert(forces.end(), force, force+6);
        }
    }

    // 设置初始状态
    void set_state(const std::vector<double>& qpos,
                   const std::vector<double>& qvel) {
        std::copy(qpos.begin(), qpos.end(), data_->qpos);
        std::copy(qvel.begin(), qvel.end(), data_->qvel);
        mj_forward(model_, data_);  // 更新运动学
    }

private:
    mjModel* model_;
    mjData* data_;
};

// XML模型示例 (robot.xml)
/*
<mujoco model="2dof_robot">
  <compiler angle="radian"/>

  <option timestep="0.002" iterations="50" solver="Newton">
    <flag warmstart="enable"/>
  </option>

  <worldbody>
    <light diffuse=".5 .5 .5" pos="0 0 3" dir="0 0 -1"/>
    <geom type="plane" size="1 1 0.1" rgba=".9 .9 .9 1"/>

    <body name="base" pos="0 0 0.5">
      <joint name="joint1" type="hinge" axis="0 0 1"
             damping="0.1" armature="0.01"/>
      <geom type="cylinder" size="0.05 0.1" rgba=".9 .1 .1 1"/>

      <body name="link1" pos="0.2 0 0">
        <joint name="joint2" type="hinge" axis="0 1 0"
               damping="0.1" armature="0.01"/>
        <geom type="capsule" size="0.03" fromto="0 0 0 0.2 0 0"
              rgba=".1 .9 .1 1"/>

        <site name="end_effector" pos="0.2 0 0" size="0.02"/>
      </body>
    </body>
  </worldbody>

  <actuator>
    <motor joint="joint1" gear="1" ctrlrange="-100 100"/>
    <motor joint="joint2" gear="1" ctrlrange="-100 100"/>
  </actuator>
</mujoco>
*/
```

### 3. Bullet Physics集成

```cpp
#include <btBulletDynamicsCommon.h>

class BulletSimulator {
public:
    BulletSimulator() {
        // 创建碰撞配置
        collision_config_ = new btDefaultCollisionConfiguration();

        // 创建碰撞检测器
        dispatcher_ = new btCollisionDispatcher(collision_config_);

        // 创建宽相检测
        broadphase_ = new btDbvtBroadphase();

        // 创建约束求解器
        solver_ = new btSequentialImpulseConstraintSolver();

        // 创建物理世界
        world_ = new btDiscreteDynamicsWorld(
            dispatcher_, broadphase_, solver_, collision_config_);

        // 设置重力
        world_->setGravity(btVector3(0, 0, -9.81));
    }

    ~BulletSimulator() {
        // 清理刚体
        for (int i = world_->getNumCollisionObjects() - 1; i >= 0; i--) {
            btCollisionObject* obj = world_->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState()) {
                delete body->getMotionState();
            }
            world_->removeCollisionObject(obj);
            delete obj;
        }

        delete world_;
        delete solver_;
        delete broadphase_;
        delete dispatcher_;
        delete collision_config_;
    }

    // 添加盒子
    void add_box(const btVector3& size, const btVector3& pos,
                 double mass, const btQuaternion& rot = btQuaternion::getIdentity()) {

        btCollisionShape* shape = new btBoxShape(size);
        collision_shapes_.push_back(shape);

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(pos);
        transform.setRotation(rot);

        btScalar mass_val(mass);
        btVector3 local_inertia(0, 0, 0);
        if (mass != 0.0f) {
            shape->calculateLocalInertia(mass_val, local_inertia);
        }

        btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(
            mass_val, motion_state, shape, local_inertia);
        btRigidBody* body = new btRigidBody(rbInfo);

        world_->addRigidBody(body);
    }

    // 仿真步进
    void step(double dt, int substeps = 10) {
        world_->stepSimulation(dt, substeps);
    }

    // 获取刚体状态
    void get_body_state(int index, btVector3& pos, btQuaternion& rot,
                        btVector3& vel, btVector3& ang_vel) {
        btCollisionObject* obj = world_->getCollisionObjectArray()[index];
        btRigidBody* body = btRigidBody::upcast(obj);

        if (body) {
            pos = body->getCenterOfMassPosition();
            rot = body->getOrientation();
            vel = body->getLinearVelocity();
            ang_vel = body->getAngularVelocity();
        }
    }

    // 设置刚体力/力矩
    void apply_force(int index, const btVector3& force,
                     const btVector3& rel_pos = btVector3(0, 0, 0)) {
        btCollisionObject* obj = world_->getCollisionObjectArray()[index];
        btRigidBody* body = btRigidBody::upcast(obj);

        if (body) {
            body->applyForce(force, rel_pos);
        }
    }

private:
    btDefaultCollisionConfiguration* collision_config_;
    btCollisionDispatcher* dispatcher_;
    btDbvtBroadphase* broadphase_;
    btSequentialImpulseConstraintSolver* solver_;
    btDiscreteDynamicsWorld* world_;
    std::vector<btCollisionShape*> collision_shapes_;
};
```

### 4. 仿真到现实 (Sim-to-Real)

```cpp
// 系统辨识: 从仿真参数估计真实系统参数
class SystemIdentification {
public:
    // 最小二乘参数估计
    static void least_squares_identification(
        const std::vector<std::vector<double>>& phi,  // 回归向量
        const std::vector<double>& y,                  // 输出
        std::vector<double>& theta) {                  // 参数估计

        int n = phi[0].size();
        int m = phi.size();

        // 构建Phi矩阵和Y向量
        Eigen::MatrixXd Phi(m, n);
        Eigen::VectorXd Y(m);

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                Phi(i, j) = phi[i][j];
            }
            Y(i) = y[i];
        }

        // 最小二乘解: theta = (Phi^T * Phi)^-1 * Phi^T * Y
        Eigen::VectorXd Theta = (Phi.transpose() * Phi).inverse() * Phi.transpose() * Y;

        theta.resize(n);
        for (int i = 0; i < n; ++i) {
            theta[i] = Theta(i);
        }
    }

    // 辨识机器人惯性参数
    static void identify_inertial_parameters(
        MuJoCoSimulator& sim,
        const std::vector<std::vector<double>>& trajectories,
        std::vector<double>& inertial_params) {

        std::vector<std::vector<double>> phi_data;
        std::vector<double> torque_data;

        for (const auto& traj : trajectories) {
            // 执行轨迹并记录数据
            for (size_t t = 0; t < traj.size() - 2; ++t) {
                std::vector<double> q(traj.begin() + t, traj.begin() + t + 2);
                std::vector<double> qd(2), qdd(2);

                // 数值微分
                qd[0] = (traj[t+1] - traj[t]) / 0.002;
                qd[1] = (traj[t+2] - traj[t+1]) / 0.002;
                qdd[0] = (qd[1] - qd[0]) / 0.002;

                // 构建回归向量 (基于动力学方程)
                std::vector<double> phi = {
                    qdd[0],                                    // 加速度相关
                    qdd[1],
                    sin(q[0]), cos(q[0]),                      // 重力相关
                    qd[0] * qd[1],                             // 科氏力
                    qd[0] * qd[0]                              // 离心力
                };

                phi_data.push_back(phi);

                // 获取实际力矩 (从真实机器人)
                double tau_actual = get_actual_torque(q, qd);
                torque_data.push_back(tau_actual);
            }
        }

        least_squares_identification(phi_data, torque_data, inertial_params);
    }

private:
    static double get_actual_torque(const std::vector<double>& q,
                                    const std::vector<double>& qd) {
        // 从真实机器人读取力矩
        return 0.0;  // 占位
    }
};
```

---

## 最佳实践

1. **时间步长**: MuJoCo推荐2ms，Bullet推荐1-2ms
2. **碰撞检测**: 使用凸包近似复杂几何体提高效率
3. **接触参数**: 调整摩擦系数和恢复系数匹配实际
4. **随机化**: Domain Randomization提高Sim-to-Real迁移
5. **可视化**: 使用rviz或自定义OpenGL渲染

---

## 权威参考

- [1] MuJoCo Documentation: <https://mujoco.readthedocs.io/>
- [2] Bullet Physics Manual: <https://pybullet.org/>
- [3] "Rigid Body Dynamics Algorithms" by Roy Featherstone
- [4] "Contact and Friction Simulation" by Coumans & Bai

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L4 (专业级)
