# C到硬件的Pragma指令详解

## 目录

- [C到硬件的Pragma指令详解](#c到硬件的pragma指令详解)
  - [目录](#目录)
  - [1. Pragma概述](#1-pragma概述)
    - [1.1 Pragma的作用](#11-pragma的作用)
    - [1.2 Pragma分类速查](#12-pragma分类速查)
  - [2. INTERFACE Pragma详解](#2-interface-pragma详解)
    - [2.1 基本语法](#21-基本语法)
    - [2.2 标量接口模式](#22-标量接口模式)
    - [2.3 存储器接口模式](#23-存储器接口模式)
    - [2.4 AXI总线接口模式](#24-axi总线接口模式)
    - [2.5 控制接口模式](#25-控制接口模式)
  - [3. PIPELINE Pragma详解](#3-pipeline-pragma详解)
    - [3.1 基本语法与选项](#31-基本语法与选项)
    - [3.2 II（启动间隔）详解](#32-ii启动间隔详解)
    - [3.3 PIPELINE OFF - 禁用流水线](#33-pipeline-off---禁用流水线)
    - [3.4 REWIND选项 - 循环间流水线](#34-rewind选项---循环间流水线)
    - [3.5 流水线风格](#35-流水线风格)
  - [4. UNROLL Pragma详解](#4-unroll-pragma详解)
    - [4.1 基本语法与选项](#41-基本语法与选项)
    - [4.2 完全展开](#42-完全展开)
    - [4.3 部分展开](#43-部分展开)
    - [4.4 区域展开](#44-区域展开)
    - [4.5 UNROLL与其他指令组合](#45-unroll与其他指令组合)
  - [5. ARRAY\_PARTITION Pragma详解](#5-array_partition-pragma详解)
    - [5.1 基本语法与选项](#51-基本语法与选项)
    - [5.2 Complete分区](#52-complete分区)
    - [5.3 Block分区](#53-block分区)
    - [5.4 Cyclic分区](#54-cyclic分区)
    - [5.5 分区策略选择](#55-分区策略选择)
  - [6. DATAFLOW Pragma详解](#6-dataflow-pragma详解)
    - [6.1 基本语法与选项](#61-基本语法与选项)
    - [6.2 DATAFLOW基本原理](#62-dataflow基本原理)
    - [6.3 STREAM指令配置](#63-stream指令配置)
    - [6.4 DATAFLOW使用约束](#64-dataflow使用约束)
    - [6.5 复杂DATAFLOW设计](#65-复杂dataflow设计)
  - [7. 完整示例：综合运用](#7-完整示例综合运用)
    - [7.1 图像卷积加速器（综合示例）](#71-图像卷积加速器综合示例)
    - [7.2 优化效果对比](#72-优化效果对比)
  - [总结](#总结)

---

## 1. Pragma概述

### 1.1 Pragma的作用

```c
/*
 * Pragma是C/C++预处理器指令，用于向HLS工具传递编译指导信息。
 * 这些指令不会影响C代码的功能正确性，但会显著影响生成的硬件。
 */

// 基本语法
#pragma HLS <directive_name> [options]

// 示例
void example(int a[100], int b[100]) {
    #pragma HLS INTERFACE mode=ap_vld port=a
    #pragma HLS INTERFACE mode=ap_vld port=b
    #pragma HLS PIPELINE II=1

    for (int i = 0; i < 100; i++) {
        a[i] = b[i] * 2;
    }
}

/*
 * Pragma的作用域：
 *
 * 1. 文件级：影响整个源文件
 *    #pragma HLS interface mode=ap_ctrl_none port=return
 *
 * 2. 函数级：影响整个函数
 *    void func(...) {
 *        #pragma HLS PIPELINE
 *        ...
 *    }
 *
 * 3. 循环级：影响特定循环
 *    for (...) {
 *        #pragma HLS UNROLL
 *    }
 *
 * 4. 区域级：影响代码区域
 *    {
 *        #pragma HLS PIPELINE
 *        ...
 *    }
 */
```

### 1.2 Pragma分类速查

```
┌─────────────────────────────────────────────────────────────────────┐
│                    HLS Pragma分类速查表                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  类别          Pragma              功能                              │
│  ─────────────────────────────────────────────────────────────────  │
│                                                                      │
│  接口优化      INTERFACE           定义端口接口模式                  │
│               ARRAY_MAP            数组映射与合并                    │
│               ARRAY_RESHAPE        数组重塑                          │
│                                                                      │
│  并行优化      PIPELINE            循环流水线                        │
│               UNROLL               循环展开                          │
│               LOOP_FLATTEN         循环展平                          │
│               LOOP_MERGE           循环合并                          │
│                                                                      │
│  存储优化      ARRAY_PARTITION     数组分区                          │
│               BIND_STORAGE         存储器绑定                        │
│               RESOURCE             资源指定                          │
│                                                                      │
│  任务并行      DATAFLOW            数据流优化                        │
│               STREAM               流数据通道                        │
│                                                                      │
│  函数优化      INLINE              函数内联                          │
│               ALLOCATION           资源分配限制                      │
│               FUNCTION_INSTANTIATE 函数实例化                        │
│                                                                      │
│  时序约束      LATENCY             延迟约束                          │
│               LOOP_TRIPCOUNT       循环次数提示                      │
│                                                                      │
│  其他          DEPENDENCE          依赖声明                          │
│               OCCURRENCE           执行频率                          │
│               RESET                复位行为                          │
│               STABLE               稳定信号                          │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/

### 1.3 Pragma优先级规则

```c
/*
 * Pragma优先级（高到低）：
 *
 * 1. 最内层优先
 * 2. 后定义的覆盖先定义的
 * 3. 显式优于隐式
 */

void pragma_priority(int a[100]) {
    #pragma HLS PIPELINE II=2      // 外层：II=2

    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE II=1  // 内层：II=1（覆盖外层）

        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL     // 最内层：完全展开
            a[i*10+j] = process(a[i*10+j]);
        }
    }
}
```

---

## 2. INTERFACE Pragma详解

### 2.1 基本语法

```c
#pragma HLS INTERFACE \
    mode=<mode_type> \
    port=<port_name> \
    [bundle=<bundle_name>] \
    [register] \
    [depth=<fifo_depth>] \
    [offset=<slave|direct>] \
    [clock=<clock_name>]

// 模式类型（mode）：
// - ap_ctrl_none, ap_ctrl_hs, ap_ctrl_chain（控制接口）
// - ap_none, ap_vld, ap_ack, ap_hs（握手接口）
// - ap_memory, ap_fifo, ap_bus（存储器接口）
// - s_axilite, m_axi（总线接口）
// - axis（AXI Stream）
```

### 2.2 标量接口模式

```c
// ap_none - 简单线连接（默认）
void interface_none(
    int input_data,          // ap_none
    int *output_data         // ap_none
) {
    #pragma HLS INTERFACE mode=ap_none port=input_data
    #pragma HLS INTERFACE mode=ap_none port=output_data
    *output_data = input_data * 2;
}
/* 生成信号：
   input_data      : input[31:0]
   output_data     : output[31:0]
   ap_return       : output[31:0]  (如果有返回值)
*/

// ap_vld - 带有效信号（源同步）
void interface_vld(
    int input_data,          // ap_vld
    int *output_data         // ap_vld
) {
    #pragma HLS INTERFACE mode=ap_vld port=input_data
    #pragma HLS INTERFACE mode=ap_vld port=output_data
    *output_data = input_data + 1;
}
/* 生成信号：
   input_data      : input[31:0]
   input_data_ap_vld : input (有效指示)
   output_data     : output[31:0]
   output_data_ap_vld : output (有效指示)
*/

// ap_ack - 带应答信号（目的同步）
void interface_ack(
    int input_data,          // ap_ack
    int *output_data         // ap_ack
) {
    #pragma HLS INTERFACE mode=ap_ack port=input_data
    #pragma HLS INTERFACE mode=ap_ack port=output_data
    *output_data = input_data << 1;
}
/* 生成信号：
   input_data      : input[31:0]
   input_data_ap_ack : output (应答指示)
   output_data     : output[31:0]
   output_data_ap_ack : input (应答指示)
*/

// ap_hs - 完整握手（vld + ack）
void interface_hs(
    int input_data,          // ap_hs
    int *output_data         // ap_hs
) {
    #pragma HLS INTERFACE mode=ap_hs port=input_data
    #pragma HLS INTERFACE mode=ap_hs port=output_data
    *output_data = input_data ^ 0xFF;
}
/* 生成信号：
   input_data      : input[31:0]
   input_data_ap_vld : input
   input_data_ap_ack : output
   output_data     : output[31:0]
   output_data_ap_vld : output
   output_data_ap_ack : input
*/
```

### 2.3 存储器接口模式

```c
// ap_memory - 标准存储器接口
void interface_memory(
    int mem[1024],           // ap_memory
    int addr,
    int we,
    int wdata,
    int *rdata
) {
    #pragma HLS INTERFACE mode=ap_memory port=mem

    if (we) {
        mem[addr] = wdata;
    }
    *rdata = mem[addr];
}
/* 生成信号：
   mem_address0  : output[9:0]
   mem_ce0       : output
   mem_we0       : output
   mem_d0        : output[31:0]
   mem_q0        : input[31:0]
*/

// ap_fifo - FIFO队列接口
void interface_fifo(
    hls::stream<int> &fifo_in,   // ap_fifo
    hls::stream<int> &fifo_out   // ap_fifo
) {
    #pragma HLS INTERFACE mode=ap_fifo port=fifo_in
    #pragma HLS INTERFACE mode=ap_fifo port=fifo_out

    while (!fifo_in.empty()) {
        #pragma HLS PIPELINE
        int data = fifo_in.read();
        fifo_out.write(data * 2);
    }
}
/* 生成信号：
   fifo_in_dout  : input[31:0]
   fifo_in_empty_n : input
   fifo_in_read  : output
   fifo_out_din  : output[31:0]
   fifo_out_full_n : input
   fifo_out_write : output
*/

// ap_bus - 总线接口（legacy）
void interface_bus(
    int *mem,
    int base_addr,
    int n
) {
    #pragma HLS INTERFACE mode=ap_bus port=mem
    #pragma HLS INTERFACE mode=ap_vld port=base_addr
    #pragma HLS INTERFACE mode=ap_vld port=n

    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        mem[base_addr + i] = i;
    }
}
```

### 2.4 AXI总线接口模式

```c
// s_axilite - AXI4-Lite从接口（寄存器配置）
void interface_s_axilite(
    int *param1,             // s_axilite
    int *param2,             // s_axilite
    int data[1000]           // m_axi
) {
    #pragma HLS INTERFACE mode=s_axilite port=param1 bundle=CONTROL
    #pragma HLS INTERFACE mode=s_axilite port=param2 bundle=CONTROL
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=CONTROL
    #pragma HLS INTERFACE mode=m_axi port=data offset=slave bundle=GMEM

    int threshold = *param1;
    int scale = *param2;

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        if (data[i] > threshold) {
            data[i] = data[i] * scale;
        }
    }
}
/* AXI-Lite寄存器映射：
   0x00: ap_ctrl (start/done/idle/ready)
   0x04: GIE (全局中断使能)
   0x08: IER (中断使能)
   0x0C: ISR (中断状态)
   0x10: param1
   0x18: param2
   0x20: data (指针)
*/

// m_axi - AXI4主接口（存储器访问）
void interface_m_axi(
    int *src,                // m_axi
    int *dst,                // m_axi
    int n
) {
    #pragma HLS INTERFACE mode=m_axi port=src \
                offset=slave bundle=gmem0 \
                depth=1024 max_read_burst_length=16
    #pragma HLS INTERFACE mode=m_axi port=dst \
                offset=slave bundle=gmem1 \
                depth=1024 max_write_burst_length=16
    #pragma HLS INTERFACE mode=s_axilite port=n bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=control

    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        dst[i] = src[i] * 2;
    }
}
/* m_axi选项：
   - offset: slave/direct (地址偏移模式)
   - bundle: 总线捆绑名称
   - depth: 编译时数组深度提示
   - max_read_burst_length: 最大读突发长度
   - max_write_burst_length: 最大写突发长度
   - num_read_outstanding: 最大读事务数
   - num_write_outstanding: 最大写事务数
*/

// axis - AXI Stream接口
#include "ap_axi_sdata.h"

typedef ap_axiu<32, 2, 5, 6> axis_data_t;  // 32bit数据, 2bit user, 5bit id, 6bit dest

void interface_axis(
    hls::stream<axis_data_t> &in_stream,   // axis
    hls::stream<axis_data_t> &out_stream   // axis
) {
    #pragma HLS INTERFACE mode=axis port=in_stream
    #pragma HLS INTERFACE mode=axis port=out_stream
    #pragma HLS INTERFACE mode=ap_ctrl_none port=return

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        axis_data_t in_data = in_stream.read();

        axis_data_t out_data;
        out_data.data = in_data.data * 2;
        out_data.keep = in_data.keep;
        out_data.strb = in_data.strb;
        out_data.user = in_data.user;
        out_data.last = in_data.last;
        out_data.id = in_data.id;
        out_data.dest = in_data.dest;

        out_stream.write(out_data);
    }
}
```

### 2.5 控制接口模式

```c
// ap_ctrl_hs - 标准握手控制（默认）
void control_hs(int in[100], int out[100]) {
    #pragma HLS INTERFACE mode=ap_ctrl_hs port=return

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        out[i] = in[i] * 2;
    }
}
/* 控制信号：
   ap_start  : input  (启动)
   ap_done   : output (完成)
   ap_idle   : output (空闲)
   ap_ready  : output (就绪)
*/

// ap_ctrl_chain - 链式控制（支持连续启动）
void control_chain(int in[100], int out[100]) {
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        out[i] = in[i] * 2;
    }
}
/* 额外信号：
   ap_continue : input (继续)
   ap_done在ap_continue为高时才能拉高
   支持数据流式处理
*/

// ap_ctrl_none - 无控制接口（always run）
void control_none(
    hls::stream<int> &in_stream,
    hls::stream<int> &out_stream
) {
    #pragma HLS INTERFACE mode=ap_ctrl_none port=return
    #pragma HLS INTERFACE mode=axis port=in_stream
    #pragma HLS INTERFACE mode=axis port=out_stream

    while (1) {
        #pragma HLS PIPELINE
        if (!in_stream.empty()) {
            int data = in_stream.read();
            out_stream.write(data * 2);
        }
    }
}
/* 特点：
   - 无ap_start/ap_done信号
   - 上电即开始运行
   - 通常与axis接口配合使用
*/
```

---

## 3. PIPELINE Pragma详解

### 3.1 基本语法与选项

```c
#pragma HLS PIPELINE \
    [II=<interval>] \
    [off] \
    [rewind] \
    [style=<stp|flp>] \
    [enable_flush]

// II: Initiation Interval，启动间隔（默认=1）
// off: 禁用流水线
// rewind: 循环间流水线（连续数据流）
// style: stp(stall pipeline)或flp(flush pipeline)
// enable_flush: 启用流水线刷新
```

### 3.2 II（启动间隔）详解

```c
// II=1 - 理想吞吐，每周期启动一次迭代
void pipeline_ii1(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        c[i] = a[i] + b[i];  // 简单操作，可达II=1
    }
}
// 总周期 = 100 + 流水线深度 - 1 ≈ 102

// II=2 - 放宽要求，允许资源复用
void pipeline_ii2(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2
        // 2个乘法操作，但只有1个乘法器
        c[i] = a[i] * b[i] + a[i] * 2;
    }
}
// 总周期 = 100 * 2 + 流水线深度 - 1 ≈ 203

// II目标不可达的情况
void pipeline_ii_fail(int a[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        // 乘法有3周期延迟，无法II=1
        // HLS会自动调整II为3或插入流水线寄存器
        c[i] = a[i] * a[i] * a[i];
    }
}
```

### 3.3 PIPELINE OFF - 禁用流水线

```c
// 在特定区域禁用流水线
void selective_pipeline(int a[1000], int b[1000]) {
    #pragma HLS PIPELINE II=1

    for (int i = 0; i < 500; i++) {
        a[i] = process1(a[i]);  // 流水线执行
    }

    // 复杂控制流，难以流水线
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE off
        // 条件分支复杂，禁用流水线
        if (condition1(i)) {
            a[i] = special_process(a[i]);
        } else if (condition2(i)) {
            a[i] = another_process(a[i]);
        } else {
            for (int j = 0; j < 10; j++) {
                a[i] += b[j];
            }
        }
    }

    for (int i = 500; i < 1000; i++) {
        a[i] = process2(a[i]);  // 流水线执行
    }
}
```

### 3.4 REWIND选项 - 循环间流水线

```c
// rewind实现连续数据流处理
void pipeline_rewind(int in[FRAME_SIZE], int out[FRAME_SIZE]) {
    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        #pragma HLS PIPELINE II=1 rewind

        for (int i = 0; i < FRAME_SIZE; i++) {
            #pragma HLS UNROLL factor=4
            out[i] = filter(in[i]);
        }
    }
}

/* 执行时序对比：
 *
 * 无rewind：
 * Frame0: [════════════════════] (1000周期)
 * Frame1:                          [════════════════════] (1000周期)
 * Frame2:                                               [════════════════════]
 *
 * 有rewind：
 * Frame0: [════════════════════]
 * Frame1:      [════════════════════]  <-- 当前帧结束时下一帧已启动
 * Frame2:           [════════════════════]
 *
 * 延迟：FRAME_SIZE + 流水线填充 ≈ 1002周期
 * 吞吐：每周期完成FRAME_SIZE/4个输出
 */

// rewind的限制条件
void rewind_limitations(int a[100], int b[100]) {
    for (int i = 0; i < 10; i++) {
        #pragma HLS PIPELINE rewind

        // ✓ 支持：完美嵌套循环
        for (int j = 0; j < 100; j++) {
            #pragma HLS UNROLL
            a[j] = b[j] + i;
        }

        // ✗ 不支持：循环间依赖
        // a[i] = a[i-1] + 1;  // 依赖上一迭代结果

        // ✗ 不支持：条件退出
        // if (some_condition) break;
    }
}
```

### 3.5 流水线风格

```c
// stp (Stall Pipeline) - 停顿流水线
void pipeline_stp(int a[100], int b[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE style=stp
        // 当有效数据不可用时流水线停顿
        b[i] = complex_operation(a[i]);
    }
}

// flp (Flush Pipeline) - 刷新流水线
void pipeline_flp(int a[100], int b[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE style=flp enable_flush
        // 循环退出时刷新流水线中的数据
        b[i] = another_operation(a[i]);
    }
}

/* 对比：
 *
 * STP (Stall Pipeline):
 * - 保持流水线状态
 * - 适合连续流处理
 * - 功耗较高（始终活跃）
 *
 * FLP (Flush Pipeline):
 * - 可清空流水线
 * - 适合突发处理
 * - 支持enable_flush提前退出
 */
```

---

## 4. UNROLL Pragma详解

### 4.1 基本语法与选项

```c
#pragma HLS UNROLL \
    [factor=<N>] \
    [region] \
    [skip_exit_check]

// factor: 展开因子（完全展开时不指定）
// region: 区域展开（包含嵌套循环）
// skip_exit_check: 跳过退出条件检查
```

### 4.2 完全展开

```c
// 完全展开 - 所有迭代同时执行
void unroll_full(int a[8], int b[8], int c[8]) {
    for (int i = 0; i < 8; i++) {
        #pragma HLS UNROLL
        c[i] = a[i] + b[i];
    }
}

// 等效于：
void unroll_full_equivalent(int a[8], int b[8], int c[8]) {
    c[0] = a[0] + b[0];
    c[1] = a[1] + b[1];
    c[2] = a[2] + b[2];
    c[3] = a[3] + b[3];
    c[4] = a[4] + b[4];
    c[5] = a[5] + b[5];
    c[6] = a[6] + b[6];
    c[7] = a[7] + b[7];
}

// 硬件资源：8个加法器，1周期完成
// 适用场景：小循环（<32迭代），需要最大并行性
```

### 4.3 部分展开

```c
// 展开因子4 - 循环体复制4次
void unroll_factor4(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL factor=4
        c[i] = a[i] + b[i];
    }
}

// 等效于：
void unroll_factor4_equivalent(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i += 4) {
        c[i+0] = a[i+0] + b[i+0];  // 同时执行
        c[i+1] = a[i+1] + b[i+1];  // 同时执行
        c[i+2] = a[i+2] + b[i+2];  // 同时执行
        c[i+3] = a[i+3] + b[i+3];  // 同时执行
    }
}

// 硬件资源：4个加法器，25周期完成
// 适用场景：平衡并行性与资源

// 边界处理（当100不是4的倍数）
void unroll_boundary(int a[100], int b[100], int c[100]) {
    // HLS自动处理：
    // - 生成条件逻辑保护越界访问
    // - 或使用skip_exit_check指示循环次数是倍数

    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL factor=4 skip_exit_check
        // 使用skip_exit_check时，HLS假设循环次数是factor的倍数
        // 性能更好，但需要用户保证安全
        c[i] = a[i] + b[i];
    }
}
```

### 4.4 区域展开

```c
// region展开 - 展开嵌套循环区域
void unroll_region(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        #pragma HLS UNROLL region

        // 整个内层循环区域被展开
        for (int j = 0; j < 10; j++) {
            c[i][j] = a[i][j] + b[i][j];
        }

        // 其他代码也在展开区域内
        int sum = 0;
        for (int k = 0; k < 10; k++) {
            sum += c[i][k];
        }
    }
}

// 对比：仅内层展开
void unroll_inner_only(int a[10][10], int b[10][10], int c[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            #pragma HLS UNROLL  // 仅这一层展开
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
```

### 4.5 UNROLL与其他指令组合

```c
// UNROLL + PIPELINE
void unroll_with_pipeline(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i += 4) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        // 每周期启动一次外层迭代
        // 每次外层迭代内，4个内层操作并行
        c[i+0] = a[i+0] + b[i+0];
        c[i+1] = a[i+1] + b[i+1];
        c[i+2] = a[i+2] + b[i+2];
        c[i+3] = a[i+3] + b[i+3];
    }
}

// UNROLL + ARRAY_PARTITION
void unroll_with_partition(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=b cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=c cyclic factor=4

    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL factor=4
        // 分区后的数组支持4端口并行访问
        c[i] = a[i] + b[i];
    }
}
```

---

## 5. ARRAY_PARTITION Pragma详解

### 5.1 基本语法与选项

```c
#pragma HLS ARRAY_PARTITION \
    variable=<array_name> \
    <type> \
    [dim=<dimension>] \
    [factor=<int>] \

// type:
// - complete: 完全分区（每个元素独立）
// - block: 块分区
// - cyclic: 循环分区

// dim:
// - 0: 所有维度
// - 1,2,3,...: 指定维度

// factor:
// - block/cyclic分区的因子
```

### 5.2 Complete分区

```c
// 一维数组完全分区
void partition_complete_1d(int a[16], int b[16], int c[16]) {
    #pragma HLS ARRAY_PARTITION variable=a complete
    #pragma HLS ARRAY_PARTITION variable=b complete
    #pragma HLS ARRAY_PARTITION variable=c complete

    for (int i = 0; i < 16; i++) {
        #pragma HLS UNROLL
        c[i] = a[i] + b[i];
    }
}

/* 硬件结构：
 * a[0] ──┐
 * a[1] ──┤
 * ...    ├──▶ 16个并行加法器
 * a[15] ─┤
 *        │
 * b[0] ──┤
 * b[1] ──┤
 * ...    │
 * b[15] ─┘
 *
 * 资源：16个加法器，1周期完成
 * 适用：小数组（<32元素），需要完全并行访问
 */

// 多维数组完全分区
void partition_complete_2d(int a[4][4], int b[4][4], int c[4][4]) {
    #pragma HLS ARRAY_PARTITION variable=a complete
    // 等效于 dim=0，所有维度完全分区
    // 16个独立存储器

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            #pragma HLS UNROLL
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

// 指定维度完全分区
void partition_dim(int a[4][4], int b[4][4], int c[4][4]) {
    #pragma HLS ARRAY_PARTITION variable=a dim=1 complete
    // 第1维完全分区：4个存储器，每个存储4个元素
    // a[0][*], a[1][*], a[2][*], a[3][*]

    #pragma HLS ARRAY_PARTITION variable=b dim=2 complete
    // 第2维完全分区：4个存储器，每个跨第1维存储
    // a[*][0], a[*][1], a[*][2], a[*][3]

    for (int i = 0; i < 4; i++) {
        #pragma HLS PIPELINE
        for (int j = 0; j < 4; j++) {
            #pragma HLS UNROLL
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}
```

### 5.3 Block分区

```c
// Block分区 - 连续元素分到不同存储器
void partition_block(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a block factor=4
    // 分为4个块，每块25个元素
    // block0: a[0..24]
    // block1: a[25..49]
    // block2: a[50..74]
    // block3: a[75..99]

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        c[i] = a[i] + b[i];  // 访问block(i/25)
    }
}

/* 访问模式：
 * 索引: 0  1  2  ... 24 | 25 26 ... 49 | ...
 * 块:   ├─ block0 ─────┤├─ block1 ────┤├─ ...
 *
 * 适合：连续访问模式，空间局部性好
 */

// 二维数组Block分区
void partition_block_2d(int a[16][16], int b[16][16]) {
    #pragma HLS ARRAY_PARTITION variable=a dim=1 block factor=4
    // 第1维分为4块
    // block0: a[0..3][*]
    // block1: a[4..7][*]
    // block2: a[8..11][*]
    // block3: a[12..15][*]

    #pragma HLS ARRAY_PARTITION variable=b dim=2 block factor=4
    // 第2维分为4块

    for (int i = 0; i < 16; i++) {
        #pragma HLS PIPELINE
        for (int j = 0; j < 16; j++) {
            #pragma HLS UNROLL
            // a按行访问，b按列访问
            // 需要合理分区以避免冲突
            a[i][j] = a[i][j] + b[j][i];
        }
    }
}
```

### 5.4 Cyclic分区

```c
// Cyclic分区 - 循环交错分配
void partition_cyclic(int a[100], int b[100], int c[100]) {
    #pragma HLS ARRAY_PARTITION variable=a cyclic factor=4
    // 循环分配到4个存储器
    // bank0: a[0], a[4], a[8], ...
    // bank1: a[1], a[5], a[9], ...
    // bank2: a[2], a[6], a[10], ...
    // bank3: a[3], a[7], a[11], ...

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        c[i] = a[i] + b[i];  // 索引i访问bank(i%4)
    }
}

/* 访问模式：
 * 索引: 0  1  2  3 | 4  5  6  7 | 8  9  10 11 | ...
 * 块:   b0 b1 b2 b3 | b0 b1 b2 b3| b0 b1 b2 b3 | ...
 *
 * 适合：步长访问，并行访问不同元素
 */

// 矩阵乘法中的Cyclic分区应用
void matrix_multiply_cyclic(int A[32][32], int B[32][32], int C[32][32]) {
    // 第1维cyclic分区：支持行并行读取
    #pragma HLS ARRAY_PARTITION variable=A dim=1 cyclic factor=8

    // 第2维cyclic分区：支持列并行读取
    #pragma HLS ARRAY_PARTITION variable=B dim=2 cyclic factor=8

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            #pragma HLS PIPELINE II=1

            int sum = 0;
            for (int k = 0; k < 32; k += 8) {
                #pragma HLS UNROLL factor=8
                // 8个并行访问：A[i][k+0..7]和B[k+0..7][j]
                sum += A[i][k+0] * B[k+0][j] +
                       A[i][k+1] * B[k+1][j] +
                       ... +
                       A[i][k+7] * B[k+7][j];
            }
            C[i][j] = sum;
        }
    }
}
```

### 5.5 分区策略选择

```c
/*
 * 分区策略选择指南：
 *
 * ┌─────────────┬─────────────────────────────────────────────────┐
 * │ 策略        │ 适用场景                                        │
 * ├─────────────┼─────────────────────────────────────────────────┤
 * │ complete    │ - 小数组（<32元素）                             │
 * │             │ - 需要完全并行访问                              │
 * │             │ - 寄存器文件实现                                │
 * ├─────────────┼─────────────────────────────────────────────────┤
 * │ block       │ - 连续访问模式                                  │
 * │             │ - 大块数据处理                                  │
 * │             │ - 空间局部性好                                  │
 * ├─────────────┼─────────────────────────────────────────────────┤
 * │ cyclic      │ - 步长访问                                      │
 * │             │ - 需要并行访问相邻元素                          │
 * │             │ - 矩阵运算中的行列访问                          │
 * └─────────────┴─────────────────────────────────────────────────┘
 */

// 示例：根据访问模式选择分区
void smart_partition(int a[100], int b[100], int c[100]) {
    // 顺序访问 - 使用block
    #pragma HLS ARRAY_PARTITION variable=a block factor=4

    // 步长为4访问 - 使用cyclic
    #pragma HLS ARRAY_PARTITION variable=b cyclic factor=4

    // 小数组 - 使用complete
    int small[8];
    #pragma HLS ARRAY_PARTITION variable=small complete

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE

        // 顺序访问a
        int val_a = a[i];

        // 步长访问b（假设某种模式）
        int val_b = b[(i * 4) % 100];

        // 小数组完全并行处理
        for (int j = 0; j < 8; j++) {
            #pragma HLS UNROLL
            small[j] = process(val_a, val_b, j);
        }

        c[i] = reduce(small);
    }
}
```

---

## 6. DATAFLOW Pragma详解

### 6.1 基本语法与选项

```c
#pragma HLS DATAFLOW \
    [disable_start_propagation]

// 可选指令（与DATAFLOW配合使用）：
#pragma HLS STREAM \
    variable=<variable> \
    [depth=<N>] \
    [type=<fifo|pingpong>]

#pragma HLS CHANNEL \
    variable=<variable> \
    [depth=<N>]
```

### 6.2 DATAFLOW基本原理

```c
// 顺序执行（无DATAFLOW）
void sequential_stages(int in[1000], int out[1000]) {
    int temp1[1000], temp2[1000];

    // Stage 1: 1000周期
    stage1_process(in, temp1, 1000);

    // Stage 2: 1000周期
    stage2_process(temp1, temp2, 1000);

    // Stage 3: 1000周期
    stage3_process(temp2, out, 1000);

    // 总延迟 = 3000周期
}

// DATAFLOW优化
void dataflow_stages(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    int temp1[1000], temp2[1000];
    #pragma HLS STREAM variable=temp1 depth=16
    #pragma HLS STREAM variable=temp2 depth=16

    // 三个阶段并发执行
    stage1_process(in, temp1, 1000);
    stage2_process(temp1, temp2, 1000);
    stage3_process(temp2, out, 1000);

    // 总延迟 ≈ 1000 + 2*16 + 填充 ≈ 1032周期
}

/* DATAFLOW执行示意图：
 *
 * 时间 ──────────────────────────────────────────────▶
 *
 * Stage1: [══════════════]
 *         in[0]          in[999]
 *             │              │
 *             ▼              ▼
 * Stage2:      [══════════════]
 *              temp1[0]      temp1[999]
 *                  │              │
 *                  ▼              ▼
 * Stage3:           [══════════════]
 *                   temp2[0]      temp2[999]
 *                       │              │
 *                       ▼              ▼
 * Output:               out[0]       out[999]
 *
 * 三个阶段流水线执行，通过FIFO连接
 */
```

### 6.3 STREAM指令配置

```c
// FIFO流通道
void dataflow_fifo(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    hls::stream<int> stream1("stream1");
    hls::stream<int> stream2("stream2");
    #pragma HLS STREAM variable=stream1 depth=32 mode=fifo
    #pragma HLS STREAM variable=stream2 depth=32 mode=fifo

    stage1_stream(in, stream1, 1000);
    stage2_stream(stream1, stream2, 1000);
    stage3_stream(stream2, out, 1000);
}

// Ping-Pong缓冲（double buffering）
void dataflow_pingpong(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    int buffer1[128], buffer2[128];
    #pragma HLS STREAM variable=buffer1 depth=128 type=pingpong
    #pragma HLS STREAM variable=buffer2 depth=128 type=pingpong

    // 生产者写入buffer1时，消费者读取buffer2
    // 交替进行，无等待
    producer(in, buffer1, buffer2, 1000);
    consumer(buffer1, buffer2, out, 1000);
}

// 自动流推断
void dataflow_auto(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    int temp[1000];  // HLS自动推断为流通道
    // 默认depth=2（根据调度分析）

    stage1(in, temp, 1000);
    stage2(temp, out, 1000);
}
```

### 6.4 DATAFLOW使用约束

```c
/*
 * DATAFLOW使用约束：
 *
 * 1. 单生产者单消费者
 * 2. 数据按顺序写入和读取
 * 3. 函数内所有路径都必须执行（无双边控制流）
 * 4. 循环边界必须在编译时确定
 */

// ✓ 正确的DATAFLOW用法
void dataflow_correct(int in[100], int out[100]) {
    #pragma HLS DATAFLOW

    int temp1[100], temp2[100];

    func_a(in, temp1);
    func_b(temp1, temp2);
    func_c(temp2, out);
}

// ✗ 错误：条件执行
void dataflow_wrong_conditional(int in[100], int out[100], int mode) {
    #pragma HLS DATAFLOW

    int temp[100];

    if (mode == 0) {
        func_a(in, temp);  // 可能不执行
    } else {
        func_b(in, temp);  // 可能不执行
    }
    func_c(temp, out);
}
// 解决方案：将条件移到函数内部或使用DATAFLOW之外的调度

// ✗ 错误：多生产者
void dataflow_wrong_multi_producer(int in[100], int out[100]) {
    #pragma HLS DATAFLOW

    int temp[100];

    func_a(in, temp);
    func_b(in, temp);  // 第二个生产者！
    func_c(temp, out);
}
// 解决方案：合并生产者或使用独立缓冲区

// ✗ 错误：反馈环路
void dataflow_wrong_feedback(int in[100], int out[100]) {
    #pragma HLS DATAFLOW

    int temp[100];

    func_a(in, temp);
    func_b(temp, out);
    func_c(out, temp);  // 反馈！
}
// 解决方案：使用ap_shift_reg或重构算法
```

### 6.5 复杂DATAFLOW设计

```c
// 多分支DATAFLOW
void dataflow_branch(int in[1000], int out_a[1000], int out_b[1000]) {
    #pragma HLS DATAFLOW

    hls::stream<int> stream_in("in");
    hls::stream<int> stream_mid1("mid1");
    hls::stream<int> stream_mid2("mid2");

    // 输入分发
    distributor(in, stream_in, 1000);

    // 并行处理分支
    branch_a(stream_in, stream_mid1, 1000);
    branch_b(stream_in, stream_mid2, 1000);

    // 结果收集
    collector_a(stream_mid1, out_a, 1000);
    collector_b(stream_mid2, out_b, 1000);
}

/* 结构图：
 *           ┌────────▶ Branch A ──────┐
 *           │                         ▼
 * Input ────┤                    Collector A ──▶ Out A
 *           │                         ▲
 *           │                         │
 *           └────────▶ Branch B ──────┤
 *                                Collector B ──▶ Out B
 *
 * 注意：需要确保distributor正确处理数据路由
 */

// DATAFLOW + 任务并行
void dataflow_parallel_tasks(int in[1000], int out[1000]) {
    #pragma HLS DATAFLOW

    hls::stream<int> s1("s1"), s2("s2"), s3("s3");
    hls::stream<int> s_merge("s_merge");

    // 输入预处理
    preprocess(in, s1, 1000);

    // 并行任务
    task_a(s1, s2, 1000);
    task_b(s1, s3, 1000);

    // 合并结果
    merge_streams(s2, s3, s_merge, 1000);

    // 最终处理
    postprocess(s_merge, out, 1000);
}
```

---

## 7. 完整示例：综合运用

### 7.1 图像卷积加速器（综合示例）

```c
/*
 * 图像卷积加速器
 * 功能：3x3卷积核实时图像处理
 * 优化目标：1080p@60fps
 */

#include "ap_axi_sdata.h"
#include "ap_int.h"

#define IMG_WIDTH  1920
#define IMG_HEIGHT 1080
#define KERNEL_SIZE 3

typedef ap_uint<8> pixel_t;
typedef ap_int<16> coeff_t;
typedef ap_int<32> accum_t;

// 卷积核系数（通过AXI-Lite配置）
coeff_t kernel[KERNEL_SIZE][KERNEL_SIZE];
#pragma HLS ARRAY_PARTITION variable=kernel complete

// 行缓冲区模块
void line_buffer_stage(
    hls::stream<pixel_t> &pixel_in,
    hls::stream<pixel_t[KERNEL_SIZE]> &window_out,
    int width, int height
) {
    #pragma HLS INLINE off

    static pixel_t line_buf[KERNEL_SIZE-1][IMG_WIDTH];
    #pragma HLS ARRAY_PARTITION variable=line_buf complete dim=1
    #pragma HLS RESOURCE variable=line_buf core=ram_s2p

    pixel_t window[KERNEL_SIZE][KERNEL_SIZE];
    #pragma HLS ARRAY_PARTITION variable=window complete dim=0

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1

            // 读取新像素
            pixel_t new_pixel = pixel_in.read();

            // 更新窗口（列方向移位）
            for (int i = 0; i < KERNEL_SIZE; i++) {
                #pragma HLS UNROLL
                for (int j = 0; j < KERNEL_SIZE-1; j++) {
                    #pragma HLS UNROLL
                    window[i][j] = window[i][j+1];
                }
            }

            // 从行缓冲区或输入填充新列
            window[0][KERNEL_SIZE-1] = line_buf[0][x];
            window[1][KERNEL_SIZE-1] = line_buf[1][x];
            window[2][KERNEL_SIZE-1] = new_pixel;

            // 更新行缓冲区
            if (y >= 1) line_buf[0][x] = line_buf[1][x];
            line_buf[1][x] = new_pixel;

            // 输出窗口（有效像素）
            if (y >= KERNEL_SIZE-1 && x >= KERNEL_SIZE-1) {
                pixel_t out_window[KERNEL_SIZE];
                #pragma HLS ARRAY_PARTITION variable=out_window complete
                for (int i = 0; i < KERNEL_SIZE; i++) {
                    #pragma HLS UNROLL
                    out_window[i] = window[i][0];  // 输出第一列
                }
                window_out.write(out_window);
            }
        }
    }
}

// 卷积计算模块
void convolution_stage(
    hls::stream<pixel_t[KERNEL_SIZE]> &window_in,
    hls::stream<pixel_t> &pixel_out,
    int width, int height,
    coeff_t kernel[KERNEL_SIZE][KERNEL_SIZE]
) {
    #pragma HLS INLINE off

    pixel_t window[KERNEL_SIZE][KERNEL_SIZE];
    #pragma HLS ARRAY_PARTITION variable=window complete dim=0

    for (int y = KERNEL_SIZE-1; y < height; y++) {
        for (int x = KERNEL_SIZE-1; x < width; x++) {
            #pragma HLS PIPELINE II=1

            // 读取窗口列
            pixel_t in_col[KERNEL_SIZE];
            #pragma HLS ARRAY_PARTITION variable=in_col complete
            window_in.read(in_col);

            // 更新窗口
            for (int i = 0; i < KERNEL_SIZE; i++) {
                #pragma HLS UNROLL
                window[i][KERNEL_SIZE-1] = in_col[i];
                for (int j = 0; j < KERNEL_SIZE-1; j++) {
                    #pragma HLS UNROLL
                    window[i][j] = window[i][j+1];
                }
            }

            // 3x3卷积计算
            accum_t sum = 0;
            for (int i = 0; i < KERNEL_SIZE; i++) {
                #pragma HLS UNROLL
                for (int j = 0; j < KERNEL_SIZE; j++) {
                    #pragma HLS UNROLL
                    sum += (accum_t)window[i][j] * kernel[i][j];
                }
            }

            // 量化和裁剪
            sum = sum >> 8;  // 定点数调整
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;

            pixel_out.write((pixel_t)sum);
        }
    }
}

// 顶层模块
void image_convolution_top(
    hls::stream<ap_axiu<8, 0, 0, 0>> &video_in,
    hls::stream<ap_axiu<8, 0, 0, 0>> &video_out,
    coeff_t kernel_cfg[KERNEL_SIZE][KERNEL_SIZE],
    int width, int height
) {
    #pragma HLS INTERFACE mode=axis port=video_in
    #pragma HLS INTERFACE mode=axis port=video_out
    #pragma HLS INTERFACE mode=s_axilite port=kernel_cfg bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=width bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=height bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=control

    #pragma HLS ARRAY_PARTITION variable=kernel_cfg complete dim=0
    #pragma HLS DATAFLOW

    // 加载卷积核
    for (int i = 0; i < KERNEL_SIZE; i++) {
        #pragma HLS UNROLL
        for (int j = 0; j < KERNEL_SIZE; j++) {
            #pragma HLS UNROLL
            kernel[i][j] = kernel_cfg[i][j];
        }
    }

    // 流转换
    hls::stream<pixel_t> pixel_stream("pixel_stream");
    hls::stream<pixel_t[KERNEL_SIZE]> window_stream("window_stream");
    hls::stream<pixel_t> result_stream("result_stream");
    #pragma HLS STREAM variable=pixel_stream depth=16
    #pragma HLS STREAM variable=window_stream depth=16
    #pragma HLS STREAM variable=result_stream depth=16

    // DATAFLOW任务
    axis_to_pixel(video_in, pixel_stream, width * height);
    line_buffer_stage(pixel_stream, window_stream, width, height);
    convolution_stage(window_stream, result_stream, width, height, kernel);
    pixel_to_axis(result_stream, video_out, (width-KERNEL_SIZE+1) * (height-KERNEL_SIZE+1));
}

// 辅助函数
void axis_to_pixel(
    hls::stream<ap_axiu<8, 0, 0, 0>> &axis_in,
    hls::stream<pixel_t> &pixel_out,
    int pixel_count
) {
    for (int i = 0; i < pixel_count; i++) {
        #pragma HLS PIPELINE II=1
        ap_axiu<8, 0, 0, 0> data = axis_in.read();
        pixel_out.write(data.data);
    }
}

void pixel_to_axis(
    hls::stream<pixel_t> &pixel_in,
    hls::stream<ap_axiu<8, 0, 0, 0>> &axis_out,
    int pixel_count
) {
    for (int i = 0; i < pixel_count; i++) {
        #pragma HLS PIPELINE II=1
        ap_axiu<8, 0, 0, 0> data;
        data.data = pixel_in.read();
        data.keep = 0x1;
        data.strb = 0x1;
        data.last = (i == pixel_count - 1);
        axis_out.write(data);
    }
}

/*
 * 优化要点总结：
 *
 * 1. INTERFACE:
 *    - axis: 视频流接口
 *    - s_axilite: 控制接口（配置参数）
 *
 * 2. PIPELINE II=1:
 *    - 每周期处理一个像素
 *    - 满足1080p@60fps（需148.5MHz时钟）
 *
 * 3. UNROLL:
 *    - 卷积核完全展开（9个并行乘法）
 *    - 窗口移位完全展开
 *
 * 4. ARRAY_PARTITION:
 *    - 行缓冲区第1维完全分区
 *    - 卷积核完全分区
 *    - 窗口完全分区
 *
 * 5. DATAFLOW:
 *    - 流水线级间并行
 *    - FIFO缓冲数据流
 *
 * 6. RESOURCE:
 *    - 行缓冲区使用双端口RAM
 *
 * 7. INLINE off:
 *    - 模块独立，便于调试和重用
 */
```

### 7.2 优化效果对比

```
┌─────────────────────────────────────────────────────────────────────┐
│              图像卷积加速器优化效果对比                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  优化阶段           帧率      资源使用                    功耗      │
│                     (fps)    LUT    FF    BRAM   DSP     (mW)      │
│  ────────────────────────────────────────────────────────────────  │
│                                                                      │
│  未优化             2        2K     1K     2      1       50        │
│  + PIPELINE         15       3K     2K     2      1       80        │
│  + UNROLL           30       5K     4K     2      9       150       │
│  + ARRAY_PARTITION  45       6K     5K     4      9       180       │
│  + DATAFLOW         60       7K     6K     4      9       200       │
│  + 精细调优         60       6K     5K     3      9       170       │
│                                                                      │
│  目标：1080p@60fps = 1920*1080*60 = 124.4M像素/秒                   │
│  实现：II=1 @ 150MHz = 150M像素/秒 ✓                                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
*/

---

## 总结

本章详细讲解了HLS pragma指令的使用：

1. **INTERFACE**：定义端口接口模式，连接外部世界
2. **PIPELINE**：实现循环流水线，提高吞吐率
3. **UNROLL**：展开循环实现空间并行
4. **ARRAY_PARTITION**：优化存储器访问，消除瓶颈
5. **DATAFLOW**：实现任务级并行，重叠执行
6. **综合运用**：通过图像卷积加速器展示完整优化流程

掌握这些pragma指令是高效HLS设计的关键。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
