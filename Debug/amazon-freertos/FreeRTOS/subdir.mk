################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../amazon-freertos/FreeRTOS/event_groups.c \
../amazon-freertos/FreeRTOS/list.c \
../amazon-freertos/FreeRTOS/queue.c \
../amazon-freertos/FreeRTOS/stream_buffer.c \
../amazon-freertos/FreeRTOS/tasks.c \
../amazon-freertos/FreeRTOS/timers.c 

OBJS += \
./amazon-freertos/FreeRTOS/event_groups.o \
./amazon-freertos/FreeRTOS/list.o \
./amazon-freertos/FreeRTOS/queue.o \
./amazon-freertos/FreeRTOS/stream_buffer.o \
./amazon-freertos/FreeRTOS/tasks.o \
./amazon-freertos/FreeRTOS/timers.o 

C_DEPS += \
./amazon-freertos/FreeRTOS/event_groups.d \
./amazon-freertos/FreeRTOS/list.d \
./amazon-freertos/FreeRTOS/queue.d \
./amazon-freertos/FreeRTOS/stream_buffer.d \
./amazon-freertos/FreeRTOS/tasks.d \
./amazon-freertos/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
amazon-freertos/FreeRTOS/%.o: ../amazon-freertos/FreeRTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DSDK_DEBUGCONSOLE=0 -DPRINTF_FLOAT_ENABLE=0 -DSDK_OS_FREE_RTOS -DFSL_RTOS_FREE_RTOS -DCPU_MK64FN1M0VLL12_cm4 -DCPU_MK64FN1M0VLL12 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I../board -I../source -I../ -I../amazon-freertos/FreeRTOS/portable -I../amazon-freertos/portable -I../amazon-freertos/include -I../drivers -I../CMSIS -I../drivers/freertos -I../utilities -I../startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


