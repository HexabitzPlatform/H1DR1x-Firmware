################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/startup_stm32f091xc.s 

C_SRCS += \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_dma.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_gpio.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_it.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_rtc.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_timers.c \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_uart.c 

OBJS += \
./H1DR1/H1DR1.o \
./H1DR1/H1DR1_dma.o \
./H1DR1/H1DR1_gpio.o \
./H1DR1/H1DR1_it.o \
./H1DR1/H1DR1_rtc.o \
./H1DR1/H1DR1_timers.o \
./H1DR1/H1DR1_uart.o \
./H1DR1/startup_stm32f091xc.o 

S_DEPS += \
./H1DR1/startup_stm32f091xc.d 

C_DEPS += \
./H1DR1/H1DR1.d \
./H1DR1/H1DR1_dma.d \
./H1DR1/H1DR1_gpio.d \
./H1DR1/H1DR1_it.d \
./H1DR1/H1DR1_rtc.d \
./H1DR1/H1DR1_timers.d \
./H1DR1/H1DR1_uart.d 


# Each subdirectory must supply rules for building sources it contributes
H1DR1/H1DR1.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_dma.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_dma.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_dma.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_gpio.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_gpio.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_gpio.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_it.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_it.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_it.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_rtc.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_rtc.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_rtc.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_timers.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_timers.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_timers.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/H1DR1_uart.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/H1DR1_uart.c H1DR1/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H1DR1/H1DR1_uart.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"
H1DR1/startup_stm32f091xc.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/H1DR1/startup_stm32f091xc.s H1DR1/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0 -g3 -c -x assembler-with-cpp -MMD -MP -MF"H1DR1/startup_stm32f091xc.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@" "$<"

