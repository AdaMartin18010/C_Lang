# 深海计算


---

## 📑 目录

- [深海计算](#深海计算)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [水下通信](#水下通信)
    - [水声通信](#水声通信)
    - [水下光学通信](#水下光学通信)
  - [压力容器计算](#压力容器计算)
    - [压力壳设计](#压力壳设计)
    - [耐压电子舱设计](#耐压电子舱设计)
  - [低功耗设计](#低功耗设计)
    - [功耗管理策略](#功耗管理策略)
    - [能量收集](#能量收集)
  - [传感器网络](#传感器网络)
    - [水下传感节点](#水下传感节点)
    - [数据融合](#数据融合)
    - [网络路由](#网络路由)
  - [参考资源](#参考资源)


---

## 概述

深海计算（Deep Sea Computing）是指在水下高压、低温、黑暗环境中进行的计算活动。深海环境对电子设备提出了独特挑战，包括极端压力（可达1100个大气压）、腐蚀性海水、有限的能源供应和困难的通信条件。

## 水下通信

### 水声通信

水声通信是深海最主要的通信方式：

```c
// 水声通信物理层参数
typedef struct {
    float carrier_freq;      // 载波频率 (Hz)
    float bandwidth;         // 带宽 (Hz)
    float symbol_rate;       // 符号率 (baud)
    int modulation_type;     // 调制方式
    float tx_power;          // 发射功率 (W)
} AcousticConfig;

// 调制方式枚举
typedef enum {
    MOD_FSK,        // 频移键控
    MOD_PSK,        // 相移键控
    MOD_OFDM,       // 正交频分复用
    MOD_DSSS        // 直接序列扩频
} ModulationType;

// OFDM 水声通信实现
#define OFDM_SUBCARRIERS 64
#define OFDM_CP_LEN 16      // 循环前缀长度
#define OFDM_SYMBOL_LEN (OFDM_SUBCARRIERS + OFDM_CP_LEN)

typedef struct {
    float real;
    float imag;
} complex_float;

// IFFT 实现（基2时域抽取）
void ifft(complex_float *in, complex_float *out, int n) {
    // 位反转排列
    for (int i = 0; i < n; i++) {
        int j = bit_reverse(i, log2(n));
        out[j] = in[i];
    }

    // 蝶形运算
    for (int s = 1; s <= log2(n); s++) {
        int m = 1 << s;
        complex_float wm = {
            cosf(2 * M_PI / m),
            -sinf(2 * M_PI / m)
        };

        for (int k = 0; k < n; k += m) {
            complex_float w = {1.0f, 0.0f};
            for (int j = 0; j < m/2; j++) {
                complex_float t = complex_mul(w, out[k + j + m/2]);
                complex_float u = out[k + j];
                out[k + j] = complex_add(u, t);
                out[k + j + m/2] = complex_sub(u, t);
                w = complex_mul(w, wm);
            }
        }
    }
}

// OFDM 调制器
void ofdm_modulate(const uint8_t *data, size_t len,
                   float *output, const AcousticConfig *cfg) {
    complex_float freq_domain[OFDM_SUBCARRIERS];
    complex_float time_domain[OFDM_SUBCARRIERS];

    size_t bits_per_symbol = 2;  // QPSK
    size_t symbols = (len * 8 + bits_per_symbol - 1) / bits_per_symbol;

    for (size_t s = 0; s < symbols; s++) {
        // 将比特映射到星座点
        for (int k = 0; k < OFDM_SUBCARRIERS; k++) {
            int bit_idx = s * OFDM_SUBCARRIERS * bits_per_symbol +
                          k * bits_per_symbol;
            if (bit_idx / 8 < len) {
                int bits = (data[bit_idx / 8] >> (bit_idx % 8)) & 0x3;
                // QPSK 映射
                freq_domain[k].real = (bits & 1) ? 0.707f : -0.707f;
                freq_domain[k].imag = (bits & 2) ? 0.707f : -0.707f;
            } else {
                freq_domain[k].real = 0;
                freq_domain[k].imag = 0;
            }
        }

        // IFFT
        ifft(freq_domain, time_domain, OFDM_SUBCARRIERS);

        // 添加循环前缀
        int out_idx = s * OFDM_SYMBOL_LEN;
        for (int i = 0; i < OFDM_CP_LEN; i++) {
            output[out_idx + i] = time_domain[OFDM_SUBCARRIERS -
                                              OFDM_CP_LEN + i].real;
        }
        for (int i = 0; i < OFDM_SUBCARRIERS; i++) {
            output[out_idx + OFDM_CP_LEN + i] = time_domain[i].real;
        }
    }
}
```

### 水下光学通信

```c
// 蓝绿激光通信（短距离高速）
#define OPTICAL_WAVELENGTH 450e-9  // 450nm 蓝光
#define PULSE_DURATION 1e-9        // 1ns 脉冲

typedef struct {
    float wavelength;
    float beam_divergence;      // 发散角
    float tx_power;
    float receiver_aperture;
    float filter_bandwidth;
} OpticalLinkConfig;

// 光信道建模
float calculate_optical_loss(float distance, float attenuation_coef,
                             float scattering_coef) {
    // Beer-Lambert 定律
    float absorption_loss = exp(-attenuation_coef * distance);

    // 散射损耗（简化模型）
    float scattering_loss = exp(-scattering_coef * distance);

    return absorption_loss * scattering_loss;
}

// 光通信接收机
void optical_receiver(float *adc_samples, size_t num_samples,
                      uint8_t *output_bits) {
    // 带通滤波
    float filtered[1024];
    bandpass_filter(adc_samples, filtered, num_samples,
                    400e6, 500e6, 1e9);  // 400-500MHz 带通

    // 包络检测
    float envelope[1024];
    for (size_t i = 0; i < num_samples; i++) {
        envelope[i] = sqrtf(filtered[i] * filtered[i]);
    }

    // 阈值判决
    float threshold = calculate_adaptive_threshold(envelope, num_samples);

    size_t bit_idx = 0;
    for (size_t i = 0; i < num_samples; i += SAMPLES_PER_BIT) {
        float energy = 0;
        for (size_t j = 0; j < SAMPLES_PER_BIT && i + j < num_samples; j++) {
            energy += envelope[i + j];
        }
        output_bits[bit_idx++] = (energy > threshold) ? 1 : 0;
    }
}
```

## 压力容器计算

### 压力壳设计

```c
// 球壳压力容器设计计算

typedef struct {
    float inner_radius;      // 内半径 (m)
    float outer_radius;      // 外半径 (m)
    float yield_strength;    // 材料屈服强度 (Pa)
    float safety_factor;
    float elastic_modulus;   // 弹性模量
    float poisson_ratio;     // 泊松比
} PressureVessel;

// 拉梅公式计算球壳应力
void calculate_spherical_stress(PressureVessel *v, float pressure,
                                float *max_stress, float *max_strain) {
    float a = v->inner_radius;
    float b = v->outer_radius;
    float p = pressure;

    // 内表面切向应力（最大应力点）
    float sigma_t_inner = 1.5 * p * (b*b*b) / (b*b*b - a*a*a);

    // 外表面切向应力
    float sigma_t_outer = 1.5 * p * (a*a*a) / (b*b*b - a*a*a);

    *max_stress = sigma_t_inner;

    // 应变计算
    float E = v->elastic_modulus;
    float nu = v->poisson_ratio;
    *max_strain = sigma_t_inner * (1 - 2*nu) / E;

    // 安全系数检查
    float allowable_stress = v->yield_strength / v->safety_factor;
    if (*max_stress > allowable_stress) {
        printf("警告：应力超过许用值\n");
    }
}

// 计算所需壁厚
float calculate_required_thickness(float inner_radius, float pressure,
                                   float yield_strength,
                                   float safety_factor) {
    float allowable = yield_strength / safety_factor;
    // 薄壁近似
    float t_min = pressure * inner_radius / (2 * allowable);
    // 厚壁修正
    float ratio = inner_radius / t_min;
    if (ratio < 10) {
        // 需要迭代求解
        float t = t_min;
        for (int i = 0; i < 10; i++) {
            float a = inner_radius;
            float b = inner_radius + t;
            float stress = 1.5 * pressure * b*b*b / (b*b*b - a*a*a);
            if (stress > allowable) {
                t *= 1.1f;
            } else {
                break;
            }
        }
        return t;
    }
    return t_min;
}
```

### 耐压电子舱设计

```c
// 电子舱热设计
typedef struct {
    float ambient_temp;      // 环境温度 (K)
    float max_component_temp; // 元件最高温度
    float power_dissipation;  // 功耗 (W)
    float thermal_resistance_case_ambient;
    float thermal_resistance_junction_case;
} ThermalDesign;

// 计算所需散热能力
float calculate_required_thermal_resistance(ThermalDesign *design) {
    float delta_t = design->max_component_temp - design->ambient_temp;
    return delta_t / design->power_dissipation;
}

// 油补偿设计
#define COMPENSATION_OIL "硅油"
#define OIL_COMPRESSIBILITY 1.0e-9  // Pa^-1

typedef struct {
    float internal_volume;       // 内部容积
    float piston_area;           // 补偿活塞面积
    float max_piston_travel;     // 最大行程
    float spring_constant;       // 弹簧刚度
} OilCompensator;

// 计算活塞位移
float calculate_piston_displacement(OilCompensator *comp,
                                     float pressure_delta) {
    // 油液体积压缩
    float volume_change = comp->internal_volume *
                          OIL_COMPRESSIBILITY * pressure_delta;

    // 弹簧压缩
    float spring_compression = pressure_delta * comp->piston_area /
                               comp->spring_constant;

    // 总位移
    return volume_change / comp->piston_area + spring_compression;
}
```

## 低功耗设计

### 功耗管理策略

```c
// 功耗状态机
typedef enum {
    PWR_ACTIVE,         // 全速运行
    PWR_IDLE,           // 空闲等待
    PWR_SLEEP,          // 睡眠
    PWR_DEEP_SLEEP,     // 深度睡眠
    PWR_HIBERNATE       // 休眠
} PowerState;

typedef struct {
    PowerState current_state;
    uint32_t active_duration;
    uint32_t sleep_duration;
    float battery_voltage;
    float current_consumption;
} PowerManager;

// 功耗数据（典型值）
const float power_consumption[] = {
    [PWR_ACTIVE] = 2.5f,      // 2.5W
    [PWR_IDLE] = 0.5f,        // 0.5W
    [PWR_SLEEP] = 0.05f,      // 50mW
    [PWR_DEEP_SLEEP] = 0.005f, // 5mW
    [PWR_HIBERNATE] = 0.001f  // 1mW
};

// 任务调度优化
void optimized_task_schedule(void) {
    // 批处理传感器数据采集
    SensorReading batch[10];

    // 一次性采集多个样本
    for (int i = 0; i < 10; i++) {
        batch[i] = read_sensor_burst();
        if (i < 9) {
            // 传感器内部平均，MCU 休眠
            enter_sleep_mode(100);  // 100ms
        }
    }

    // 批量处理
    process_sensor_batch(batch, 10);

    // 压缩后存储
    uint8_t compressed[256];
    int compressed_len = compress_sensor_data(batch, 10,
                                               compressed, 256);
    write_to_storage(compressed, compressed_len);
}

// 自适应采样率
float adaptive_sample_rate(float signal_bandwidth,
                           float snr_estimate) {
    // 奈奎斯特采样
    float nyquist_rate = 2.0f * signal_bandwidth;

    // 根据 SNR 调整过采样率
    float oversample_ratio = (snr_estimate > 20.0f) ? 4.0f : 8.0f;

    return nyquist_rate * oversample_ratio;
}
```

### 能量收集

```c
// 温差发电（TEG）管理
typedef struct {
    float seebeck_coeff;     // 塞贝克系数 (V/K)
    float thermal_resistance;
    float electrical_resistance;
    float max_power;
} TEG_Module;

// 计算 TEG 输出功率
float calculate_teg_power(TEG_Module *teg, float delta_t) {
    // 开路电压
    float v_open = teg->seebeck_coeff * delta_t;

    // 最大功率传输（负载匹配）
    float v_load = v_open / 2.0f;
    float i_load = v_load / teg->electrical_resistance;

    return v_load * i_load;
}

// 最大功率点跟踪（MPPT）
typedef struct {
    float voltage;
    float current;
    float power;
    float duty_cycle;
} MPPT_State;

void mppt_update(MPPT_State *state, float v_new, float i_new) {
    float p_new = v_new * i_new;

    // 扰动观察法
    if (p_new > state->power) {
        // 功率增加，继续同方向调整
        state->duty_cycle += (v_new > state->voltage) ? 0.01f : -0.01f;
    } else {
        // 功率减小，反向调整
        state->duty_cycle -= (v_new > state->voltage) ? 0.01f : -0.01f;
    }

    // 限幅
    if (state->duty_cycle > 0.95f) state->duty_cycle = 0.95f;
    if (state->duty_cycle < 0.05f) state->duty_cycle = 0.05f;

    state->voltage = v_new;
    state->current = i_new;
    state->power = p_new;
}
```

## 传感器网络

### 水下传感节点

```c
// 传感器节点数据结构
typedef struct {
    uint32_t node_id;
    float temperature;
    float pressure;
    float salinity;
    float dissolved_oxygen;
    uint64_t timestamp;
    float battery_level;
} SensorNodeData;

// 时间同步（使用水声传播延迟补偿）
typedef struct {
    float x, y, z;           // 节点位置
    float clock_offset;      // 时钟偏移
    float clock_drift;       // 时钟漂移
} NodeClockSync;

// 单向时间同步
void oneway_time_sync(NodeClockSync *node,
                      float sender_time,
                      float arrival_time) {
    // 估算传播延迟（需要预先知道距离）
    float distance = sqrtf(node->x*node->x + node->y*node->y +
                           node->z*node->z);
    float propagation_delay = distance / SOUND_SPEED_WATER;

    // 计算时钟偏移
    node->clock_offset = sender_time + propagation_delay - arrival_time;
}

// 双向时间同步（更精确）
float twoway_time_sync(NodeClockSync *node_a, NodeClockSync *node_b,
                       float t1, float t2, float t3, float t4) {
    // t1: A 发送时间, t2: B 接收时间
    // t3: B 发送时间, t4: A 接收时间

    float propagation_delay = ((t2 - t1) + (t4 - t3)) / 2.0f;
    float clock_offset = ((t2 - t1) - (t4 - t3)) / 2.0f;

    return clock_offset;
}
```

### 数据融合

```c
// 卡尔曼滤波器（多传感器融合）
typedef struct {
    float state[6];          // [x, y, z, vx, vy, vz]
    float covariance[6][6];
    float process_noise[6][6];
    float measurement_noise[3][3];
} KalmanFilter;

void kalman_predict(KalmanFilter *kf, float dt) {
    // 状态转移矩阵
    float F[6][6] = {
        {1, 0, 0, dt, 0, 0},
        {0, 1, 0, 0, dt, 0},
        {0, 0, 1, 0, 0, dt},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1}
    };

    // x = F * x
    float new_state[6] = {0};
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            new_state[i] += F[i][j] * kf->state[j];
        }
    }
    memcpy(kf->state, new_state, sizeof(new_state));

    // P = F * P * F^T + Q
    // 简化：只更新协方差
}

// 分布式数据融合
void distributed_fusion(SensorNodeData *neighbors, int num_neighbors,
                        SensorNodeData *local, float weights[]) {
    float temp_sum = 0;
    float pres_sum = 0;
    float total_weight = 0;

    for (int i = 0; i < num_neighbors; i++) {
        temp_sum += weights[i] * neighbors[i].temperature;
        pres_sum += weights[i] * neighbors[i].pressure;
        total_weight += weights[i];
    }

    // 加权平均
    local->temperature = (local->temperature + temp_sum) /
                         (1.0f + total_weight);
    local->pressure = (local->pressure + pres_sum) /
                      (1.0f + total_weight);
}
```

### 网络路由

```c
// 水下延迟容忍网络（DTN）路由
typedef struct {
    uint32_t dest_id;
    uint32_t next_hop;
    float encounter_prob;    // 相遇概率
    uint32_t last_encounter;
    float delay_estimate;
} RoutingEntry;

// 基于相遇概率的路由（PROPHET 协议简化版）
void update_encounter_prob(RoutingEntry *entry, uint32_t current_time) {
    // 老化因子
    float gamma = 0.98f;
    uint32_t time_delta = current_time - entry->last_encounter;

    // 概率衰减
    entry->encounter_prob *= powf(gamma, time_delta);

    // 相遇更新
    entry->encounter_prob += (1 - entry->encounter_prob) * 0.5f;
    entry->last_encounter = current_time;
}

// 数据包转发决策
int should_forward(RoutingEntry *local, RoutingEntry *relay,
                   uint32_t dest_id) {
    // 如果中继节点到目标的相遇概率更高，则转发
    if (relay->encounter_prob > local->encounter_prob) {
        return 1;
    }
    return 0;
}
```

## 参考资源

- [IEEE Oceanic Engineering Society](https://ieeeoes.org/)
-[深海工程技术](https://www.marine-technology.com/)
- [WHOI Deep Ocean Exploration](https://www.whoi.edu/)
- [NOAA Ocean Explorer](https://oceanexplorer.noaa.gov/)
