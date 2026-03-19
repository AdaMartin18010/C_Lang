# 机器人运动学

> **层级定位**: 18_Robotics_Physical_Control > 02_Kinematics
> **难度级别**: L5
> **前置知识**: 线性代数、三角学、C语言高级

---

## 目录

1. [齐次变换矩阵](./01_Homogeneous_Transforms.md)
2. [DH参数法](./02_DH_Parameters.md)
3. [正向运动学](./03_Forward_Kinematics.md)
4. [逆向运动学](./04_Inverse_Kinematics.md)
5. [雅可比矩阵](./05_Jacobian.md)

---

## 核心概念

### 齐次变换矩阵

4×4矩阵表示位置和姿态：

```
| R  t |
| 0  1 |
```

其中R是3×3旋转矩阵，t是3×1平移向量。

### DH参数

Denavit-Hartenberg参数描述连杆关系：

- θ: 关节角
- d: 连杆偏移
- a: 连杆长度
- α: 连杆扭角

### C语言实现

```c
typedef struct {
    double data[4][4];
} Transform;

void multiply_transform(const Transform* A, const Transform* B, Transform* result);
void inverse_transform(const Transform* T, Transform* result);
```

---

> **最后更新**: 2026-03-21
