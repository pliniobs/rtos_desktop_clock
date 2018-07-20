#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

#include "sht15.h"
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

typedef struct
{
	float Temperature;
	float Humidity;
	float Dew_Point;
}Environment_Data_t;

static QueueHandle_t Sensor_Data_Queue = NULL;

static void Task_ADQ_Environment(void *pvParameters);
static void Task_Show_Data(void *pvParameters);

int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	PRINTF("Hello World\n");

	if (xTaskCreate(Task_ADQ_Environment, "Task_ADQ_Environment", configMINIMAL_STACK_SIZE + 10, NULL, (configMAX_PRIORITIES - 1), NULL) != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}

	if (xTaskCreate(Task_Show_Data, "Task_Show_Data", configMINIMAL_STACK_SIZE + 10, NULL, (configMAX_PRIORITIES - 1), NULL) != pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1);
	}
	vTaskStartScheduler();
	for (;;);
	return 0 ;
}

static void Task_Show_Data(void *pvParameters)
{
	Environment_Data_t Sensor_Data;

	while(1)
	{
		if(xQueueReceive(Sensor_Data_Queue, &Sensor_Data, 1000 ))
		{
			PRINTF("Temperature: %0.2f Celcius <-> Humidity: %0.2f%% <-> Dew Point: %0.2f Celcius\n\n",
					Sensor_Data.Temperature,
					Sensor_Data.Humidity,
					Sensor_Data.Dew_Point);
		}
	}
}

static void Task_ADQ_Environment(void *pvParameters)
{
	float Temperature_Celcius, Humidity_Celcius, Dew_Point;
	Environment_Data_t Sensor_Aquired_Data;

	SHT15_Init();

	Sensor_Data_Queue = xQueueCreate(5, sizeof(Environment_Data_t));

	while(1)
	{
		while(SHT15_Read_Temp_Humi(&Temperature_Celcius, &Humidity_Celcius, &Dew_Point) != ANSWERED_REQUEST);

		Sensor_Aquired_Data.Temperature = Temperature_Celcius;
		Sensor_Aquired_Data.Humidity = Humidity_Celcius;
		Sensor_Aquired_Data.Dew_Point = Dew_Point;

		if(xQueueSend(Sensor_Data_Queue, &Sensor_Aquired_Data, 10) != pdPASS)
			PRINTF("Error while writing data to the queue.\n");

		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
