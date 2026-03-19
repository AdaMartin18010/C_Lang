//
// Auto-generated from: Zig\Zig_Industrial_Cases.md
// Line: 870
// Language: zig
// Block ID: 3e1d31f3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 医疗设备固件示例: 输液泵

const std = @import("std");

// 安全关键: 必须符合 IEC 62304

pub const InfusionPump = struct {
    // 状态机
    const State = enum {
        stopped,
        priming,
        infusing,
        paused,
        alarm,
    };

    state: State = .stopped,

    // 参数
    target_rate_ml_h: f32 = 0,      // 目标流速
    vtbi_ml: f32 = 0,                // 待输注体积
    infused_volume_ml: f32 = 0,      // 已输注体积

    // 硬件接口
    motor: StepperMotor,
    flow_sensor: FlowSensor,
    bubble_detector: BubbleDetector,
    occlusion_sensor: OcclusionSensor,

    // 安全监控
    safety_monitor: SafetyMonitor,

    pub fn startInfusion(self: *InfusionPump, rate: f32, volume: f32) Error!void {
        // 参数验证
        if (rate <= 0 or rate > 1000) return Error.InvalidRate;
        if (volume <= 0 or volume > 9999) return Error.InvalidVolume;

        // 安全检查
        try self.safety_monitor.preInfusionCheck();

        self.target_rate_ml_h = rate;
        self.vtbi_ml = volume;
        self.state = .priming;

        // 启动电机
        self.motor.start();
    }

    pub fn stop(self: *InfusionPump) void {
        self.motor.stop();
        self.state = .stopped;
    }

    pub fn update(self: *InfusionPump, dt_ms: u32) void {
        // 安全检查 (最高优先级)
        if (self.bubble_detector.hasBubble()) {
            self.triggerAlarm(.air_in_line);
            return;
        }

        if (self.occlusion_sensor.isOccluded()) {
            self.triggerAlarm(.occlusion);
            return;
        }

        // 状态机更新
        switch (self.state) {
            .priming => {
                if (self.flow_sensor.isFlowing()) {
                    self.state = .infusing;
                }
            },
            .infusing => {
                // 计算输注量
                const flow_rate = self.flow_sensor.getRate();
                const dt_h = @as(f32, @floatFromInt(dt_ms)) / 3600000.0;
                self.infused_volume_ml += flow_rate * dt_h;

                // 检查完成
                if (self.infused_volume_ml >= self.vtbi_ml) {
                    self.stop();
                    return;
                }

                // 闭环控制流速
                const error = self.target_rate_ml_h - flow_rate;
                self.motor.adjustSpeed(error);
            },
            else => {},
        }
    }

    fn triggerAlarm(self: *InfusionPump, alarm_type: AlarmType) void {
        self.state = .alarm;
        self.motor.stop();
        self.safety_monitor.activateAlarm(alarm_type);
    }
};

const StepperMotor = struct {
    pub fn start(self: *StepperMotor) void { _ = self; }
    pub fn stop(self: *StepperMotor) void { _ = self; }
    pub fn adjustSpeed(self: *StepperMotor, error: f32) void { _ = self; _ = error; }
};

const FlowSensor = struct {
    pub fn isFlowing(self: *FlowSensor) bool { _ = self; return false; }
    pub fn getRate(self: *FlowSensor) f32 { _ = self; return 0; }
};

const BubbleDetector = struct {
    pub fn hasBubble(self: *BubbleDetector) bool { _ = self; return false; }
};

const OcclusionSensor = struct {
    pub fn isOccluded(self: *OcclusionSensor) bool { _ = self; return false; }
};

const SafetyMonitor = struct {
    pub fn preInfusionCheck(self: *SafetyMonitor) Error!void { _ = self; }
    pub fn activateAlarm(self: *SafetyMonitor, alarm_type: AlarmType) void {
        _ = self; _ = alarm_type;
    }
};

const AlarmType = enum {
    air_in_line,
    occlusion,
    door_open,
    low_battery,
};

const Error = error{
    InvalidRate,
    InvalidVolume,
    SafetyCheckFailed,
    HardwareFault,
};
