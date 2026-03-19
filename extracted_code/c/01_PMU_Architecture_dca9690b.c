/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 402
 * Language: c
 * Block ID: dca9690b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ARMv8 PMUv3 公共事件编码 (部分) */
#define ARMV8_PMUV3_EVENT_SW_INCR               0x00
#define ARMV8_PMUV3_EVENT_L1I_CACHE_REFILL      0x01
#define ARMV8_PMUV3_EVENT_L1I_TLB_REFILL        0x02
#define ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL      0x03
#define ARMV8_PMUV3_EVENT_L1D_CACHE             0x04
#define ARMV8_PMUV3_EVENT_L1D_TLB_REFILL        0x05
#define ARMV8_PMUV3_EVENT_LD_RETIRED            0x06
#define ARMV8_PMUV3_EVENT_ST_RETIRED            0x07
#define ARMV8_PMUV3_EVENT_INST_RETIRED          0x08
#define ARMV8_PMUV3_EVENT_EXC_TAKEN             0x09
#define ARMV8_PMUV3_EVENT_EXC_RETURN            0x0A
#define ARMV8_PMUV3_EVENT_CID_WRITE_RETIRED     0x0B
#define ARMV8_PMUV3_EVENT_PC_WRITE_RETIRED      0x0C
#define ARMV8_PMUV3_EVENT_BR_IMMED_RETIRED      0x0D
#define ARMV8_PMUV3_EVENT_BR_RETURN_RETIRED     0x0E
#define ARMV8_PMUV3_EVENT_UNALIGNED_LDST_RETIRED 0x0F
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED           0x10
#define ARMV8_PMUV3_EVENT_CPU_CYCLES            0x11
#define ARMV8_PMUV3_EVENT_BR_PRED               0x12
#define ARMV8_PMUV3_EVENT_MEM_ACCESS            0x13
#define ARMV8_PMUV3_EVENT_L1I_CACHE             0x14
#define ARMV8_PMUV3_EVENT_L1D_CACHE_WB          0x15
#define ARMV8_PMUV3_EVENT_L2D_CACHE             0x16
#define ARMV8_PMUV3_EVENT_L2D_CACHE_REFILL      0x17
#define ARMV8_PMUV3_EVENT_L2D_CACHE_WB          0x18
#define ARMV8_PMUV3_EVENT_BUS_ACCESS            0x19
#define ARMV8_PMUV3_EVENT_MEMORY_ERROR          0x1A
#define ARMV8_PMUV3_EVENT_INST_SPEC             0x1B
#define ARMV8_PMUV3_EVENT_TTBR_WRITE_RETIRED    0x1C
#define ARMV8_PMUV3_EVENT_BUS_CYCLES            0x1D
#define ARMV8_PMUV3_EVENT_CHAIN                 0x1E
#define ARMV8_PMUV3_EVENT_L1D_CACHE_ALLOCATE    0x1F
#define ARMV8_PMUV3_EVENT_L2D_CACHE_ALLOCATE    0x20
#define ARMV8_PMUV3_EVENT_BR_RETIRED            0x21
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED_RETIRED   0x22
#define ARMV8_PMUV3_EVENT_STALL_FRONTEND        0x23
#define ARMV8_PMUV3_EVENT_STALL_BACKEND         0x24
#define ARMV8_PMUV3_EVENT_L1D_TLB               0x25
#define ARMV8_PMUV3_EVENT_L1I_TLB               0x26
#define ARMV8_PMUV3_EVENT_L2I_CACHE             0x27
#define ARMV8_PMUV3_EVENT_L2I_CACHE_REFILL      0x28
#define ARMV8_PMUV3_EVENT_L3D_CACHE_ALLOCATE    0x29
#define ARMV8_PMUV3_EVENT_L3D_CACHE_REFILL      0x2A
#define ARMV8_PMUV3_EVENT_L3D_CACHE             0x2B
#define ARMV8_PMUV3_EVENT_L3D_CACHE_WB          0x2C
#define ARMV8_PMUV3_EVENT_L2D_TLB_REFILL        0x2D
#define ARMV8_PMUV3_EVENT_L2I_TLB_REFILL        0x2E
#define ARMV8_PMUV3_EVENT_L2D_TLB               0x2F
#define ARMV8_PMUV3_EVENT_L2I_TLB               0x30
#define ARMV8_PMUV3_EVENT_LD_SPEC               0x70
#define ARMV8_PMUV3_EVENT_ST_SPEC               0x71
#define ARMV8_PMUV3_EVENT_LDST_SPEC             0x72
#define ARMV8_PMUV3_EVENT_DP_SPEC               0x73
#define ARMV8_PMUV3_EVENT_ASE_SPEC              0x74
#define ARMV8_PMUV3_EVENT_VFP_SPEC              0x75
#define ARMV8_PMUV3_EVENT_PC_WRITE_SPEC         0x76
#define ARMV8_PMUV3_EVENT_CRYPTO_SPEC           0x77
#define ARMV8_PMUV3_EVENT_BR_IMMED_SPEC         0x78
#define ARMV8_PMUV3_EVENT_BR_RETURN_SPEC        0x79
#define ARMV8_PMUV3_EVENT_BR_INDIRECT_SPEC      0x7A
