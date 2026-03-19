/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 423
 * Language: c
 * Block ID: 74d112ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file fcs_requirements.h
 * @brief 飞行控制系统高层需求 (HLR)
 *
 * 符合 DO-178C 需求可追溯性要求
 * 每个需求都有唯一的标识符和验证方法
 */

#ifndef FCS_REQUIREMENTS_H
#define FCS_REQUIREMENTS_H

/* ============================================================================
 * 高层需求 HLR-FCS-001 至 HLR-FCS-050
 * ============================================================================ */

/**
 * @req HLR-FCS-001
 * @brief 俯仰控制需求
 * @description
 *   飞行控制系统应控制飞机俯仰姿态，俯仰角范围 ±25度。
 *   控制精度：±0.5度（1σ）。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试 (HLR测试用例)
 * @trace_to SSR-FCS-001, SSR-FCS-002
 */
#define HLR_FCS_001_PITCH_CONTROL \
    "FCS shall control aircraft pitch attitude within ±25 degrees"

/**
 * @req HLR-FCS-002
 * @brief 横滚控制需求
 * @description
 *   飞行控制系统应控制飞机横滚姿态，横滚角范围 ±67度。
 *   控制精度：±1.0度（1σ）。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试
 */
#define HLR_FCS_002_ROLL_CONTROL \
    "FCS shall control aircraft roll attitude within ±67 degrees"

/**
 * @req HLR-FCS-003
 * @brief 失速保护
 * @description
 *   当迎角超过临界值时，系统应自动推杆降低机头，
 *   防止飞机进入失速状态。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试 + 分析
 */
#define HLR_FCS_003_STALL_PROTECTION \
    "FCS shall provide automatic stall protection when AOA exceeds threshold"

/**
 * @req HLR-FCS-004
 * @brief 过载限制
 * @description
 *   飞行控制系统应限制法向过载在 [-1.0g, +2.5g] 范围内，
 *   防止结构损坏。
 * @safety_level DAL A
 * @allocation 飞控计算机
 * @verification 测试
 */
#define HLR_FCS_004_LOAD_FACTOR_LIMIT \
    "FCS shall limit normal load factor to [-1.0g, +2.5g]"

/**
 * @req HLR-FCS-005
 * @brief 多通道冗余
 * @description
 *   飞行控制系统应采用三通道冗余架构，
 *   任一通道失效时系统继续正常工作。
 * @safety_level DAL A
 * @allocation 飞控计算机硬件
 * @verification 测试 + 分析
 */
#define HLR_FCS_005_TRIPLE_REDUNDANCY \
    "FCS shall implement triple modular redundancy"

/**
 * @req HLR-FCS-006
 * @brief 通道监控
 * @description
 *   系统应持续监控各通道输出，
 *   当检测到通道故障时自动隔离故障通道。
 * @safety_level DAL A
 * @allocation 飞控软件
 * @verification 测试
 */
#define HLR_FCS_006_CHANNEL_MONITORING \
    "FCS shall monitor channel outputs and isolate faulty channels"

/**
 * @req HLR-FCS-007
 * @brief 飞行员超控
 * @description
 *   飞行员应能通过施加足够的杆力超控自动驾驶，
 *   超控力不应超过 50磅。
 * @safety_level DAL A
 * @allocation 飞控系统
 * @verification 测试
 */
#define HLR_FCS_007_PILOT_OVERRIDE \
    "FCS shall allow pilot override with force not exceeding 50 lbf"

/* ============================================================================
 * 低层需求示例（软件实现需求）
 * ============================================================================ */

/**
 * @req LLR-FCS-001
 * @brief 俯仰控制律计算
 * @description
 *   俯仰控制律应按以下公式计算：
 *   δe = Kq * q + K_theta * (theta_cmd - theta) + ...
 *   其中 δe 为升降舵指令，单位：度
 * @parent_req HLR-FCS-001
 * @safety_level DAL A
 * @verification 评审 + 测试
 */

/**
 * @req LLR-FCS-002
 * @brief 控制指令范围检查
 * @description
 *   所有控制面输出指令应进行范围检查，
 *   确保在 [-100%, +100%] 范围内。
 *   超出范围时限制在边界值并报告错误。
 * @parent_req HLR-FCS-001
 * @safety_level DAL A
 * @verification 测试
 */

#endif /* FCS_REQUIREMENTS_H */
