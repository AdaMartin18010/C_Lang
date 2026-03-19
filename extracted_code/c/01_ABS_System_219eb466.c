/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2339
 * Language: c
 * Block ID: 219eb466
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * MODULE:    ABS_Integration
 * FILE:      abs_integration.c
 * DESCRIPTION: ABS与EBD/TCS/ESP系统的集成接口
 *============================================================================*/

/*============================================================================
 * EBD (Electronic Brakeforce Distribution) - 电子制动力分配
 *============================================================================*/

typedef struct {
    float rear_slip_threshold;      /* 后轮滑移率阈值 */
    float ideal_brake_force_front;  /* 理想前轮制动力 */
    float ideal_brake_force_rear;   /* 理想后轮制动力 */
    bool ebd_active;                /* EBD激活状态 */
} EBD_ControlType;

/* EBD工作原理：
 * 在常规制动时（非ABS状态），根据车辆载荷和减速度，
 * 自动调节前后轮制动力分配，防止后轮先抱死
 */
void EBD_Process_10ms(float vehicle_decel, float rear_slip, float pressure_front,
                       float pressure_rear)
{
    static EBD_ControlType ebd;

    /* 计算理想制动力分配 */
    /* 基于车辆质心位置和减速度计算 */
    float load_transfer = (vehicle_decel / 9.81f) * VEHICLE_CG_HEIGHT / WHEELBASE;
    float front_ratio = FRONT_WEIGHT_RATIO + load_transfer;

    ebd.ideal_brake_force_front = total_brake_force * front_ratio;
    ebd.ideal_brake_force_rear = total_brake_force * (1.0f - front_ratio);

    /* 检查后轮滑移率 */
    if (rear_slip > ebd.rear_slip_threshold && vehicle_decel < -2.0f) {
        /* 后轮有抱死趋势，减小后轮压力 */
        ebd.ebd_active = true;
        ReduceRearBrakePressure(pressure_rear * 0.1f);
    } else {
        ebd.ebd_active = false;
    }
}

/*============================================================================
 * TCS (Traction Control System) - 牵引力控制系统
 *============================================================================*/

typedef enum {
    TCS_INACTIVE = 0,
    TCS_BRAKE_CONTROL,              /* 制动控制 */
    TCS_ENGINE_CONTROL              /* 发动机扭矩控制 */
} TCS_StateType;

typedef struct {
    TCS_StateType state;
    float slip_threshold;           /* 驱动滑移率阈值 */
    float target_slip;              /* 目标滑移率 */
    uint8_t active_wheel;           /* 激活控制的车轮 */
} TCS_ControlType;

/* TCS与ABS的关系：
 * - ABS处理制动时的滑移（λ > 0）
 * - TCS处理驱动时的滑移（λ < 0，驱动轮滑转）
 * - 共享轮速传感器和液压单元
 * - TCS可使用ABS的减压阀来降低驱动轮压力
 */
void TCS_Process_10ms(const float *wheel_speeds, float vehicle_speed)
{
    static TCS_ControlType tcs;
    uint8_t i;
    float drive_slip[4];
    float max_slip = 0.0f;

    /* 计算驱动滑移率 */
    for (i = 0u; i < 4u; i++) {
        if (wheel_speeds[i] > vehicle_speed) {
            drive_slip[i] = (wheel_speeds[i] - vehicle_speed) / wheel_speeds[i];
        } else {
            drive_slip[i] = 0.0f;
        }

        if (drive_slip[i] > max_slip) {
            max_slip = drive_slip[i];
            tcs.active_wheel = i;
        }
    }

    /* 判断TCS激活条件 */
    if (max_slip > tcs.slip_threshold && vehicle_speed > 10.0f) {
        tcs.state = TCS_BRAKE_CONTROL;

        /* 对滑转车轮施加制动 */
        ApplyBrakeForTCS(tcs.active_wheel,
                         CalculateTCSBrakePressure(max_slip, tcs.target_slip));

        /* 同时请求发动机降扭 */
        RequestEngineTorqueReduction(max_slip * 100.0f);  /* 0-100% */
    } else {
        tcs.state = TCS_INACTIVE;
        ReleaseTCSBrakePressure();
    }
}

/*============================================================================
 * ESP (Electronic Stability Program) - 电子稳定程序
 *============================================================================*/

typedef struct {
    float yaw_rate_ref;             /* 参考横摆角速度 */
    float yaw_rate_actual;          /* 实际横摆角速度 */
    float sideslip_angle;           /* 质心侧偏角 */
    float sideslip_rate;            /* 侧偏角速度 */
} ESP_VehicleStateType;

typedef enum {
    ESP_MODE_NORMAL = 0,
    ESP_MODE_UNDERSTEER,            /* 转向不足 */
    ESP_MODE_OVERSTEER              /* 转向过度 */
} ESP_ControlModeType;

/* ESP与ABS的协调：
 * 1. ESP优先级高于ABS（稳定性优先于制动距离）
 * 2. ESP可主动请求ABS执行单轮制动
 * 3. 两者共享液压单元，需要仲裁机制
 */

/* 制动请求仲裁 */
typedef struct {
    float pressure_request[4];      /* 各轮压力请求 */
    uint8_t request_source[4];      /* 请求来源: 0=ABS, 1=ESP, 2=TCS, 3=EBD */
    uint8_t priority[4];            /* 请求优先级 */
} BrakeRequestArbitratorType;

void BrakeRequest_Arbitrate(BrakeRequestArbitratorType *arb,
                            float *final_pressure)
{
    uint8_t i;

    for (i = 0u; i < 4u; i++) {
        /* 选择最高优先级的请求 */
        uint8_t max_priority = 0u;
        uint8_t selected_source = 0u;
        uint8_t j;

        for (j = 0u; j < 4u; j++) {
            if (arb->priority[j] > max_priority) {
                max_priority = arb->priority[j];
                selected_source = j;
            }
        }

        final_pressure[i] = arb->pressure_request[selected_source];
    }
}

/* 优先级定义 */
#define PRIORITY_ESP        3   /* ESP最高优先级 */
#define PRIORITY_TCS        2   /* TCS次之 */
#define PRIORITY_ABS        1   /* ABS基础 */
#define PRIORITY_EBD        0   /* EBD最低 */

/*============================================================================
 * 系统协调主函数
 *============================================================================*/
void IntegratedChassisControl_10ms(void)
{
    float wheel_speeds[4];
    float vehicle_speed;
    float vehicle_decel;
    float slip_ratios[4];
    BrakeRequestArbitratorType arb;
    float final_pressure[4];
    uint8_t i;

    /* 读取传感器 */
    WSP_GetAllSpeeds(wheel_speeds);
    vehicle_speed = SR_GetVehicleSpeed();
    vehicle_decel = SR_GetVehicleAccel();

    for (i = 0u; i < 4u; i++) {
        slip_ratios[i] = SR_GetSlipRatio(i);
    }

    /* 初始化仲裁器 */
    memset(&arb, 0, sizeof(arb));

    /* ===== ABS控制 ===== */
    for (i = 0u; i < 4u; i++) {
        ABS_LogicControl_10ms(i, slip_ratios[i],
                              WSP_GetAcceleration(i));

        ABS_ValveOutputsType valve_out;
        ABS_GetValveOutputs(i, &valve_out);

        /* 转换阀状态为压力请求 */
        arb.pressure_request[i] = ValveStateToPressure(&valve_out);
        arb.request_source[i] = 0u;  /* ABS */
        arb.priority[i] = PRIORITY_ABS;
    }

    /* ===== EBD控制 ===== */
    if (!ABS_IsAnyChannelActive()) {
        EBD_Process_10ms(vehicle_decel,
                         (slip_ratios[2] + slip_ratios[3]) / 2.0f,
                         GetFrontPressure(), GetRearPressure());

        if (EBD_IsActive()) {
            /* EBD请求调节后轮压力 */
            arb.pressure_request[2] = GetEBDRequestPressure(2);
            arb.pressure_request[3] = GetEBDRequestPressure(3);
            arb.priority[2] = PRIORITY_EBD;
            arb.priority[3] = PRIORITY_EBD;
        }
    }

    /* ===== TCS控制 ===== */
    if (vehicle_speed > 10.0f) {
        TCS_Process_10ms(wheel_speeds, vehicle_speed);

        if (TCS_IsActive()) {
            uint8_t tcs_wheel = TCS_GetActiveWheel();
            arb.pressure_request[tcs_wheel] = TCS_GetBrakePressure();
            arb.priority[tcs_wheel] = PRIORITY_TCS;
        }
    }

    /* ===== ESP控制 ===== */
    ESP_Process_10ms();

    if (ESP_IsActive()) {
        uint8_t esp_wheel = ESP_GetTargetWheel();
        arb.pressure_request[esp_wheel] = ESP_GetBrakePressure();
        arb.priority[esp_wheel] = PRIORITY_ESP;
    }

    /* 仲裁并输出 */
    BrakeRequest_Arbitrate(&arb, final_pressure);
    ApplyWheelPressures(final_pressure);
}
