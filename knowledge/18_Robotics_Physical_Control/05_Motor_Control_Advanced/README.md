
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

# 高级电机控制 (Advanced Motor Control)

> **层级定位**: 18_Robotics_Physical_Control > 05_Motor_Control_Advanced
> **难度级别**: ⭐⭐⭐⭐⭐ L5
> **前置知识**: 电机基础、控制理论、嵌入式C、矢量数学
> **参考标准**: IEEE Industrial Electronics Society, TI Motor Control Lab

---

## 目录

1. [FOC理论基础](./01_FOC_Theory.md)
2. [SVPWM实现](./02_SVPWM.md)
3. [无感FOC](./03_Sensorless_FOC.md)
4. [电机参数辨识](./04_Parameter_Identification.md)

---

## 高级控制技术

### 磁场定向控制 (Field-Oriented Control, FOC)

FOC将三相交流电机控制转换为类似直流电机的控制，通过将电流分解为励磁分量(d轴)和转矩分量(q轴)。

#### 核心概念

```
 Clarke变换: Iαβ = Clarke(Iabc)
 Park变换:  Idq = Park(Iαβ, θ)

 逆Park变换: Vαβ = InvPark(Vdq, θ)
 SVPWM:      Vabc = SVPWM(Vαβ)
```

#### C语言实现

```c
#include <math.h>
#include <stdint.h>

// FOC状态结构体
typedef struct {
    // 电流反馈
    float i_a, i_b, i_c;      // 三相电流
    float i_alpha, i_beta;    // Clarke变换后
    float i_d, i_q;           // Park变换后

    // 电压输出
    float v_d, v_q;           // d-q轴电压
    float v_alpha, v_beta;    // 逆Park变换后
    float v_a, v_b, v_c;      // 三相电压

    // 位置和速度
    float theta;              // 电角度 (rad)
    float omega;              // 电角速度 (rad/s)

    // PI控制器
    struct {
        float kp, ki;
        float integral;
        float output_limit;
    } pi_d, pi_q;             // d轴和q轴电流环PI

    // 电机参数
    float rs;                 // 定子电阻 (Ω)
    float ld, lq;             // d-q轴电感 (H)
    float flux_linkage;       // 永磁体磁链 (Wb)
    float pole_pairs;         // 极对数
} FOC_Controller;

// 初始化FOC控制器
void foc_init(FOC_Controller* foc, float rs, float ld, float lq,
              float flux, int pole_pairs) {
    foc->rs = rs;
    foc->ld = ld;
    foc->lq = lq;
    foc->flux_linkage = flux;
    foc->pole_pairs = pole_pairs;

    // 电流环PI参数整定 (基于极点配置)
    float bandwidth = 1000;  // 电流环带宽 1kHz
    float wc = 2 * M_PI * bandwidth;

    foc->pi_d.kp = wc * ld;
    foc->pi_d.ki = wc * rs;
    foc->pi_d.integral = 0;
    foc->pi_d.output_limit = 12.0;  // 电压限制 (V)

    foc->pi_q.kp = wc * lq;
    foc->pi_q.ki = wc * rs;
    foc->pi_q.integral = 0;
    foc->pi_q.output_limit = 12.0;

    foc->i_d = foc->i_q = 0;
    foc->v_d = foc->v_q = 0;
}

// Clarke变换 (3相 -> 2相静止坐标系)
void clarke_transform(float i_a, float i_b, float i_c,
                      float* i_alpha, float* i_beta) {
    *i_alpha = i_a;
    *i_beta = (i_a + 2 * i_b) / sqrtf(3.0f);

    // 或者使用等幅值变换:
    // *i_alpha = (2*i_a - i_b - i_c) / 3;
    // *i_beta = (i_b - i_c) / sqrt(3);
}

// Park变换 (静止坐标系 -> 旋转坐标系)
void park_transform(float i_alpha, float i_beta, float theta,
                    float* i_d, float* i_q) {
    float cos_t = cosf(theta);
    float sin_t = sinf(theta);

    *i_d =  i_alpha * cos_t + i_beta * sin_t;
    *i_q = -i_alpha * sin_t + i_beta * cos_t;
}

// 逆Park变换
void inverse_park(float v_d, float v_q, float theta,
                  float* v_alpha, float* v_beta) {
    float cos_t = cosf(theta);
    float sin_t = sinf(theta);

    *v_alpha = v_d * cos_t - v_q * sin_t;
    *v_beta  = v_d * sin_t + v_q * cos_t;
}

// PI控制器更新
float pi_update(float* integral, float kp, float ki,
                float error, float dt, float limit) {
    *integral += error * dt;

    // 积分限幅
    if (*integral > limit / ki) *integral = limit / ki;
    if (*integral < -limit / ki) *integral = -limit / ki;

    float output = kp * error + ki * (*integral);

    // 输出限幅
    if (output > limit) output = limit;
    if (output < -limit) output = -limit;

    return output;
}

// FOC主控制循环 (每PWM周期调用一次, 如10kHz)
void foc_update(FOC_Controller* foc, float i_a, float i_b, float theta,
                float i_d_ref, float i_q_ref, float dt,
                float* pwm_a, float* pwm_b, float* pwm_c) {

    // 1. 计算第三相电流
    float i_c = -(i_a + i_b);
    foc->i_a = i_a;
    foc->i_b = i_b;
    foc->i_c = i_c;

    // 2. Clarke变换
    clarke_transform(i_a, i_b, i_c, &foc->i_alpha, &foc->i_beta);

    // 3. Park变换
    foc->theta = theta;
    park_transform(foc->i_alpha, foc->i_beta, theta, &foc->i_d, &foc->i_q);

    // 4. d-q轴电流PI控制 (含解耦和前馈)
    float error_d = i_d_ref - foc->i_d;
    float error_q = i_q_ref - foc->i_q;

    // 电压前馈 (解耦项)
    float v_d_ff = -foc->omega * foc->lq * foc->i_q;  // 交叉耦合项
    float v_q_ff =  foc->omega * foc->ld * foc->i_d +
                    foc->omega * foc->flux_linkage;   // 反电动势

    foc->v_d = pi_update(&foc->pi_d.integral, foc->pi_d.kp, foc->pi_d.ki,
                         error_d, dt, foc->pi_d.output_limit) + v_d_ff;
    foc->v_q = pi_update(&foc->pi_q.integral, foc->pi_q.kp, foc->pi_q.ki,
                         error_q, dt, foc->pi_q.output_limit) + v_q_ff;

    // 5. 逆Park变换
    inverse_park(foc->v_d, foc->v_q, theta, &foc->v_alpha, &foc->v_beta);

    // 6. SVPWM计算
    svpwm_calculate(foc->v_alpha, foc->v_beta, 24.0f,  // 24V母线电压
                    pwm_a, pwm_b, pwm_c);
}
```

### 空间矢量PWM (SVPWM)

SVPWM比正弦PWM提高约15%的电压利用率。

```c
// SVPWM实现
void svpwm_calculate(float v_alpha, float v_beta, float vdc,
                     float* duty_a, float* duty_b, float* duty_c) {

    // 归一化到单位电压
    float v_ref = sqrtf(v_alpha*v_alpha + v_beta*v_beta) / (vdc / sqrtf(3.0f));
    float angle = atan2f(v_beta, v_alpha);

    // 确定扇区 (1-6)
    int sector;
    if (angle >= 0 && angle < M_PI/3) sector = 1;
    else if (angle >= M_PI/3 && angle < 2*M_PI/3) sector = 2;
    else if (angle >= 2*M_PI/3 && angle < M_PI) sector = 3;
    else if (angle >= -M_PI && angle < -2*M_PI/3) sector = 4;
    else if (angle >= -2*M_PI/3 && angle < -M_PI/3) sector = 5;
    else sector = 6;

    // 计算相邻矢量作用时间
    float x = v_ref * sinf(M_PI/3 - angle + (sector-1)*M_PI/3);
    float y = v_ref * sinf(angle - (sector-1)*M_PI/3);
    float z = 1 - x - y;

    float t1, t2, t0;
    switch(sector) {
        case 1: t1 = z; t2 = y; break;
        case 2: t1 = y; t2 = -x; break;
        case 3: t1 = -z; t2 = x; break;
        case 4: t1 = -x; t2 = -y; break;
        case 5: t1 = x; t2 = z; break;
        case 6: t1 = -y; t2 = -z; break;
        default: t1 = t2 = 0;
    }

    // 限制
    if (t1 + t2 > 1) {
        float sum = t1 + t2;
        t1 /= sum;
        t2 /= sum;
    }
    t0 = 1 - t1 - t2;

    // 计算占空比 (七段式SVPWM)
    float ta = (1 - t1 - t2) / 2;
    float tb = ta + t1;
    float tc = tb + t2;

    switch(sector) {
        case 1: *duty_a = tc; *duty_b = tb; *duty_c = ta; break;
        case 2: *duty_a = tb; *duty_b = tc; *duty_c = ta; break;
        case 3: *duty_a = ta; *duty_b = tc; *duty_c = tb; break;
        case 4: *duty_a = ta; *duty_b = tb; *duty_c = tc; break;
        case 5: *duty_b = ta; *duty_c = tb; *duty_a = tc; break;
        case 6: *duty_c = ta; *duty_a = tb; *duty_b = tc; break;
    }
}
```

---

## 深入理解

### 1. 无传感器FOC

基于观测器的转子位置估计，无需编码器。

```c
typedef struct {
    // 滑模观测器参数
    float h;              // 滑模增益
    float lpf_coef;       // 低通滤波系数

    // 状态估计
    float i_alpha_est, i_beta_est;
    float e_alpha_est, e_beta_est;  // 反电动势估计
    float theta_est;                // 角度估计
    float omega_est;                // 速度估计

    // PLL参数
    float pll_kp, pll_ki;
    float pll_integral;
} SensorlessObserver;

// 滑模观测器更新
void smo_update(SensorlessObserver* obs, float i_alpha, float i_beta,
                float v_alpha, float v_beta, float rs, float ls, float dt) {

    // 电流误差
    float err_alpha = i_alpha - obs->i_alpha_est;
    float err_beta  = i_beta  - obs->i_beta_est;

    // 滑模控制律 (符号函数)
    float z_alpha = (err_alpha > 0) ? obs->h : -obs->h;
    float z_beta  = (err_beta > 0)  ? obs->h : -obs->h;

    // 状态观测
    float di_alpha = (v_alpha - rs*i_alpha - z_alpha) / ls;
    float di_beta  = (v_beta  - rs*i_beta  - z_beta)  / ls;

    obs->i_alpha_est += di_alpha * dt;
    obs->i_beta_est  += di_beta  * dt;

    // 反电动势估计 (滤波后的滑模信号)
    obs->e_alpha_est = obs->e_alpha_est * obs->lpf_coef + z_alpha * (1 - obs->lpf_coef);
    obs->e_beta_est  = obs->e_beta_est  * obs->lpf_coef + z_beta  * (1 - obs->lpf_coef);
}

// PLL角度跟踪
void pll_update(SensorlessObserver* obs, float dt) {
    // 计算误差角度
    float error = -obs->e_alpha_est * cosf(obs->theta_est) -
                   obs->e_beta_est  * sinf(obs->theta_est);

    // PI调节器
    obs->pll_integral += error * dt;
    obs->omega_est = obs->pll_kp * error + obs->pll_ki * obs->pll_integral;

    // 更新角度
    obs->theta_est += obs->omega_est * dt;

    // 归一化到0-2π
    while (obs->theta_est > 2*M_PI) obs->theta_est -= 2*M_PI;
    while (obs->theta_est < 0) obs->theta_est += 2*M_PI;
}
```

### 2. MTPA控制 (最大转矩电流比)

对于内置式永磁同步电机(IPMSM)，利用磁阻转矩提高转矩输出。

```c
// MTPA查表计算 (离线计算，运行时查表)
float mtpa_id_lookup[100];  // i_d查表
float mtpa_iq_lookup[100];  // i_q查表

// 初始化MTPA表
void mtpa_table_init(float ld, float lq, float flux) {
    for (int i = 0; i < 100; i++) {
        float i_s = i * 0.1f;  // 电流幅值 0-10A

        // 解析解: id = (flux - sqrt(flux^2 + 4*(Lq-Ld)^2*is^2)) / (2*(Lq-Ld))
        float delta_l = lq - ld;
        float id = (flux - sqrtf(flux*flux + 4*delta_l*delta_l*i_s*i_s)) / (2*delta_l);
        float iq = sqrtf(i_s*i_s - id*id);

        mtpa_id_lookup[i] = id;
        mtpa_iq_lookup[i] = iq;
    }
}

// 获取MTPA电流给定
void mtpa_get_current_ref(float torque_ref, float* id_ref, float* iq_ref) {
    // 根据转矩计算电流幅值 (简化)
    float kt = 0.1;  // 转矩常数 Nm/A
    float i_s = fabsf(torque_ref) / kt;

    int idx = (int)(i_s * 10);
    if (idx > 99) idx = 99;

    *id_ref = (torque_ref > 0) ? mtpa_id_lookup[idx] : mtpa_id_lookup[idx];
    *iq_ref = (torque_ref > 0) ? mtpa_iq_lookup[idx] : -mtpa_iq_lookup[idx];
}
```

### 3. 实际应用案例

#### 案例: 无人机电机FOC控制

```c
// 无人机无刷电机FOC控制器
typedef struct {
    FOC_Controller foc;
    SensorlessObserver observer;

    // 速度环
    struct {
        float kp, ki;
        float integral;
    } speed_pi;

    float speed_ref;      // 速度给定 (rad/s)
    float current_limit;  // 电流限制 (A)
} DroneMotorController;

// 初始化
void drone_motor_init(DroneMotorController* mc) {
    // 电机参数: 2204无刷电机
    foc_init(&mc->foc, 0.08f, 0.000015f, 0.000020f, 0.005f, 7);

    // 速度环PI
    mc->speed_pi.kp = 0.005;
    mc->speed_pi.ki = 0.01;
    mc->speed_pi.integral = 0;

    mc->current_limit = 20.0;  // 最大电流20A
}

// 控制循环 (每1ms调用一次)
void drone_motor_control(DroneMotorController* mc,
                         float i_a, float i_b,      // 相电流
                         float v_alpha, float v_beta, // 电压测量
                         float dt,
                         float* pwm_a, float* pwm_b, float* pwm_c) {

    // 1. 无传感器角度估计
    smo_update(&mc->observer, i_a, i_b, v_alpha, v_beta,
               mc->foc.rs, mc->foc.ld, dt);
    pll_update(&mc->observer, dt);

    float theta = mc->observer.theta_est;
    float speed = mc->observer.omega_est / mc->foc.pole_pairs;  // 机械速度

    // 2. 速度环PI控制
    float speed_error = mc->speed_ref - speed;
    mc->speed_pi.integral += speed_error * dt;
    float i_q_ref = mc->speed_pi.kp * speed_error + mc->speed_pi.ki * mc->speed_pi.integral;

    // 电流限制
    if (i_q_ref > mc->current_limit) i_q_ref = mc->current_limit;
    if (i_q_ref < -mc->current_limit) i_q_ref = -mc->current_limit;

    // 3. FOC电流控制
    foc_update(&mc->foc, i_a, i_b, theta,
               0, i_q_ref, dt,  // id=0控制
               pwm_a, pwm_b, pwm_c);
}
```

---

## 最佳实践

1. **电流采样**: 使用双电阻或三电阻采样，在PWM下桥臂导通时刻采样
2. **角度同步**: 确保电流采样和PWM更新同步，避免相位延迟
3. **过流保护**: 硬件过流保护 + 软件电流限制
4. **参数辨识**: 使用RLS或高频注入法在线辨识电机参数

---

## 权威参考

- [1] Texas Instruments, "InstaSPIN-FOC User's Guide", SPRUHJ1
- [2] ST Microelectronics, "PMSM Sensorless FOC", AN1078
- [3] Microchip, "AN1078 - Sensorless Field Oriented Control"
- [4] Lorenz, R.D. et al. "Motion Control with Induction Motors", IEEE Proceedings, 1994

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
> **内容深度**: L5 (研究级)
