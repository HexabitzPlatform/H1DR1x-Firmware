################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/User/main.c 

OBJS += \
./User/main.o 

C_DEPS += \
./User/main.d 


# Each subdirectory must supply rules for building sources it contributes
User/main.o: D:/Hexabitz/for\ Release/Modules\ firmware/H1DR1x/User/main.c User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g1 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DH1DR1 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/LSM6DS3/inc -I../../Thirdparty/LSM303AGR/inc -I../../H1DR1 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"User/main.d" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

