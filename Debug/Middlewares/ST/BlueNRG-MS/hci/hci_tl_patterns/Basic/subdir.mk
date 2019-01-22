################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.c 

OBJS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.o 

C_DEPS += \
./Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/hci_tl.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/%.o: ../Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F411xE -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Inc" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/includes" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/hci/hci_tl_patterns/Basic" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Middlewares/ST/BlueNRG-MS/utils" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include" -I"/home/hosni/embedded_tools/stm32_eclipse_workspace/STM32F4Nucleo64-BLUENRG/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


