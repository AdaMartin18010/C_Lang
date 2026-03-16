# ISO 26262 - 道路车辆功能安全

> 汽车电子电气系统的功能安全国际标准

---

## 概述

ISO 26262是源自IEC 61508的国际标准，专门针对道路车辆（乘用车、卡车、巴士等）的电子电气系统的功能安全。

| 属性 | 说明 |
|:-----|:-----|
| **版本** | ISO 26262:2018（第二版）|
| **适用范围** | 乘用车、卡车、巴士、拖车、半挂车 |
| **核心概念** | ASIL（汽车安全完整性等级）|
| **部分数量** | 12部分 |

---

## ASIL等级

ASIL根据风险严重程度(S)、暴露概率(E)和可控性(C)确定。

| 等级 | 描述 | 示例 |
|:-----|:-----|:-----|
| QM | 质量管理 | 信息娱乐 |
| ASIL A | 最低安全 | 后视镜除雾 |
| ASIL B | 低安全 | 大灯控制 |
| ASIL C | 中安全 | 大灯自动调节 |
| ASIL D | 最高安全 | 转向、制动 |

---

## 软件开发要求（Part 6）

### ASIL与MISRA C关系

| ASIL | MISRA C要求 |
|:-----|:------------|
| QM | 建议符合 |
| ASIL A/B | 建议符合Required规则 |
| ASIL C/D | 必须符合Required+Mandatory |

### 防御性编程示例

```c
/* ASIL D级别的防御性编程 */
int process_sensor_data(const SensorData *data)
{
    /* NULL检查 */
    if (data == NULL) {
        return ERROR_NULL_POINTER;
    }

    /* 范围检查 */
    if (data->value < SENSOR_MIN || data->value > SENSOR_MAX) {
        return ERROR_OUT_OF_RANGE;
    }

    /* 合理性检查 */
    if (data->timestamp == 0) {
        return ERROR_INVALID_TIMESTAMP;
    }

    return process_valid_data(data);
}
```

---

**参考**: [MISRA C:2023](../MISRA_C_2023/)
