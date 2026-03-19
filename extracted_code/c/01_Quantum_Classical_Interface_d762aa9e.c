/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Classical_Interface.md
 * Line: 607
 * Language: c
 * Block ID: d762aa9e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 量子测量结果处理
// IQ解调、阈值判定、纠错
// ============================================================================

#define MAX_MEASUREMENTS    8192
#define ADC_BITS            12
#define ADC_RANGE           (1 << ADC_BITS)

// 原始ADC样本
typedef struct {
    int16_t i_samples[MAX_MEASUREMENTS];
    int16_t q_samples[MAX_MEASUREMENTS];
    uint32_t num_samples;
} RawMeasurement;

// 积分结果
typedef struct {
    double i_integral;
    double q_integral;
    double magnitude;
    double phase;
} IQResult;

// 分类结果
typedef enum {
    STATE_0 = 0,
    STATE_1 = 1,
    STATE_INVALID = -1
} QubitState;

// ============================================================================
// IQ解调与积分
// ============================================================================

void demodulate_iq(const RawMeasurement *raw,
                   double if_frequency_mhz,
                   double sample_rate_msps,
                   IQResult *result) {

    double sum_i = 0.0;
    double sum_q = 0.0;
    double dt = 1.0 / sample_rate_msps;

    for (uint32_t n = 0; n < raw->num_samples; n++) {
        double t = n * dt;
        double phase = 2.0 * M_PI * if_frequency_mhz * t;

        // 数字混频
        double i_demod = raw->i_samples[n] * cos(phase) -
                         raw->q_samples[n] * sin(phase);
        double q_demod = raw->i_samples[n] * sin(phase) +
                         raw->q_samples[n] * cos(phase);

        sum_i += i_demod;
        sum_q += q_demod;
    }

    result->i_integral = sum_i;
    result->q_integral = sum_q;
    result->magnitude = sqrt(sum_i * sum_i + sum_q * sum_q);
    result->phase = atan2(sum_q, sum_i);
}

// ============================================================================
// 阈值判定 (单阈值)
// ============================================================================

QubitState classify_single_threshold(const IQResult *result,
                                      double threshold,
                                      bool use_magnitude) {
    double value = use_magnitude ? result->magnitude : result->i_integral;

    if (value < threshold) {
        return STATE_0;
    } else {
        return STATE_1;
    }
}

// ============================================================================
// 多阈值/多态分类 (区分 |0>, |1>, |2>等)
// ============================================================================

#define NUM_THRESHOLD_LEVELS    4

typedef struct {
    double thresholds[NUM_THRESHOLD_LEVELS - 1];
    double centers[NUM_THRESHOLD_LEVELS];
} MultiThreshold;

int classify_multi_level(const IQResult *result, const MultiThreshold *mt) {
    double value = result->magnitude;

    for (int i = 0; i < NUM_THRESHOLD_LEVELS - 1; i++) {
        if (value < mt->thresholds[i]) {
            return i;
        }
    }
    return NUM_THRESHOLD_LEVELS - 1;
}

// ============================================================================
// 多 shot 统计
// ============================================================================

typedef struct {
    uint32_t total_shots;
    uint32_t count_0;
    uint32_t count_1;
    double prob_0;
    double prob_1;
    double fidelity;        // 测量保真度估计
} MeasurementStats;

void compute_measurement_stats(const QubitState *results,
                                uint32_t num_shots,
                                MeasurementStats *stats) {
    stats->total_shots = num_shots;
    stats->count_0 = 0;
    stats->count_1 = 0;

    for (uint32_t i = 0; i < num_shots; i++) {
        if (results[i] == STATE_0) {
            stats->count_0++;
        } else if (results[i] == STATE_1) {
            stats->count_1++;
        }
    }

    stats->prob_0 = (double)stats->count_0 / num_shots;
    stats->prob_1 = (double)stats->count_1 / num_shots;

    // 简单保真度估计 (假设理想态为|0>或|1>)
    stats->fidelity = fmax(stats->prob_0, stats->prob_1);
}

// ============================================================================
// 测量校准
// ============================================================================

// 校准数据
typedef struct {
    IQResult centroid_0;    // |0>态中心
    IQResult centroid_1;    // |1>态中心
    double separation;      // 分离度
    double snr;             // 信噪比
} MeasurementCalibration;

void calibrate_measurement(MeasurementCalibration *cal,
                           const IQResult *prepare_0_results,
                           const IQResult *prepare_1_results,
                           uint32_t num_shots) {
    // 计算质心
    cal->centroid_0.i_integral = 0;
    cal->centroid_0.q_integral = 0;
    cal->centroid_1.i_integral = 0;
    cal->centroid_1.q_integral = 0;

    for (uint32_t i = 0; i < num_shots; i++) {
        cal->centroid_0.i_integral += prepare_0_results[i].i_integral;
        cal->centroid_0.q_integral += prepare_0_results[i].q_integral;
        cal->centroid_1.i_integral += prepare_1_results[i].i_integral;
        cal->centroid_1.q_integral += prepare_1_results[i].q_integral;
    }

    cal->centroid_0.i_integral /= num_shots;
    cal->centroid_0.q_integral /= num_shots;
    cal->centroid_1.i_integral /= num_shots;
    cal->centroid_1.q_integral /= num_shots;

    // 计算分离度
    double di = cal->centroid_1.i_integral - cal->centroid_0.i_integral;
    double dq = cal->centroid_1.q_integral - cal->centroid_0.q_integral;
    cal->separation = sqrt(di * di + dq * dq);

    // 计算SNR (简化)
    cal->snr = cal->separation / 10.0;  // 假设噪声标准差为10
}

QubitState classify_with_calibration(const IQResult *result,
                                      const MeasurementCalibration *cal) {
    double d0_i = result->i_integral - cal->centroid_0.i_integral;
    double d0_q = result->q_integral - cal->centroid_0.q_integral;
    double d0 = sqrt(d0_i * d0_i + d0_q * d0_q);

    double d1_i = result->i_integral - cal->centroid_1.i_integral;
    double d1_q = result->q_integral - cal->centroid_1.q_integral;
    double d1 = sqrt(d1_i * d1_i + d1_q * d1_q);

    return (d0 < d1) ? STATE_0 : STATE_1;
}
