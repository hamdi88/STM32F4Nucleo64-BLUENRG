################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/home/hosni/STM32Cube/ST_Nucleo_f411/SW4STM32/startup_stm32f411xe.s 

C_SRCS += \
/home/hosni/STM32Cube/ST_Nucleo_f411/SW4STM32/syscalls.c 

OBJS += \
./Application/SW4STM32/startup_stm32f411xe.o \
./Application/SW4STM32/syscalls.o 

C_DEPS += \
./Application/SW4STM32/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f411xe.o: /home/hosni/STM32Cube/ST_Nucleo_f411/SW4STM32/startup_stm32f411xe.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/SW4STM32/syscalls.o: /home/hosni/STM32Cube/ST_Nucleo_f411/SW4STM32/syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F411xE -I"/home/hosni/STM32Cube/ST_Nucleo_f411/Inc" -I"/home/hosni/STM32Cube/ST_Nucleo_f411/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/hosni/STM32Cube/ST_Nucleo_f411/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/home/hosni/STM32Cube/ST_Nucleo_f411/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/hosni/STM32Cube/ST_Nucleo_f411/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


