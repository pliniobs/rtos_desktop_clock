/*
 * sht15.h
 *
 *  Created on: 18 de jul de 2018
 *      Author: plinio
 */

#ifndef SHT15_H_
#define SHT15_H_

#include "fsl_common.h"
#include "returncode.h"

#define SHT15_ACK		0
#define SHT15_NACK		1

#define SHT15_SENSOR_BUSY	1
#define SHT15_SENSOR_READY	0

#define SHT15_TEMPERATURE_READ_INSTRUCTION		0x03
#define SHT15_HUMIDITY_READ_INSTRUCTION			0x05
#define SHT15_STATUS_READ_INSTRUCTION			0x07
#define SHT15_STATUS_WRITE_INSTRUCTION			0x06
#define SHT15_SOFTWARE_RESET_INSTRUCTION		0x1E

#define SENSOR_D1_5V_C	(float)-40.1
#define SENSOR_D1_3V5_C	(float)-39.7

#define SENSOR_D2_14BIT_C	(float)0.01
#define SENSOR_D2_12BIT_C	(float)0.04

#define SENSOR_C1	(float)-2.0468
#define SENSOR_C2_12BIT	(float)0.0367
#define SENSOR_C2_8BIT	(float)0.5872
#define SENSOR_C3_12BIT	(float)-0.0000015955
#define SENSOR_C3_8BIT	(float)-0.0004084500

#define SENSOR_T1		(float)0.01
#define SENSOR_T2_12BIT (float)0.00008
#define SENSOR_T2_8BIT  (float)0.00128

#define SHT15_CLOCK_PIN		9u
#define SHT15_CLOCK_GPIO	GPIOB
#define SHT15_CLOCK_PORT	PORTB

#define SHT15_DATA_PIN		1u
#define SHT15_DATA_GPIO		GPIOA
#define SHT15_DATA_PORT		PORTA

void SHT15_Init(void);

ReturnCode_t SHT15_Read_Temperature(float *Temperature_Value);

ReturnCode_t SHT15_Read_Humidity(float *Humidity_Value);

ReturnCode_t SHT15_Read_Temp_Humi(float *Temperature_Value, float *Humidity_Value, float *Dew_Point);

ReturnCode_t SHT15_Get_Raw_Temperature(uint16_t *Raw_Temperature_Value);

ReturnCode_t SHT15_Get_Raw_Humidity(uint16_t *Raw_Humidity_Value);

#endif /* SHT15_H_ */
