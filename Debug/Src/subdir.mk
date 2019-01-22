################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/app_x-cube-ble1.c \
../Src/gatt_db.c \
../Src/hci_tl_interface.c \
../Src/main.c \
../Src/nucleo_f411re_bus.c \
../Src/sensor.c \
../Src/stm32f4_nucleo_f411re.c \
../Src/stm32f4xx_hal_exti.c \
../Src/stm32f4xx_hal_msp.c \
../Src/stm32f4xx_it.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/app_x-cube-ble1.o \
./Src/gatt_db.o \
./Src/hci_tl_interface.o \
./Src/main.o \
./Src/nucleo_f411re_bus.o \
./Src/sensor.o \
./Src/stm32f4_nucleo_f411re.o \
./Src/stm32f4xx_hal_exti.o \
./Src/stm32f4xx_hal_msp.o \
./Src/stm32f4xx_it.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/app_x-cube-ble1.d \
./Src/gatt_db.d \
./Src/hci_tl_interface.d \
./Src/main.d \
./Src/nucleo_f411re_bus.d \
./Src/sensor.d \
./Src/stm32f4_nucleo_f411re.d \
./Src/stm32f4xx_hal_exti.d \
./Src/stm32f4xx_hal_msp.d \
./Src/stm32f4xx_it.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F411xE -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Inc" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/includes" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/utils" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


