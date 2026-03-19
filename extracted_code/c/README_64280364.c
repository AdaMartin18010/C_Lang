/*
 * Auto-generated from: 04_Industrial_Scenarios\10_Deep_Sea\README.md
 * Line: 196
 * Language: c
 * Block ID: 64280364
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
