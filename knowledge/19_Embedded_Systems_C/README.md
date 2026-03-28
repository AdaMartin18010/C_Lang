# 19_Embedded_Systems_C - 嵌入式系统（C语言核心）

> **定位**: C语言在嵌入式系统开发中的知识体系梳理
> **目标**: 构建从裸机到RTOS、从8位到32/64位的完整知识图谱
> **状态**: 🚧 持续构建中
> **最后更新**: 2026-03-29

---

## 📑 目录结构

```
19_Embedded_Systems_C/
├── README.md                           # 本文件
├── 00_Embedded_Fundamentals/           # 嵌入式基础理论
│   ├── 01_Embedded_System_Definition.md
│   ├── 02_Bare_Metal_vs_OS.md
│   ├── 03_Real_Time_Concepts.md
│   ├── 04_Memory_Maps_and_Linker_Scripts.md
│   ├── 05_Startup_Code_Analysis.md
│   └── 06_Interrupt_Vector_Table.md
├── 01_MCU_Architectures/               # 微控制器架构
│   ├── README.md
│   ├── 01_ARM_Cortex_M_Series.md
│   ├── 02_RISC_V_Embedded.md
│   ├── 03_8051_Legacy.md
│   ├── 04_AVR_Architecture.md
│   └── 05_Espressif_SoC_Architecture.md
├── 02_Peripheral_Programming/          # 外设编程
│   ├── README.md
│   ├── 01_GPIO_Concepts.md
│   ├── 02_Timer_Counter_Modules.md
│   ├── 03_PWM_Generation.md
│   ├── 04_ADC_DAC_Interface.md
│   ├── 05_DMA_Controller.md
│   ├── 06_Watchdog_Timers.md
│   └── 07_Power_Management_Modes.md
├── 03_Embedded_C_Programming/          # 嵌入式C编程
│   ├── README.md
│   ├── 01_Volatile_Keyword.md
│   ├── 02_Bit_Manipulation_Techniques.md
│   ├── 03_Memory_Barriers.md
│   ├── 04_Register_Access_Patterns.md
│   ├── 05_Bitfields_vs_Shifts.md
│   ├── 06_Const_Data_in_Flash.md
│   ├── 07_Stack_Heap_in_Embedded.md
│   └── 08_Function_Pointers_for_ISRs.md
├── 04_RTOS_Concepts/                   # 实时操作系统
│   ├── README.md
│   ├── 01_RTOS_Fundamentals.md
│   ├── 02_Task_Scheduling_Algorithms.md
│   ├── 03_Context_Switch_Mechanism.md
│   ├── 04_Inter_Task_Communication.md
│   ├── 05_Semaphores_and_Mutexes.md
│   ├── 06_Message_Queues.md
│   └── 07_Memory_Management_in_RTOS.md
├── 05_FreeRTOS_Detail/                 # FreeRTOS详解
│   ├── README.md
│   ├── 01_FreeRTOS_Architecture.md
│   ├── 02_Task_Management_API.md
│   ├── 03_Synchronization_Primitives.md
│   ├── 04_Timer_Services.md
│   ├── 05_Event_Groups.md
│   ├── 06_Stream_Buffer.md
│   └── 07_FreeRTOS_on_Different_MCUs.md
├── 06_Zephyr_RTOS/                     # Zephyr RTOS
│   ├── README.md
│   └── 01_Zephyr_Device_Tree.md
├── 07_Bootloaders/                     # 引导程序
│   ├── README.md
│   ├── 01_Bootloader_Concepts.md
│   ├── 02_First_Stage_Bootloader.md
│   ├── 03_Second_Stage_Bootloader.md
│   ├── 04_OTA_Updates.md
│   └── 05_Secure_Boot.md
├── 08_Debugging_Techniques/            # 调试技术
│   ├── README.md
│   ├── 01_JTAG_SWD_Debug.md
│   ├── 02_Serial_Debug_Output.md
│   ├── 03_Assert_Macros.md
│   ├── 04_HardFault_Handler.md
│   └── 05_Logging_Frameworks.md
└── 09_Embedded_Toolchains/             # 嵌入式工具链
    ├── README.md
    ├── 01_GCC_ARM_Embedded.md
    ├── 02_CMake_for_Embedded.md
    ├── 03_Linker_Script_Guide.md
    └── 04_Flash_Programming.md
```

---

## 🎯 C语言嵌入式核心知识点

### 关键概念图谱

```
                    ┌─────────────────────────────────────┐
                    │     Embedded C Programming          │
                    └──────────────┬──────────────────────┘
                                   │
           ┌───────────────────────┼───────────────────────┐
           │                       │                       │
           ▼                       ▼                       ▼
┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────────┐
│   Memory Access     │  │   Concurrency       │  │   Hardware Abstraction │
├─────────────────────┤  ├─────────────────────┤  ├─────────────────────┤
│ • volatile          │  │ • Interrupts        │  │ • Register Maps     │
│ • memory barriers   │  │ • Critical Sections │  │ • Bit manipulation  │
│ • memory alignment  │  │ • Atomic operations │  │ • MMIO patterns     │
│ • linker sections   │  │ • Lock-free         │  │ • Device drivers    │
└─────────────────────┘  └─────────────────────┘  └─────────────────────┘
```

---

## 📊 嵌入式C语言技能矩阵

| 技能领域 | 初级 | 中级 | 高级 | 专家 |
|----------|------|------|------|------|
| **GPIO操作** | 基本读写 | 中断配置 | 复杂状态机 | 自定义硬件抽象层 |
| **定时器** | 简单延时 | PWM生成 | 输入捕获 | 自定义定时器框架 |
| **中断** | 使用库函数 | 手写ISR | 嵌套中断管理 | 零延迟中断 |
| **DMA** | 基本传输 | 循环模式 | 双缓冲 | 分散-聚集 |
| **RTOS** | 任务创建 | 同步原语 | 内存优化 | 自定义调度器 |
| **低功耗** | 睡眠模式 | 时钟门控 | 动态电压调节 | 能量收集系统 |

---

## 🔗 关联模块

| 关联模块 | 关联内容 |
|----------|----------|
| `18_Industrial_Protocols_C` | 协议栈在嵌入式系统上的实现 |
| `20_Hardware_Interfaces_C` | 底层硬件接口驱动 |
| `04_Industrial_Scenarios/14_Embedded_IoT` | 实际应用场景 |
| `01_Core_Knowledge_System` | C语言基础特性 |

---

## 📚 参考资源

- **ARM Cortex-M参考手册**
- **FreeRTOS官方文档**
- **Zephyr项目文档**
- **《嵌入式C编程》Michael Pont**
- **《Making Embedded Systems》Elecia White**
