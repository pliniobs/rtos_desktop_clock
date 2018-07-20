################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/freertos/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include amazon-freertos/portable/subdir.mk
-include amazon-freertos/FreeRTOS/portable/subdir.mk
-include amazon-freertos/FreeRTOS/subdir.mk
-include CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: RTOS_Desktop_Clock.axf

# Tool invocations
RTOS_Desktop_Clock.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="RTOS_Desktop_Clock.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T RTOS_Desktop_Clock_Debug.ld -o "RTOS_Desktop_Clock.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) RTOS_Desktop_Clock.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "RTOS_Desktop_Clock.axf"; # arm-none-eabi-objcopy -v -O binary "RTOS_Desktop_Clock.axf" "RTOS_Desktop_Clock.bin" ; # checksum -p MK64FN1M0xxx12 -d "RTOS_Desktop_Clock.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
