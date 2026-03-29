# ADC (模数转换器) 深度解析

> **核心参数**: 分辨率、采样率、精度、信噪比
> **架构类型**: SAR、Sigma-Delta、Flash、Pipeline
> **应用领域**: 传感器读取、信号采集、测量系统
> **最后更新**: 2026-03-29

---

## 1. ADC架构形式化分类

### 1.1 ADC架构比较矩阵

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       ADC架构类型比较矩阵                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  类型          分辨率      采样率        功耗        应用                    │
│  ────────────────────────────────────────────────────────────────────────  │
│                                                                             │
│  Flash         6-8 bit     >1 GSPS      极高        示波器、射频            │
│  (并行比较)     2^N个比较器  立即输出    面积大        超高速采样             │
│                                                                             │
│  Pipeline      8-16 bit    10-500 MSPS  高          通信、视频              │
│  (流水线)      多级级联    流水线延迟   中等面积       高速信号处理           │
│                                                                             │
│  SAR           8-20 bit    10 KSPS-10   低          嵌入式、工业            │
│  (逐次逼近)    1个比较器    MSPS        面积小        通用采集                │
│                                                                             │
│  Sigma-Delta   16-32 bit   1-1000 KSPS  极低        音频、精密测量          │
│  (Σ-Δ)         过采样+噪声  高延迟      数字为主       高精度传感器           │
│                整形                                                  高分辨率 │
│                                                                             │
│  Dual-Slope    12-24 bit   <100 SPS     极低        数字万用表              │
│  (双积分)      高精度      极慢         抗干扰强       精密测量               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 嵌入式MCU常见ADC架构

| MCU系列 | ADC类型 | 分辨率 | 最高采样率 | 特性 |
|---------|---------|--------|-----------|------|
| **STM32F1** | SAR | 12-bit | 1 MSPS | 基本功能 |
| **STM32F4** | SAR | 12-bit | 2.4 MSPS | 三重交错、DMA |
| **STM32H7** | SAR | 16-bit | 3.6 MSPS | 过采样、DFSDM |
| **STM32G4** | SAR | 12/16-bit | 4 MSPS | 高分辨率+硬件过采样 |
| **STM32U5** | SAR | 12/14-bit | 2 MSPS | 超低功耗 |
| **MSP430** | SAR | 12-bit | 200 KSPS | 超低功耗 |
| **ESP32** | SAR | 12-bit | 100 KSPS | WiFi集成 |

---

## 2. SAR ADC深度分析

### 2.1 SAR ADC工作原理

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       SAR ADC逐次逼近原理                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  电路结构:                                                                  │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                                                                     │   │
│  │   采样保持      DAC          比较器           SAR逻辑               │   │
│  │  ┌──────┐    ┌──────┐      ┌──────┐      ┌──────────┐              │   │
│  │  │  S/H │───→│  Vdac│─────→│  +   │─────→│ Successive│              │   │
│  │  │      │    │      │      │  -   │      │ Approx.   │              │   │
│  │  └──┬───┘    └──▲───┘      └──┬───┘      │ Register  │              │   │
│  │     │           │            │          └─────┬─────┘              │   │
│  │    Vin       数字输入        Vref              │                    │   │
│  │              (来自SAR)                         │                    │   │
│  │                                               ↓                    │   │
│  │                                          数字输出                   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│  转换过程 (以4-bit为例，输入Vin=6V，Vref=10V):                              │
│  ──────────────────────────────────────────────                             │
│                                                                             │
│  步骤   SAR值    Vdac = SAR/16 × Vref    比较    结果                       │
│  ─────────────────────────────────────────────────────                      │
│  1      1000     5.0V                    Vin>Vdac   保持1 (MSB=1)           │
│  2      1100     7.5V                    Vin<Vdac   改0 (第2位=0)           │
│  3      1010     6.25V                   Vin<Vdac   改0 (第3位=0)           │
│  4      1001     5.625V                  Vin>Vdac   保持1 (LSB=1)           │
│                                                                             │
│  结果: 1001 (二进制) = 9 (十进制)                                           │
│  对应电压: 9/16 × 10V = 5.625V ≈ 6V (量化误差)                              │
│                                                                             │
│  转换时间公式:                                                              │
│  T_conv = T_sample + N × T_clock + T_extra                                │
│  其中 N = 分辨率位数                                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.2 SAR ADC时序参数

**定义 2.1 (SAR ADC时序)**

```
完整转换周期 = 采样时间 + 转换时间

采样时间 (T_s):
  采样电容充电时间
  T_s = (R_s + R_adc) × C_adc × ln(2^(N+1))

  其中:
    R_s = 信号源阻抗
    R_adc = ADC输入阻抗
    C_adc = ADC采样电容
    N = 分辨率位数

转换时间 (T_c):
  逐次逼近过程时间
  T_c = N × T_adc_clock + T_sync

  其中:
    N = 分辨率 (如12)
    T_adc_clock = ADC时钟周期
    T_sync = 同步开销 (通常1-3周期)

最大采样率:
  f_max = 1 / (T_s + T_c)

示例 (STM32F407, 12-bit, 30MHz ADC时钟):
  T_s_min = 3 cycles = 100ns (最小采样)
  T_c = 12 cycles = 400ns
  T_total = 500ns
  f_max = 2 MSPS
```

### 2.3 采样时间选择决策树

```
采样时间选择
│
├── 信号源阻抗?
│   ├── 高阻抗 (>10kΩ)
│   │   └── 需要更长采样时间
│   │       ├── R_s = 10kΩ → T_s ≈ 15 cycles
│   │       ├── R_s = 50kΩ → T_s ≈ 84 cycles
│   │       └── R_s = 100kΩ → T_s ≈ 160 cycles
│   │
│   └── 低阻抗 (<1kΩ)
│       └── 短采样时间即可
│           └── T_s = 3-15 cycles
│
├── 信号频率?
│   ├── 高频信号 (>10kHz)
│   │   └── 使用外部运放缓冲
│   │       └── 降低源阻抗
│   │
│   └── 低频信号
│       └── 可容忍较长采样时间
│
└── 精度要求?
    ├── 高精度 (>12 ENOB)
    │   └── 增加采样时间确保完全充电
    │
    └── 一般精度
        └── 标准采样时间即可
```

---

## 3. ADC误差分析

### 3.1 误差类型矩阵

| 误差类型 | 描述 | 典型值 | 补偿方法 |
|----------|------|--------|----------|
| **失调误差 (Offset)** | 零输入非零输出 | ±1-2 LSB | 软件校准 |
| **增益误差 (Gain)** | 满量程偏差 | ±1-2 LSB | 软件校准 |
| **微分非线性 (DNL)** | 代码宽度偏离1LSB | ±0.5-1 LSB | 硬件改善 |
| **积分非线性 (INL)** | 传输函数偏离直线 | ±1-2 LSB | 硬件改善 |
| **量化误差** | 固有分辨率限制 | ±0.5 LSB | 过采样 |
| **噪声** | 热噪声+电源噪声 | 1-2 LSB | 滤波、去耦 |

### 3.2 有效位数 (ENOB) 计算

**定义 3.1 (有效位数)**

```
ENOB = (SINAD - 1.76) / 6.02

其中:
  SINAD = Signal to Noise and Distortion Ratio (dB)

SINAD计算:
  SINAD = 10 × log10(P_signal / (P_noise + P_distortion))

总谐波失真 (THD):
  THD = sqrt(P_2^2 + P_3^2 + ... + P_N^2) / P_1

无杂散动态范围 (SFDR):
  SFDR = P_signal / P_max_spur

理想N-bit ADC:
  SINAD_ideal = 6.02 × N + 1.76 dB
  SNR_ideal = 6.02 × N + 1.76 dB

实际ADC:
  SINAD_actual < SINAD_ideal
  ENOB < N

示例 (12-bit ADC):
  理想SINAD = 74 dB
  实测SINAD = 66 dB
  ENOB = (66 - 1.76) / 6.02 ≈ 10.67 bits

  实际精度损失: 12 - 10.67 = 1.33 bits
```

### 3.3 校准算法实现

```c
/* ADC校准形式化实现 */

typedef struct {
    float offset;       /* 失调误差 (LSB) */
    float gain;         /* 增益误差系数 */
    int32_t ref_low;    /* 参考低点校准值 */
    int32_t ref_high;   /* 参考高点校准值 */
} ADCCalibration;

/* 两点校准 */
void adc_calibrate_two_point(ADC_TypeDef *ADCx,
                              ADCCalibration *cal,
                              float v_ref_low,    /* 如 0.1V */
                              float v_ref_high)   /* 如 3.2V */
{
    /* 采集参考点 */
    cal->ref_low = adc_read_average(ADCx, 1000);

    /* 切换到高参考电压 */
    // 外部电路切换或内部参考
    cal->ref_high = adc_read_average(ADCx, 1000);

    /* 计算校准参数 */
    float code_span = cal->ref_high - cal->ref_low;
    float volt_span = v_ref_high - v_ref_low;

    cal->gain = code_span / volt_span;
    cal->offset = cal->ref_low - (v_ref_low * cal->gain);
}

/* 应用校准 */
float adc_apply_calibration(ADCCalibration *cal, int32_t raw_code) {
    return (raw_code - cal->offset) / cal->gain;
}

/* 温度影响补偿 */
float adc_compensate_temperature(ADCCalibration *cal,
                                  float raw_value,
                                  float temp_current,
                                  float temp_cal)
{
    /* 温度系数 (典型值) */
    const float TEMP_COEFF_GAIN = 0.001;   /* 0.1%/°C */
    const float TEMP_COEFF_OFFSET = 0.02;  /* LSB/°C */

    float delta_temp = temp_current - temp_cal;

    float gain_corr = 1.0 + (TEMP_COEFF_GAIN * delta_temp);
    float offset_corr = TEMP_COEFF_OFFSET * delta_temp;

    return (raw_value - offset_corr) / gain_corr;
}

/* 校准流程状态机 */
typedef enum {
    CAL_STATE_IDLE = 0,
    CAL_STATE_INIT,
    CAL_STATE_MEASURE_ZERO,
    CAL_STATE_MEASURE_FULLSCALE,
    CAL_STATE_CALCULATE,
    CAL_STATE_VERIFY,
    CAL_STATE_COMPLETE,
    CAL_STATE_ERROR
} CalibrationState;

CalibrationState adc_calibration_fsm(ADC_TypeDef *ADCx,
                                      CalibrationState current,
                                      ADCCalibration *cal)
{
    switch (current) {
        case CAL_STATE_INIT:
            /* 初始化ADC，配置参考源 */
            return CAL_STATE_MEASURE_ZERO;

        case CAL_STATE_MEASURE_ZERO:
            /* 采集零点 */
            cal->ref_low = adc_read_average(ADCx, 1000);
            return CAL_STATE_MEASURE_FULLSCALE;

        case CAL_STATE_MEASURE_FULLSCALE:
            /* 采集满量程 */
            cal->ref_high = adc_read_average(ADCx, 1000);
            return CAL_STATE_CALCULATE;

        case CAL_STATE_CALCULATE:
            /* 计算校准系数 */
            cal->gain = (cal->ref_high - cal->ref_low) / 4095.0f;
            cal->offset = cal->ref_low;
            return CAL_STATE_VERIFY;

        case CAL_STATE_VERIFY:
            /* 验证校准 */
            // 检查线性度
            if (verify_linearity(ADCx, cal)) {
                return CAL_STATE_COMPLETE;
            } else {
                return CAL_STATE_ERROR;
            }

        default:
            return current;
    }
}
```

---

## 4. 高级ADC技术

### 4.1 过采样与抽取

**定义 4.1 (过采样)**

```
过采样: 以远高于奈奎斯特率的频率采样

过采样比 (OSR):
  OSR = f_sample / (2 × f_signal)

噪声整形收益:
  每增加4倍过采样，分辨率增加1 bit

  N_effective = N_original + 0.5 × log2(OSR)

示例:
  12-bit ADC, OSR = 16
  N_effective = 12 + 0.5 × log2(16) = 12 + 2 = 14 bits

硬件过采样 (STM32):
  自动对2-256个样本求平均
  结果右移得到更高分辨率

  过采样配置:
    OVFS = 2^n  (n=0..8)
    右移位数 = n
    结果 = (sum of OVFS samples) >> n
```

### 4.2 DMA与ADC连续采集

```c
/* DMA+ADC连续采集实现 */

#define ADC_BUFFER_SIZE 1024
#define ADC_NUM_CHANNELS 4

typedef struct {
    uint16_t buffer[2][ADC_BUFFER_SIZE];  /* 双缓冲 */
    volatile uint8_t active_buffer;
    volatile uint8_t ready[2];
    DMA_HandleTypeDef *hdma;
    ADC_HandleTypeDef *hadc;
} ADCContinuousCapture;

/* 初始化连续DMA采集 */
void adc_continuous_init(ADCContinuousCapture *cap,
                          ADC_HandleTypeDef *hadc,
                          DMA_HandleTypeDef *hdma)
{
    cap->hadc = hadc;
    cap->hdma = hdma;
    cap->active_buffer = 0;
    cap->ready[0] = 0;
    cap->ready[1] = 0;

    /* 配置ADC为连续模式 */
    hadc->Init.ContinuousConvMode = ENABLE;
    hadc->Init.DMAContinuousRequests = ENABLE;

    /* 配置多通道扫描 */
    hadc->Init.ScanConvMode = ENABLE;
    hadc->Init.NbrOfConversion = ADC_NUM_CHANNELS;

    /* 配置DMA双缓冲模式 */
    hdma->Init.Mode = DMA_CIRCULAR;
    HAL_DMAEx_MultiBufferStart(hdma,
                               (uint32_t)&hadc->Instance->DR,
                               (uint32_t)cap->buffer[0],
                               (uint32_t)cap->buffer[1],
                               ADC_BUFFER_SIZE);

    /* 启动ADC */
    HAL_ADC_Start_DMA(hadc, (uint32_t *)cap->buffer[0], ADC_BUFFER_SIZE);
}

/* DMA半传输中断 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
    /* 前半缓冲区已满 */
    adc_capture.ready[0] = 1;
}

/* DMA传输完成中断 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    /* 后半缓冲区已满 */
    adc_capture.ready[1] = 1;
}

/* 数据处理 */
void adc_process_data(ADCContinuousCapture *cap) {
    for (int buf = 0; buf < 2; buf++) {
        if (cap->ready[buf]) {
            /* 分离各通道数据 */
            uint16_t ch_data[ADC_NUM_CHANNELS][ADC_BUFFER_SIZE/ADC_NUM_CHANNELS];

            for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
                int channel = i % ADC_NUM_CHANNELS;
                int sample = i / ADC_NUM_CHANNELS;
                ch_data[channel][sample] = cap->buffer[buf][i];
            }

            /* 处理各通道 */
            for (int ch = 0; ch < ADC_NUM_CHANNELS; ch++) {
                process_channel_data(ch, ch_data[ch],
                                     ADC_BUFFER_SIZE/ADC_NUM_CHANNELS);
            }

            cap->ready[buf] = 0;
        }
    }
}

/* 性能计算:
 *
 * 假设: 4通道, 每个缓冲区1024样本, 12-bit ADC @ 2 MSPS
 *
 * 采集时间 = 1024 / (2MHz / 4) = 2.048 ms
 * 处理时间必须 < 2.048 ms
 *
 * 采样率每通道 = 2MHz / 4 = 500 kHz
 * 奈奎斯特频率 = 250 kHz (每通道)
 */
```

### 4.3 模拟看门狗

```c
/* ADC模拟看门狗配置 */

typedef struct {
    uint16_t low_threshold;     /* 下限阈值 */
    uint16_t high_threshold;    /* 上限阈值 */
    uint8_t channel_mask;       /* 监控通道位掩码 */
    void (*callback)(uint8_t channel, uint16_t value, uint8_t high_violation);
} ADCAnalogWatchdog;

/* 初始化模拟看门狗 */
void adc_watchdog_init(ADC_HandleTypeDef *hadc,
                        ADCAnalogWatchdog *wdg)
{
    ADC_AnalogWDGConfTypeDef wdg_config = {0};

    wdg_config.WatchdogMode = ADC_ANALOGWATCHDOG_ALL;
    wdg_config.LowThreshold = wdg->low_threshold;
    wdg_config.HighThreshold = wdg->high_threshold;
    wdg_config.ITMode = ENABLE;

    HAL_ADC_AnalogWDGConfig(hadc, &wdg_config);
}

/* 模拟看门狗中断 */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc) {
    uint16_t value = HAL_ADC_GetValue(hadc);
    uint8_t channel = hadc->Instance->SQR1 & ADC_SQR1_SQ1;  /* 提取通道 */

    uint8_t high_violation = (value > wdg_config.HighThreshold);

    if (wdg.callback) {
        wdg.callback(channel, value, high_violation);
    }
}
```

---

## 5. 参考资源

- **STM32 Reference Manual**: ADC章节
- **Analog Devices MT-001**: ADC架构指南
- **IEEE 1241**: ADC术语和测试方法标准
- **TI SLAA013**: 理解数据转换器应用报告
