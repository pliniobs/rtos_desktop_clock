/*
 * sht15.c
 *
 *  Created on: 18 de jul de 2018
 *      Author: plinio
 */

#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "sht15.h"
#include "returncode.h"
#include "math.h"

const gpio_pin_config_t Sensor_Pin_Output_Mode = {kGPIO_DigitalOutput, 0};
const gpio_pin_config_t Sensor_Pin_Input_Mode = {kGPIO_DigitalInput, 0};

void SHT15_Make_Data_Pin_Output(void);
void SHT15_Make_Data_Pin_Input(void);
uint8_t SHT15_Shift_Out(uint8_t Shift_Out_Data);
uint8_t SHT15_Shift_In(uint8_t ACK_Value);
uint8_t SHT15_Get_Sensor_Status(void);
void SHT15_Generate_Start(void);

void SHT15_Init(void)
{
	CLOCK_EnableClock(kCLOCK_PortA);                           /* Port D Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB);                           /* Port A Clock Gate Control: Clock enabled */

	PORT_SetPinMux(SHT15_CLOCK_PORT, SHT15_CLOCK_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(SHT15_DATA_PORT, SHT15_DATA_PIN, kPORT_MuxAsGpio);

	GPIO_PinInit(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, &Sensor_Pin_Output_Mode);
	GPIO_PinInit(SHT15_DATA_GPIO, SHT15_DATA_PIN, &Sensor_Pin_Output_Mode);

	SHT15_Make_Data_Pin_Output();
}

ReturnCode_t SHT15_Read_Temp_Humi(float *Temperature_Value, float *Humidity_Value, float *Dew_Point)
{
	static enum
	{
		Read_Temperature,
		Read_Humidity,
		Calculate_Data,
	} SHT15_Read_Temp_Humi_State_Machine = Read_Temperature;

	ReturnCode_t Ret_Code = OPERATION_RUNNING;
	ReturnCode_t Operation_Ret_Code;

	float Temperature, Humidity, Humidity_Compensated;
	uint16_t Raw_Temperature, Raw_Humidity;

	switch (SHT15_Read_Temp_Humi_State_Machine)
	{
	case Read_Temperature:
		Operation_Ret_Code = SHT15_Get_Raw_Temperature(&Raw_Temperature);
		Ret_Code = OPERATION_RUNNING;
		if(Operation_Ret_Code == ANSWERED_REQUEST)
		{
			SHT15_Read_Temp_Humi_State_Machine = Read_Humidity;
		}else if(Operation_Ret_Code >= RETURN_ERROR_VALUE)
		{
			SHT15_Read_Temp_Humi_State_Machine = Read_Temperature;
			Ret_Code = ERR_DEVICE;
		}
		break;
	case Read_Humidity:
		Operation_Ret_Code = SHT15_Get_Raw_Humidity(&Raw_Humidity);
		Ret_Code = OPERATION_RUNNING;
		if(Operation_Ret_Code == ANSWERED_REQUEST)
		{
			SHT15_Read_Temp_Humi_State_Machine = Calculate_Data;
		}else if(Operation_Ret_Code >= RETURN_ERROR_VALUE)
		{
			SHT15_Read_Temp_Humi_State_Machine = Read_Temperature;
			Ret_Code = ERR_DEVICE;
		}
		break;

	case Calculate_Data:
		Temperature = SENSOR_D1_5V_C + (SENSOR_D2_14BIT_C * Raw_Temperature);
		Humidity = SENSOR_C1
						+ (SENSOR_C2_12BIT * Raw_Humidity)
						+ (SENSOR_C3_12BIT * Raw_Humidity * Raw_Humidity);

		Humidity_Compensated = ((Temperature - 25.0)*(SENSOR_T1 + (SENSOR_T2_12BIT * Raw_Humidity))) + Humidity;

		*Dew_Point = pow((Humidity_Compensated/100.0), 0.125) * (112.0 + (0.9*Temperature))
				+ (0.1 * Temperature)
				- 112.0;
		*Humidity_Value = Humidity_Compensated;
		*Temperature_Value = Temperature;
		SHT15_Read_Temp_Humi_State_Machine = Read_Temperature;
		Ret_Code = ANSWERED_REQUEST;
		break;

	}

	return Ret_Code;
}

ReturnCode_t SHT15_Get_Raw_Temperature(uint16_t *Raw_Temperature_Value)
{
	static enum
	{
		Generate_Start_Convertion,
		Wait_Convertion,
		Get_Response,
	} SHT15_Read_Temperature_State_Machine = Generate_Start_Convertion;

	ReturnCode_t Ret_Code = OPERATION_RUNNING;
	uint8_t Sensor_Data[2];
	static uint32_t Start_Time_Stamp, Actual_Time_Stamp;
	uint16_t Sensor_Output;

	switch (SHT15_Read_Temperature_State_Machine)
	{
	case Generate_Start_Convertion:
		SHT15_Generate_Start();
		SHT15_Shift_Out(SHT15_TEMPERATURE_READ_INSTRUCTION);
		Start_Time_Stamp = 0;//HAL_GetTick();
		SHT15_Read_Temperature_State_Machine = Wait_Convertion;

		Ret_Code = OPERATION_RUNNING;
		break;
	case Wait_Convertion:
		Ret_Code = OPERATION_RUNNING;
		Actual_Time_Stamp = 0;//HAL_GetTick();
		if((Actual_Time_Stamp - Start_Time_Stamp) > 300)
		{
			SHT15_Read_Temperature_State_Machine = Generate_Start_Convertion;
			Ret_Code = ERR_DEVICE;
		}

		if(SHT15_Get_Sensor_Status() != SHT15_SENSOR_BUSY)
		{
			SHT15_Read_Temperature_State_Machine = Get_Response;
		}
		break;
	case Get_Response:
		Sensor_Data[0] = SHT15_Shift_In(SHT15_ACK);
		Sensor_Data[1] = SHT15_Shift_In(SHT15_NACK);
		Sensor_Output = (Sensor_Data[0] << 8) | Sensor_Data[1];

		*Raw_Temperature_Value = Sensor_Output;

		SHT15_Read_Temperature_State_Machine = Generate_Start_Convertion;
		Ret_Code = ANSWERED_REQUEST;
		break;
	}

	return Ret_Code;
}

ReturnCode_t SHT15_Get_Raw_Humidity(uint16_t *Raw_Humidity_Value)
{
	static enum
	{
		Generate_Start_Convertion,
		Wait_Convertion,
		Get_Response,
	} SHT15_Read_Humidity_State_Machine = Generate_Start_Convertion;

	ReturnCode_t Ret_Code = OPERATION_RUNNING;
	uint8_t Sensor_Data[2];
	static uint32_t Start_Time_Stamp, Actual_Time_Stamp;
	uint16_t Sensor_Output;

	switch (SHT15_Read_Humidity_State_Machine)
	{
	case Generate_Start_Convertion:
		SHT15_Generate_Start();
		SHT15_Shift_Out(SHT15_HUMIDITY_READ_INSTRUCTION);
		Start_Time_Stamp = 0;//HAL_GetTick();
		SHT15_Read_Humidity_State_Machine = Wait_Convertion;

		Ret_Code = OPERATION_RUNNING;
		break;
	case Wait_Convertion:
		Ret_Code = OPERATION_RUNNING;

		Actual_Time_Stamp = 0;//HAL_GetTick();
		if((Actual_Time_Stamp - Start_Time_Stamp) > 300)
		{
			SHT15_Read_Humidity_State_Machine = Generate_Start_Convertion;
			Ret_Code = ERR_DEVICE;
		}

		if(SHT15_Get_Sensor_Status() != SHT15_SENSOR_BUSY)
		{
			SHT15_Read_Humidity_State_Machine = Get_Response;
		}
		break;
	case Get_Response:
		Sensor_Data[0] = SHT15_Shift_In(SHT15_ACK);
		Sensor_Data[1] = SHT15_Shift_In(SHT15_NACK);
		Sensor_Output = (Sensor_Data[0] << 8) | Sensor_Data[1];
		*Raw_Humidity_Value = Sensor_Output;
		SHT15_Read_Humidity_State_Machine = Generate_Start_Convertion;
		Ret_Code = ANSWERED_REQUEST;
		break;
	}

	return Ret_Code;
}

ReturnCode_t SHT15_Read_Temperature(float *Temperature_Value)
{
	ReturnCode_t Ret_Code;
	ReturnCode_t Operation_Ret_Code;

	uint16_t Raw_Temperature_Value;
	float Temp_Value;

	Ret_Code = OPERATION_RUNNING;
	Operation_Ret_Code = SHT15_Get_Raw_Temperature(&Raw_Temperature_Value);
	if(Operation_Ret_Code == ANSWERED_REQUEST)
	{
		Temp_Value = SENSOR_D1_5V_C + (SENSOR_D2_14BIT_C * Raw_Temperature_Value);
		*Temperature_Value = Temp_Value;
		Ret_Code = ANSWERED_REQUEST;
	}else if(Operation_Ret_Code >= RETURN_ERROR_VALUE)
	{
		Ret_Code = ERR_DEVICE;
	}

	return Ret_Code;
}

ReturnCode_t SHT15_Read_Humidity(float *Humidity_Value)
{
	ReturnCode_t Ret_Code;
	ReturnCode_t Operation_Ret_Code;

	uint16_t Raw_Humidity_Value;
	float Humi_Value;

	Ret_Code = OPERATION_RUNNING;
	Operation_Ret_Code = SHT15_Get_Raw_Humidity(&Raw_Humidity_Value);
	if(Operation_Ret_Code == ANSWERED_REQUEST)
	{
		Humi_Value = SENSOR_C1
				+ (SENSOR_C2_12BIT * Raw_Humidity_Value)
				+ (SENSOR_C3_12BIT * Raw_Humidity_Value * Raw_Humidity_Value);

		*Humidity_Value = Humi_Value;
		Ret_Code = ANSWERED_REQUEST;
	}else if(Operation_Ret_Code >= RETURN_ERROR_VALUE)
	{
		Ret_Code = ERR_DEVICE;
	}

	return Ret_Code;
}

void SHT15_Generate_Start(void)
{
	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);
	GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 1);

	GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 0);
	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);

	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);
	GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 1);

	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);
}

uint8_t SHT15_Shift_Out(uint8_t Shift_Out_Data)
{
	uint8_t Bit_Counter;
	uint8_t ACK_Value;

	SHT15_Make_Data_Pin_Output();

	for (Bit_Counter = 0; Bit_Counter < 8; ++Bit_Counter) {
		if(Shift_Out_Data & 0x80)
			GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 1);
		else
			GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 0);

		GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);
		GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);

		Shift_Out_Data <<= 1;
	}

	SHT15_Make_Data_Pin_Input();

	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);

	if(GPIO_ReadPinInput(SHT15_DATA_GPIO, SHT15_DATA_PIN) == 1)
		ACK_Value = SHT15_ACK;
	else
		ACK_Value = SHT15_NACK;

	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);

	return ACK_Value;
}

uint8_t SHT15_Shift_In(uint8_t ACK_Value)
{
	uint8_t Bit_Counter;
	uint8_t Input_Data;

	SHT15_Make_Data_Pin_Input();

	Input_Data = 0;

	for (Bit_Counter = 0; Bit_Counter < 8; ++Bit_Counter) {

		Input_Data <<= 1;

		if(GPIO_ReadPinInput(SHT15_DATA_GPIO, SHT15_DATA_PIN) == 1)
			Input_Data |= 0x01;


		GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);
		GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);
	}

	SHT15_Make_Data_Pin_Output();

	if(ACK_Value == SHT15_ACK)
		GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 0);
	else
		GPIO_PinWrite(SHT15_DATA_GPIO, SHT15_DATA_PIN, 1);



	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 1);
	GPIO_PinWrite(SHT15_CLOCK_GPIO, SHT15_CLOCK_PIN, 0);

	return Input_Data;
}

uint8_t SHT15_Get_Sensor_Status(void)
{
	uint8_t Sensor_Status;
	SHT15_Make_Data_Pin_Input();

	if(GPIO_ReadPinInput(SHT15_DATA_GPIO, SHT15_DATA_PIN) == 1)
		Sensor_Status = SHT15_SENSOR_BUSY;
	else
		Sensor_Status = SHT15_SENSOR_READY;

	return Sensor_Status;
}

void SHT15_Make_Data_Pin_Output(void)
{
	GPIO_PinInit(SHT15_DATA_GPIO, SHT15_DATA_PIN, &Sensor_Pin_Output_Mode);
}

void SHT15_Make_Data_Pin_Input(void)
{
	GPIO_PinInit(SHT15_DATA_GPIO, SHT15_DATA_PIN, &Sensor_Pin_Input_Mode);
}
