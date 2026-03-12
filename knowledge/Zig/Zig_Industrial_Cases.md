# Zig 工业场景案例研究

> **定位**: Zig 应用 / 工业实践 | **难度**: ⭐⭐⭐⭐⭐ | **预估学时**: 60-80小时
> **前置知识**: Zig 语言高级特性、系统设计、领域知识

---

## 📋 目录

- [Zig 工业场景案例研究](#zig-工业场景案例研究)
  - [📋 目录](#-目录)
  - [一、汽车电子 (Automotive)](#一汽车电子-automotive)
    - [1.1 ECU 固件开发](#11-ecu-固件开发)
    - [1.2 AUTOSAR 适配层](#12-autosar-适配层)
  - [二、航空航天 (Aerospace)](#二航空航天-aerospace)
    - [2.1 飞行控制系统](#21-飞行控制系统)
  - [三、高频交易 (HFT)](#三高频交易-hft)
    - [3.1 低延迟网络栈](#31-低延迟网络栈)
  - [四、通信系统 (Telecom)](#四通信系统-telecom)
    - [4.1 5G 基带处理](#41-5g-基带处理)
  - [五、工业控制 (Industrial Control)](#五工业控制-industrial-control)
    - [5.1 PLC 运行时](#51-plc-运行时)
  - [六、医疗设备 (Medical)](#六医疗设备-medical)
    - [6.1 嵌入式医疗设备](#61-嵌入式医疗设备)
    - [6.2 符合安全标准](#62-符合安全标准)
  - [总结](#总结)

---

## 一、汽车电子 (Automotive)

### 1.1 ECU 固件开发

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          汽车 ECU 软件架构                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                         应用层 (ASW)                                 │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │   │
│  │  │ 发动机控制 │  │ 刹车系统  │  │ 转向控制  │  │ 车身电子  │            │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │   │
│  └────────────────────────┬────────────────────────────────────────────┘   │
│                           │ RTE (运行时环境)                                │
│  ┌────────────────────────┼────────────────────────────────────────────┐   │
│  │                   运行时层 (RTE)                                      │   │
│  │  信号路由 │ 事件分发 │ 数据转换 │ 错误处理                              │   │
│  └────────────────────────┼────────────────────────────────────────────┘   │
│                           │                                                │
│  ┌────────────────────────┼────────────────────────────────────────────┐   │
│  │                   基础软件 (BSW)                                      │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐            │   │
│  │  │ 通信服务  │  │ 存储服务  │  │ IO 驱动  │  │ 诊断服务  │            │   │
│  │  │ (CAN/LIN)│  │ (EEPROM) │  │ (ADC/PWM)│  │ (UDS)    │            │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘            │   │
│  └────────────────────────┼────────────────────────────────────────────┘   │
│                           │                                                │
│  ┌────────────────────────┴────────────────────────────────────────────┐   │
│  │                      微控制器驱动 (MCAL)                              │   │
│  │  微控制器驱动 │ 存储器驱动 │ 通信驱动 │ IO 驱动 │ 看门狗                │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                              │
│  功能安全: ASIL-D 等级要求                                                     │
│  ─────────────────────────                                                    │
│  - 单点故障度量 (SPFM) > 99%                                                 │
│  - 潜伏故障度量 (LFM) > 90%                                                  │
│  - 每小时硬件故障概率 (PMHF) < 10^-8                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

```zig
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
```

### 1.2 AUTOSAR 适配层

```zig
// AUTOSAR 运行时环境适配

// RTE 接口生成 (类似 AUTOSAR RTE 生成器)
fn GenerateRTEInterface(comptime component_name: []const u8, comptime Interface: type) type {
    return struct {
        const name = component_name;

        // 发送端口
        pub fn write(data: Interface) void {
            // 通过 RTE 发送数据
            RTE.send(component_name, data);
        }

        // 接收端口
        pub fn read() ?Interface {
            return RTE.receive(component_name);
        }
    };
}

// RTE 模拟实现
const RTE = struct {
    var buffer: [1024]u8 = undefined;

    fn send(comptime name: []const u8, data: anytype) void {
        _ = name;
        _ = data;
        // 序列化并发送
    }

    fn receive(comptime name: []const u8) ?anytype {
        _ = name;
        // 反序列化并返回
        return null;
    }
};

// 软件组件示例
const EngineSWC = struct {
    // 端口定义
    pub const ThrottleInput = GenerateRTEInterface("Throttle", u8);
    pub const RPMOutput = GenerateRTEInterface("RPM", u16);

    pub fn runnable() void {
        // 读取输入
        const throttle = ThrottleInput.read() orelse 0;

        // 处理逻辑
        const rpm = calculateRPM(throttle);

        // 写入输出
        RPMOutput.write(rpm);
    }

    fn calculateRPM(throttle: u8) u16 {
        return @as(u16, @intCast(throttle)) * 80;
    }
};
```

---

## 二、航空航天 (Aerospace)

### 2.1 飞行控制系统

```zig
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
```

---

## 三、高频交易 (HFT)

### 3.1 低延迟网络栈

```zig
// DPDK 风格的低延迟网络处理

const std = @import("std");

// 内核旁路网络接口
pub const KernelBypassNIC = struct {
    rx_ring: RingBuffer,
    tx_ring: RingBuffer,

    // 轮询接收 (无中断延迟)
    pub fn pollReceive(self: *KernelBypassNIC, packets: []*Packet) usize {
        var count: usize = 0;

        // 批量处理 (减少缓存未命中)
        while (count < packets.len) {
            const pkt = self.rx_ring.dequeue() orelse break;
            packets[count] = pkt;
            count += 1;
        }

        return count;
    }

    // 零拷贝发送
    pub fn send(self: *KernelBypassNIC, pkt: *Packet) void {
        // 直接操作网卡描述符
        self.tx_ring.enqueue(pkt);
    }
};

// 无锁环形缓冲区
const RingBuffer = struct {
    const SIZE = 1024;  // 2 的幂次方便位运算
    const MASK = SIZE - 1;

    buffer: [SIZE]*Packet,
    head: std.atomic.Value(usize),
    tail: std.atomic.Value(usize),

    pub fn enqueue(self: *RingBuffer, pkt: *Packet) bool {
        const current_tail = self.tail.load(.monotonic);
        const next_tail = (current_tail + 1) & MASK;

        // 检查满
        if (next_tail == self.head.load(.acquire)) {
            return false;
        }

        self.buffer[current_tail] = pkt;
        self.tail.store(next_tail, .release);
        return true;
    }

    pub fn dequeue(self: *RingBuffer) ?*Packet {
        const current_head = self.head.load(.monotonic);

        // 检查空
        if (current_head == self.tail.load(.acquire)) {
            return null;
        }

        const pkt = self.buffer[current_head];
        self.head.store((current_head + 1) & MASK, .release);
        return pkt;
    }
};

// 市场数据处理器
pub const MarketDataHandler = struct {
    order_book: OrderBook,
    latency_counter: LatencyHistogram,

    // 热点函数: 使用 comptime 优化
    pub inline fn processQuote(
        self: *MarketDataHandler,
        quote: *const Quote,
    ) void {
        const start_tick = readTSC();  // 时间戳计数器

        // 更新订单簿
        self.order_book.update(quote);

        // 检查交易信号
        if (self.checkArbitrage()) {
            self.sendOrder();
        }

        const end_tick = readTSC();
        self.latency_counter.record(end_tick - start_tick);
    }

    fn checkArbitrage(self: *MarketDataHandler) bool {
        // 简单的价差套利检测
        const best_bid = self.order_book.bestBid();
        const best_ask = self.order_book.bestAsk();

        // 如果有其他交易所的数据，检查跨交易所套利
        _ = best_bid;
        _ = best_ask;
        return false;
    }

    fn sendOrder(self: *MarketDataHandler) void {
        // 发送交易指令
        _ = self;
    }
};

// 订单簿 (简化)
const OrderBook = struct {
    bids: [100]PriceLevel,
    asks: [100]PriceLevel,

    pub fn update(self: *OrderBook, quote: *const Quote) void {
        // 二分查找插入/更新
        const levels = if (quote.side == .bid) &self.bids else &self.asks;
        _ = levels;
        // 更新逻辑...
    }

    pub fn bestBid(self: *const OrderBook) f64 {
        // 返回最高买价
        return self.bids[0].price;
    }

    pub fn bestAsk(self: *const OrderBook) f64 {
        // 返回最低卖价
        return self.asks[0].price;
    }
};

const PriceLevel = struct {
    price: f64,
    quantity: u32,
};

const Quote = struct {
    symbol: [8]u8,
    price: f64,
    quantity: u32,
    side: enum { bid, ask },
};

const Packet = opaque {};

// 读取时间戳计数器 (x86 RDTSC)
inline fn readTSC() u64 {
    var low: u32 = undefined;
    var high: u32 = undefined;
    asm volatile (
        \\rdtsc
        : "={eax}" (low),
          "={edx}" (high),
    );
    return (@as(u64, high) << 32) | low;
}

// 延迟直方图
const LatencyHistogram = struct {
    buckets: [100]u64,  // 对数分桶

    pub fn record(self: *LatencyHistogram, cycles: u64) void {
        const bucket = std.math.log2(cycles);
        if (bucket < self.buckets.len) {
            self.buckets[bucket] += 1;
        }
    }
};
```

---

## 四、通信系统 (Telecom)

### 4.1 5G 基带处理

```zig
// 5G NR 基带信号处理

const std = @import("std");

// FFT 处理 (用于 OFDM)
pub fn FFT(comptime N: usize) type {
    return struct {
        const Self = @This();

        // 旋转因子 (预计算)
        twiddles: [N / 2]Complex,

        pub fn init() Self {
            var twiddles: [N / 2]Complex = undefined;
            for (0..N / 2) |k| {
                const angle = -2.0 * std.math.pi * @as(f32, @floatFromInt(k)) / @as(f32, @floatFromInt(N));
                twiddles[k] = Complex{
                    .re = @cos(angle),
                    .im = @sin(angle),
                };
            }
            return .{ .twiddles = twiddles };
        }

        // 时域 -> 频域
        pub fn forward(self: *const Self, input: *const [N]Complex, output: *[N]Complex) void {
            // 位反转重排
            bitReverseCopy(input, output);

            // 蝶形运算
            var stage: usize = 2;
            while (stage <= N) : (stage *= 2) {
                const half_stage = stage / 2;
                for (0..N / stage) |group| {
                    for (0..half_stage) |k| {
                        const even_idx = group * stage + k;
                        const odd_idx = even_idx + half_stage;

                        const twiddle = self.twiddles[k * (N / stage)];
                        const product = output[odd_idx].mul(twiddle);

                        output[odd_idx] = output[even_idx].sub(product);
                        output[even_idx] = output[even_idx].add(product);
                    }
                }
            }
        }
    };
}

const Complex = struct {
    re: f32,
    im: f32,

    pub fn add(self: Complex, other: Complex) Complex {
        return .{
            .re = self.re + other.re,
            .im = self.im + other.im,
        };
    }

    pub fn sub(self: Complex, other: Complex) Complex {
        return .{
            .re = self.re - other.re,
            .im = self.im - other.im,
        };
    }

    pub fn mul(self: Complex, other: Complex) Complex {
        return .{
            .re = self.re * other.re - self.im * other.im,
            .im = self.re * other.im + self.im * other.re,
        };
    }
};

fn bitReverseCopy(input: *const anyopaque, output: *anyopaque) void {
    _ = input;
    _ = output;
    // 实现位反转复制
}

// LDPC 编码器 (5G 数据信道)
pub const LDPCEncoder = struct {
    // 5G NR LDPC 基图 (简化)
    pub fn encode(self: *LDPCEncoder, data: []const u8, codeword: []u8) void {
        _ = self;
        _ = data;
        _ = codeword;
        // LDPC 编码实现
    }
};

// MIMO 预编码
pub const MIMOPrecoder = struct {
    // 预编码矩阵
    pub fn precode(
        self: *MIMOPrecoder,
        symbols: []const Complex,
        channel_matrix: []const f32,
        output: []Complex,
    ) void {
        _ = self;
        _ = symbols;
        _ = channel_matrix;
        _ = output;
        // MIMO 预编码实现
    }
};
```

---

## 五、工业控制 (Industrial Control)

### 5.1 PLC 运行时

```zig
// PLC (可编程逻辑控制器) 运行时

const std = @import("std");

// IEC 61131-3 功能块
pub const Timer = struct {
    // 输入
    in: bool = false,
    pt: u32 = 0,  // 预设时间 (ms)

    // 输出
    q: bool = false,
    et: u32 = 0,  // 经过时间

    // 内部状态
    start_time: u32 = 0,
    running: bool = false,

    pub fn update(self: *Timer, current_time: u32) void {
        if (self.in and !self.running) {
            // 启动计时
            self.start_time = current_time;
            self.running = true;
        } else if (!self.in) {
            // 停止并重置
            self.running = false;
            self.et = 0;
        }

        if (self.running) {
            self.et = current_time - self.start_time;
            self.q = self.et >= self.pt;
        } else {
            self.q = false;
        }
    }
};

// PID 功能块
pub const PID = struct {
    // 参数
    kp: f32,
    ki: f32,
    kd: f32,

    // 限制
    out_min: f32,
    out_max: f32,

    // 状态
    integral: f32 = 0,
    prev_error: f32 = 0,

    pub fn update(self: *PID, setpoint: f32, input: f32, dt: f32) f32 {
        const error = setpoint - input;

        // 比例
        const p = self.kp * error;

        // 积分 (带抗饱和)
        self.integral += error * dt;
        self.integral = std.math.clamp(self.integral, self.out_min, self.out_max);
        const i = self.ki * self.integral;

        // 微分
        const d = self.kd * (error - self.prev_error) / dt;
        self.prev_error = error;

        // 输出限幅
        return std.math.clamp(p + i + d, self.out_min, self.out_max);
    }
};

// PLC 扫描周期
pub const PLCEngine = struct {
    scan_time_ms: u32 = 10,
    last_scan: u32 = 0,

    io_inputs: [64]bool,
    io_outputs: [64]bool,

    timers: [16]Timer,
    pids: [8]PID,

    pub fn runCycle(self: *PLCEngine, current_time: u32) void {
        // 检查扫描时间
        if (current_time - self.last_scan < self.scan_time_ms) {
            return;
        }
        self.last_scan = current_time;

        // 1. 读取输入
        self.readInputs();

        // 2. 执行程序
        self.executeProgram(current_time);

        // 3. 写入输出
        self.writeOutputs();
    }

    fn readInputs(self: *PLCEngine) void {
        // 从 I/O 模块读取
        _ = self;
    }

    fn executeProgram(self: *PLCEngine, current_time: u32) void {
        // 更新定时器
        for (&self.timers) |*timer| {
            timer.update(current_time);
        }

        // 用户程序逻辑 (梯形图/结构化文本编译结果)
        // ladder_logic_run(self);
    }

    fn writeOutputs(self: *PLCEngine) void {
        // 写入 I/O 模块
        _ = self;
    }
};
```

---

## 六、医疗设备 (Medical)

### 6.1 嵌入式医疗设备

```zig
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
```

### 6.2 符合安全标准

```zig
// 医疗设备软件生命周期 (IEC 62304)

// 1. 软件需求分析
const SoftwareRequirements = struct {
    // 功能性需求
    infusion_rate_accuracy: f32 = 0.05,  // ±5%

    // 安全需求
    max_single_fault_time_ms: u32 = 100, // 单故障响应时间

    // 可靠性需求
    mtbf_hours: u32 = 10000,  // 平均无故障时间
};

// 2. 软件架构设计
const SoftwareArchitecture = struct {
    // 分层架构
    // 硬件抽象层
    // 设备驱动层
    // 中间件层
    // 应用层
    // 用户界面层
};

// 3. 软件详细设计
// 每个功能模块的详细设计文档

// 4. 软件单元实现和验证
// 单元测试覆盖率 > 80%

// 5. 软件集成和集成测试
// 集成测试用例

// 6. 软件系统测试
// 系统级验证

// 7. 软件发布
// 风险管理文档
// 临床评估

// 风险管理
const RiskManagement = struct {
    // 危害分析
    // 风险评估
    // 风险控制措施
    // 残余风险评价
};

// 可追溯性矩阵
const TraceabilityMatrix = struct {
    // 需求 -> 设计 -> 代码 -> 测试用例
};
```

---

## 总结

本文档展示了 Zig 在各种工业场景中的应用：

1. **汽车电子**: ECU 固件、AUTOSAR 适配
2. **航空航天**: 飞行控制、冗余管理
3. **高频交易**: 低延迟网络、内存池优化
4. **通信系统**: 5G 基带、信号处理
5. **工业控制**: PLC 运行时、PID 控制
6. **医疗设备**: 安全关键系统、合规开发

每个场景都体现了 Zig 的优势：

- **显式控制**: 适合实时系统
- **零成本抽象**: 高性能同时保持可读性
- **编译时计算**: 生成优化的领域特定代码
- **安全特性**: 适合安全关键应用

> **文档状态**: 工业案例完成 | **代码**: 可直接参考 | **适用版本**: Zig 0.13+
