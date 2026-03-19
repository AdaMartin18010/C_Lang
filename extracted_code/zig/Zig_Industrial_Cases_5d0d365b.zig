//
// Auto-generated from: Zig\Zig_Industrial_Cases.md
// Line: 101
// Language: zig
// Block ID: 5d0d365b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// ECU 固件示例: 发动机控制单元 (简化)

const std = @import("std");

// ECU 配置
const ECUConfig = struct {
    cylinder_count: u8 = 4,
    max_rpm: u16 = 8000,
    fuel_injector_count: u8 = 4,
    knock_sensor_enabled: bool = true,
};

const config: ECUConfig = .{};

// 传感器数据结构
const EngineSensors = struct {
    rpm: u16,           // 转速 (RPM)
    throttle: u8,       // 油门位置 (0-100%)
    coolant_temp: i16,  // 冷却液温度 (摄氏度)
    intake_air_temp: i16,
    manifold_pressure: u16,  // 进气歧管压力 (kPa)
    lambda_voltage: u16,     // 氧传感器电压
    knock_level: u8,         // 爆震强度
};

// 执行器状态
const ActuatorState = struct {
    fuel_injector_pulse_ms: f32,
    ignition_advance_deg: f32,
    idle_air_control: u8,
};

// 发动机控制算法
pub const EngineController = struct {
    sensors: EngineSensors,
    state: ActuatorState,

    // 查找表: 空燃比 (AFR) 目标值
    const afr_table = comptime blk: {
        var table: [16][16]f32 = undefined;
        // 负载 (x) vs 转速 (y)
        for (0..16) |load| {
            for (0..16) |rpm_idx| {
                const load_pct = @as(f32, @floatFromInt(load)) / 15.0 * 100.0;
                const rpm = @as(f32, @floatFromInt(rpm_idx)) / 15.0 * 8000.0;

                // 化学计量比: 14.7:1 (汽油)
                // 高负载:  richer (12-13:1)
                // 低负载:  leaner (15-16:1)
                const base_afr: f32 = 14.7;
                const load_correction = if (load_pct > 70) -2.0 else 0.5;
                const rpm_correction = if (rpm > 6000) -0.5 else 0.0;

                table[load][rpm_idx] = base_afr + load_correction + rpm_correction;
            }
        }
        break :blk table;
    };

    pub fn calculateFuelInjection(self: *EngineController) void {
        // 计算空气质量流量 (MAF)
        const maf = self.calculateMAF();

        // 查找目标空燃比
        const load_idx = @min(self.sensors.throttle / 7, 15);
        const rpm_idx = @min(self.sensors.rpm / 500, 15);
        const target_afr = afr_table[load_idx][rpm_idx];

        // 计算燃油质量
        const fuel_mass = maf / target_afr;

        // 转换为喷油器脉冲宽度 (简化计算)
        const injector_flow_rate: f32 = 2.5; // g/s per injector
        const pulse_width_ms = (fuel_mass / config.fuel_injector_count) /
                               injector_flow_rate * 1000.0;

        self.state.fuel_injector_pulse_ms = pulse_width_ms;
    }

    fn calculateMAF(self: *EngineController) f32 {
        // 速度密度法计算 MAF
        const ve: f32 = 0.85;  // 容积效率 (简化)
        const displacement: f32 = 2.0;  // 排量 (L)
        const rpm = @as(f32, @floatFromInt(self.sensors.rpm));
        const map = @as(f32, @floatFromInt(self.sensors.manifold_pressure));
        const temp_k = @as(f32, @floatFromInt(self.sensors.intake_air_temp)) + 273.15;

        // MAF = (VE * 排量 * 转速/120 * MAP) / (R * T)
        const r: f32 = 8.314;
        const maf = (ve * displacement * rpm / 120.0 * map) / (r * temp_k);

        return maf;
    }

    pub fn calculateIgnitionTiming(self: *EngineController) void {
        // 基础点火提前角
        var advance: f32 = 15.0;  // 度 BTDC (上止点前)

        // 基于转速的调整
        const rpm = self.sensors.rpm;
        if (rpm > 3000) {
            advance += 5.0;
        }
        if (rpm > 6000) {
            advance += 3.0;
        }

        // 爆震检测后的 retard
        if (self.sensors.knock_level > 50) {
            advance -= @as(f32, @floatFromInt(self.sensors.knock_level)) / 10.0;
        }

        self.state.ignition_advance_deg = advance;
    }
};

// CAN 总线通信
const CANFrame = struct {
    id: u11,
    data: [8]u8,
    dlc: u4,
};

pub const CANController = struct {
    pub fn sendEngineData(ecu: *const EngineController) void {
        // 发动机转速和负载
        const frame1 = CANFrame{
            .id = 0x100,
            .dlc = 4,
            .data = .{
                @truncate(ecu.sensors.rpm),
                @truncate(ecu.sensors.rpm >> 8),
                ecu.sensors.throttle,
                @truncate(@as(u16, @bitCast(ecu.sensors.coolant_temp))),
            },
        };
        _ = frame1;

        // 发送帧 (实际实现会使用 CAN 控制器寄存器)
    }
};
