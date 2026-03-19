/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 349
 * Language: c
 * Block ID: 79d3e3cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// autosar_rte_interface.c
// AUTOSAR RTE接口验证示例

#include "Std_Types.h"
#include "Rte_Type.h"

#define RTE_E_OK        0
#define RTE_E_NOK       1
#define RTE_E_INVALID   2
#define RTE_E_TIMEOUT   3

#define SENSOR_DATA_MIN 0
#define SENSOR_DATA_MAX 4095
#define SENSOR_ARRAY_SIZE 8

/*@
  @predicate valid_sensor_data(uint16 value) =
    SENSOR_DATA_MIN <= value <= SENSOR_DATA_MAX;
*/

// RTE写入接口规范
/*@
  @requires \valid(data);
  @requires valid_sensor_data(*data);
  @assigns \nothing;

  @behavior success:
    @assumes // RTE缓冲区可用
    @ensures \result == RTE_E_OK;
  @behavior buffer_full:
    @assumes // RTE缓冲区满
    @ensures \result == RTE_E_NOK;
  @complete behaviors;
  @disjoint behaviors;
*/
Std_ReturnType Rte_Write_SensorData(uint16* data);

// RTE读取接口规范
/*@
  @requires \valid(data);
  @assigns *data;

  @behavior data_available:
    @assumes // 有新数据
    @ensures \result == RTE_E_OK;
    @ensures valid_sensor_data(*data);
  @behavior no_data:
    @assumes // 无新数据
    @ensures \result == RTE_E_NOK;
    @ensures *data == \old(*data);
  @complete behaviors;
  @disjoint behaviors;
*/
Std_ReturnType Rte_Read_SensorData(uint16* data);

// 数组数据传输
/*@
  @requires \valid_read(src + (0..SENSOR_ARRAY_SIZE-1));
  @requires \forall integer i; 0 <= i < SENSOR_ARRAY_SIZE ==>
    valid_sensor_data(src[i]);
  @assigns \nothing;
  @ensures \result == RTE_E_OK ==>
    // 数据已成功传输到RTE
    \true;
*/
Std_ReturnType Rte_Write_SensorArray(const uint16 src[SENSOR_ARRAY_SIZE]);
