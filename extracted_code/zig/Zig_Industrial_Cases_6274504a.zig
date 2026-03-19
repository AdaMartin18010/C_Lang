//
// Auto-generated from: Zig\Zig_Industrial_Cases.md
// Line: 314
// Language: zig
// Block ID: 6274504a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 飞行控制律 (Flight Control Laws)

const std = @import("std");

// 飞行器状态
const AircraftState = struct {
    // 姿态
    roll: f32,    // 滚转角 (度)
    pitch: f32,   // 俯仰角 (度)
    yaw: f32,     // 偏航角 (度)

    // 角速率
    p: f32,       // 滚转角速率 (度/秒)
    q: f32,       // 俯仰角速率 (度/秒)
    r: f32,       // 偏航角速率 (度/秒)

    // 空速
    tas: f32,     // 真空速 (节)

    // 高度
    altitude: f32, // 英尺
};

// 飞行员输入
const PilotInput = struct {
    roll_stick: f32,   // -1 到 1
    pitch_stick: f32,
    yaw_pedal: f32,
};

// 控制面输出
const ControlSurfaces = struct {
    aileron: f32,      // -1 到 1
    elevator: f32,
    rudder: f32,
};

// PID 控制器
fn PIDController(comptime Kp: f32, comptime Ki: f32, comptime Kd: f32) type {
    return struct {
        integral: f32 = 0,
        prev_error: f32 = 0,

        pub fn update(self: *@This(), setpoint: f32, measured: f32, dt: f32) f32 {
            const error = setpoint - measured;

            // 比例项
            const p_term = Kp * error;

            // 积分项
            self.integral += error * dt;
            const i_term = Ki * self.integral;

            // 微分项
            const derivative = (error - self.prev_error) / dt;
            const d_term = Kd * derivative;

            self.prev_error = error;

            return p_term + i_term + d_term;
        }
    };
}

// 飞行控制计算机
pub const FlightControlComputer = struct {
    // 三轴 PID 控制器
    roll_pid: PIDController(2.0, 0.1, 0.5),
    pitch_pid: PIDController(1.5, 0.05, 0.3),
    yaw_pid: PIDController(1.0, 0.02, 0.2),

    pub fn computeControl(
        self: *FlightControlComputer,
        state: AircraftState,
        input: PilotInput,
        dt: f32,
    ) ControlSurfaces {
        // 期望角速率 (由杆量决定)
        const p_desired = input.roll_stick * 30.0;   // 最大 30 deg/s
        const q_desired = input.pitch_stick * 15.0;
        const r_desired = input.yaw_pedal * 10.0;

        // PID 控制
        const roll_cmd = self.roll_pid.update(p_desired, state.p, dt);
        const pitch_cmd = self.pitch_pid.update(q_desired, state.q, dt);
        const yaw_cmd = self.yaw_pid.update(r_desired, state.r, dt);

        // 限幅
        return .{
            .aileron = std.math.clamp(roll_cmd, -1.0, 1.0),
            .elevator = std.math.clamp(pitch_cmd, -1.0, 1.0),
            .rudder = std.math.clamp(yaw_cmd, -1.0, 1.0),
        };
    }
};

// 冗余管理 (三重模块化冗余 - TMR)
pub const RedundantSensor = struct {
    const NUM_CHANNELS = 3;
    values: [NUM_CHANNELS]f32,

    pub fn read(self: *const RedundantSensor) ?f32 {
        // 中值选择
        var sorted = self.values;
        std.mem.sort(f32, &sorted, {}, std.sort.asc(f32));

        const median = sorted[1];

        // 一致性检查
        var valid_count: u8 = 0;
        for (self.values) |v| {
            if (@abs(v - median) < 5.0) {  // 5% 容差
                valid_count += 1;
            }
        }

        if (valid_count >= 2) {
            return median;
        }
        return null;  // 传感器失效
    }
};
