# 机器人动力学 (Robot Dynamics)

> **层级定位**: 18_Robotics_Physical_Control > 03_Dynamics
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 经典力学、拉格朗日力学、矩阵运算、C语言高级
> **参考标准**: IEEE Robotics and Automation Society

---

## 内容

- [拉格朗日力学](./01_Lagrangian_Mechanics.md)
- [牛顿-欧拉递推](./02_Newton_Euler.md)
- [轨迹规划](./03_Trajectory_Planning.md)

---

## 动力学基础

机器人动力学描述机器人运动与力的关系，是机器人控制的基础。

### 动力学方程

标准形式的机器人动力学方程：

```
M(q)q̈ + C(q, q̇)q̇ + G(q) = τ
```

其中：

- **M(q)**: 质量矩阵 (n×n正定对称矩阵)
- **C(q, q̇)**: 科氏力和离心力矩阵 (n×n)
- **G(q)**: 重力向量 (n×1)
- **τ**: 关节力矩向量 (n×1)
- **q**: 关节位置向量 (n×1)

#### C语言数据结构

```c
#include <math.h>
#include <string.h>

#define MAX_JOINTS 7
#define MAX_LINKS 7

typedef struct {
    int n_joints;                    // 关节数量
    double mass[MAX_LINKS];          // 连杆质量
    double inertia[MAX_LINKS][3][3]; // 连杆惯性张量
    double com[MAX_LINKS][3];        // 质心位置 (相对连杆坐标系)
    double length[MAX_LINKS];        // 连杆长度
} RobotDynamics;

// 动力学计算结果
typedef struct {
    double M[MAX_JOINTS][MAX_JOINTS];  // 质量矩阵
    double C[MAX_JOINTS][MAX_JOINTS];  // 科氏力矩阵
    double G[MAX_JOINTS];               // 重力向量
} DynamicsMatrices;
```

---

## 深入理解

### 1. 拉格朗日动力学

#### 理论基础

拉格朗日方程基于能量方法：

```
d/dt(∂L/∂q̇) - ∂L/∂q = τ
```

其中拉格朗日量 L = T - V (动能 - 势能)

#### C实现：二连杆机器人动力学

```c
// 二连杆平面机器人动力学计算
// 参数: l1, l2 (连杆长度), m1, m2 (质量), I1, I2 (转动惯量)

typedef struct {
    double l1, l2;      // 连杆长度
    double m1, m2;      // 质量
    double I1, I2;      // 绕质心的转动惯量
    double lc1, lc2;    // 质心距离
    double g;           // 重力加速度
} TwoLinkParams;

void twolink_dynamics(
    const TwoLinkParams* p,
    const double q[2],      // 关节角度 [theta1, theta2]
    const double qd[2],     // 关节速度
    const double qdd[2],    // 关节加速度
    double tau[2]           // 输出力矩
) {
    double s2 = sin(q[1]);           // sin(theta2)
    double c2 = cos(q[1]);           // cos(theta2)
    double s12 = sin(q[0] + q[1]);   // sin(theta1 + theta2)
    double c12 = cos(q[0] + q[1]);   // cos(theta1 + theta2)

    // 质量矩阵 M(q)
    double M11 = p->m1*pow(p->lc1, 2) + p->I1 +
                 p->m2*(pow(p->l1, 2) + pow(p->lc2, 2) + 2*p->l1*p->lc2*c2) + p->I2;
    double M12 = p->m2*(pow(p->lc2, 2) + p->l1*p->lc2*c2) + p->I2;
    double M22 = p->m2*pow(p->lc2, 2) + p->I2;

    // 科氏力和离心力 C(q, qd)*qd
    double h = -p->m2 * p->l1 * p->lc2 * s2;
    double C1 = 2*h*qd[0]*qd[1] + h*qd[1]*qd[1];
    double C2 = -h*qd[0]*qd[0];

    // 重力 G(q)
    double G1 = (p->m1*p->lc1 + p->m2*p->l1)*p->g*cos(q[0]) +
                p->m2*p->lc2*p->g*c12;
    double G2 = p->m2*p->lc2*p->g*c12;

    // 计算力矩: tau = M*qdd + C + G
    tau[0] = M11*qdd[0] + M12*qdd[1] + C1 + G1;
    tau[1] = M12*qdd[0] + M22*qdd[1] + C2 + G2;
}

// 计算质量矩阵 (用于控制)
void twolink_mass_matrix(
    const TwoLinkParams* p,
    const double q[2],
    double M[2][2]
) {
    double c2 = cos(q[1]);

    M[0][0] = p->m1*pow(p->lc1, 2) + p->I1 +
              p->m2*(pow(p->l1, 2) + pow(p->lc2, 2) + 2*p->l1*p->lc2*c2) + p->I2;
    M[0][1] = p->m2*(pow(p->lc2, 2) + p->l1*p->lc2*c2) + p->I2;
    M[1][0] = M[0][1];
    M[1][1] = p->m2*pow(p->lc2, 2) + p->I2;
}
```

### 2. 牛顿-欧拉递推算法

#### 算法原理

牛顿-欧拉方法是计算效率最高的机器人动力学算法，时间复杂度O(n)。

**前向递推** (计算运动学量):

```
ω_i = ω_{i-1} + z_i * q̇_i
α_i = α_{i-1} + z_i * q̈_i + ω_{i-1} × z_i * q̇_i
a_i = a_{i-1} + α_i × r_i + ω_i × (ω_i × r_i)
```

**后向递推** (计算力/力矩):

```
F_i = m_i * a_{c,i}
N_i = I_i * α_i + ω_i × (I_i * ω_i)
tau_i = z_i^T * (N_i + r_{c,i} × F_i)
```

#### C实现

```c
typedef struct {
    double w[3];   // 角速度
    double wd[3];  // 角加速度
    double a[3];   // 线加速度
    double ac[3];  // 质心加速度
} LinkKinematics;

typedef struct {
    double F[3];   // 力
    double N[3];   // 力矩
    double tau;    // 关节力矩
} LinkDynamics;

// 牛顿-欧拉递推算法
void newton_euler_dynamics(
    const RobotDynamics* robot,
    const double q[MAX_JOINTS],
    const double qd[MAX_JOINTS],
    const double qdd[MAX_JOINTS],
    double tau[MAX_JOINTS]
) {
    int n = robot->n_joints;
    LinkKinematics kin[MAX_LINKS];
    LinkDynamics dyn[MAX_LINKS];

    // 初始化基座
    memset(&kin[0], 0, sizeof(LinkKinematics));
    kin[0].a[2] = -9.81;  // 重力方向 (假设z向上)

    // 前向递推: 计算每个连杆的运动学量
    for (int i = 1; i <= n; i++) {
        // 获取变换参数 (简化假设，实际使用DH参数)
        double s = sin(q[i-1]);
        double c = cos(q[i-1]);

        double z[3] = {0, 0, 1};  // 旋转轴
        double r[3] = {robot->length[i-1] * c, robot->length[i-1] * s, 0};

        // 角速度传递
        for (int j = 0; j < 3; j++) {
            kin[i].w[j] = kin[i-1].w[j] + z[j] * qd[i-1];
        }

        // 角加速度传递
        double wxz[3];
        cross_product(kin[i-1].w, z, wxz);
        for (int j = 0; j < 3; j++) {
            kin[i].wd[j] = kin[i-1].wd[j] + z[j] * qdd[i-1] + wxz[j] * qd[i-1];
        }

        // 线加速度
        double wxr[3], wxwxr[3];
        cross_product(kin[i].w, r, wxr);
        cross_product(kin[i].w, wxr, wxwxr);

        double axr[3];
        cross_product(kin[i].wd, r, axr);

        for (int j = 0; j < 3; j++) {
            kin[i].a[j] = kin[i-1].a[j] + axr[j] + wxwxr[j];
        }

        // 质心加速度
        double rc[3] = {robot->com[i-1][0], robot->com[i-1][1], robot->com[i-1][2]};
        double wxrc[3], wxwxrc[3], axrc[3];
        cross_product(kin[i].w, rc, wxrc);
        cross_product(kin[i].w, wxrc, wxwxrc);
        cross_product(kin[i].wd, rc, axrc);

        for (int j = 0; j < 3; j++) {
            kin[i].ac[j] = kin[i].a[j] + axrc[j] + wxwxrc[j];
        }
    }

    // 后向递推: 计算力和力矩
    for (int i = n; i >= 1; i--) {
        // 连杆受力
        for (int j = 0; j < 3; j++) {
            dyn[i].F[j] = robot->mass[i-1] * kin[i].ac[j];
        }

        // 连杆受力矩
        double Iw[3];
        matrix_vector_mult(robot->inertia[i-1], kin[i].w, Iw);

        double wxIw[3];
        cross_product(kin[i].w, Iw, wxIw);

        double Iwd[3];
        matrix_vector_mult(robot->inertia[i-1], kin[i].wd, Iwd);

        for (int j = 0; j < 3; j++) {
            dyn[i].N[j] = Iwd[j] + wxIw[j];
        }

        // 关节力矩
        double rc[3] = {robot->com[i-1][0], robot->com[i-1][1], robot->com[i-1][2]};
        double rc_cross_F[3];
        cross_product(rc, dyn[i].F, rc_cross_F);

        double z[3] = {0, 0, 1};
        dyn[i].tau = 0;
        for (int j = 0; j < 3; j++) {
            dyn[i].tau += z[j] * (dyn[i].N[j] + rc_cross_F[j]);
        }

        tau[i-1] = dyn[i].tau;
    }
}

// 辅助函数: 叉积
void cross_product(const double a[3], const double b[3], double result[3]) {
    result[0] = a[1]*b[2] - a[2]*b[1];
    result[1] = a[2]*b[0] - a[0]*b[2];
    result[2] = a[0]*b[1] - a[1]*b[0];
}

// 辅助函数: 矩阵向量乘法 (3x3)
void matrix_vector_mult(const double M[3][3], const double v[3], double result[3]) {
    for (int i = 0; i < 3; i++) {
        result[i] = 0;
        for (int j = 0; j < 3; j++) {
            result[i] += M[i][j] * v[j];
        }
    }
}
```

### 3. 轨迹规划

#### S曲线轨迹规划 (S-Curve Trajectory)

S曲线规划提供平滑的加加速度(jerk)限制，减少机械振动。

```c
typedef struct {
    double q0, q1;      // 起点和终点
    double v_max;       // 最大速度
    double a_max;       // 最大加速度
    double j_max;       // 最大加加速度(jerk)

    // 计算得到的参数
    double T;           // 总时间
    double Ta, Tv, Td;  // 加速、匀速、减速段时间
    double aa, ad;      // 实际加速度和减速度
} SCurveProfile;

// 计算S曲线轨迹参数
void scurve_profile_calculate(SCurveProfile* prof) {
    double dq = prof->q1 - prof->q0;

    // 计算最小加减速时间 (达到最大加速度)
    double Tj = prof->a_max / prof->j_max;

    // 假设可以达到最大速度
    prof->Ta = prof->Tv = prof->Td = 0;

    // 计算加速段时间
    double v_max_candidate = sqrt(fabs(dq) * prof->a_max);

    if (v_max_candidate <= prof->v_max) {
        // 三角形速度曲线 (达不到最大速度)
        prof->aa = prof->ad = v_max_candidate;
        prof->Ta = prof->Td = 2 * sqrt(fabs(dq) / prof->a_max);
        prof->Tv = 0;
        prof->T = prof->Ta + prof->Td;
    } else {
        // 梯形速度曲线
        prof->aa = prof->ad = prof->a_max;
        prof->Ta = prof->Td = prof->v_max / prof->a_max;

        double d_accel = 0.5 * prof->aa * prof->Ta * prof->Ta;
        double d_const = fabs(dq) - 2 * d_accel;

        if (d_const > 0) {
            prof->Tv = d_const / prof->v_max;
        } else {
            prof->Tv = 0;
        }

        prof->T = prof->Ta + prof->Tv + prof->Td;
    }
}

// 获取t时刻的轨迹点
void scurve_evaluate(const SCurveProfile* prof, double t, double* q, double* qd, double* qdd) {
    double sign = (prof->q1 > prof->q0) ? 1 : -1;

    // 加速段
    if (t < prof->Ta) {
        *qdd = sign * prof->aa;
        *qd = sign * prof->aa * t;
        *q = prof->q0 + sign * 0.5 * prof->aa * t * t;
    }
    // 匀速段
    else if (t < prof->Ta + prof->Tv) {
        double t_const = t - prof->Ta;
        *qdd = 0;
        *qd = sign * prof->v_max;
        *q = prof->q0 + sign * (0.5 * prof->aa * prof->Ta * prof->Ta + prof->v_max * t_const);
    }
    // 减速段
    else if (t < prof->T) {
        double t_dec = t - prof->Ta - prof->Tv;
        *qdd = -sign * prof->ad;
        *qd = sign * (prof->v_max - prof->ad * t_dec);
        *q = prof->q1 - sign * (0.5 * prof->ad * (prof->T - t) * (prof->T - t));
    }
    // 结束
    else {
        *qdd = 0;
        *qd = 0;
        *q = prof->q1;
    }
}
```

### 4. 实际应用案例

#### 案例: 机械臂重力补偿 + PD控制

```c
// 基于动力学的重力补偿控制
void gravity_compensation_control(
    const RobotDynamics* robot,
    const double q[MAX_JOINTS],
    const double qd[MAX_JOINTS],
    const double q_desired[MAX_JOINTS],
    double tau[MAX_JOINTS]
) {
    // 计算重力项
    double G[MAX_JOINTS];
    compute_gravity_vector(robot, q, G);

    // PD反馈增益
    double Kp[MAX_JOINTS] = {100, 100, 50, 50, 20, 10, 5};
    double Kd[MAX_JOINTS] = {20, 20, 10, 10, 5, 2, 1};

    for (int i = 0; i < robot->n_joints; i++) {
        double e = q_desired[i] - q[i];
        double ed = -qd[i];  // 假设期望速度为0

        // 重力补偿 + PD控制
        tau[i] = G[i] + Kp[i] * e + Kd[i] * ed;
    }
}

// 计算重力向量 (g=0, qd=0, qdd=0时的逆动力学)
void compute_gravity_vector(
    const RobotDynamics* robot,
    const double q[MAX_JOINTS],
    double G[MAX_JOINTS]
) {
    double zero[MAX_JOINTS] = {0};
    newton_euler_dynamics(robot, q, zero, zero, G);
}
```

---

## 最佳实践

1. **数值稳定性**: 使用LU分解或Cholesky分解求解线性系统
2. **计算效率**: 利用动力学模型的稀疏性优化计算
3. **实时性**: 对于高自由度机器人，使用递归算法而非闭式解
4. **验证方法**: 使用能量守恒验证动力学实现正确性

---

## 权威参考

- [1] Craig, J.J. "Introduction to Robotics: Mechanics and Control", Pearson, 2018
- [2] Siciliano, B. et al. "Robotics: Modelling, Planning and Control", Springer, 2009
- [3] Featherstone, R. "Rigid Body Dynamics Algorithms", Springer, 2014
- [4] Lynch, K.M. & Park, F.C. "Modern Robotics", Cambridge University Press, 2017

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
