################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/ADXL345.c \
../Src/Communication.c \
../Src/accel_control.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/main.c \
../Src/stm32l4xx_hal_msp.c \
../Src/stm32l4xx_it.c \
../Src/system_stm32l4xx.c \
../Src/usart.c 

OBJS += \
./Src/ADXL345.o \
./Src/Communication.o \
./Src/accel_control.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/stm32l4xx_hal_msp.o \
./Src/stm32l4xx_it.o \
./Src/system_stm32l4xx.o \
./Src/usart.o 

C_DEPS += \
./Src/ADXL345.d \
./Src/Communication.d \
./Src/accel_control.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/main.d \
./Src/stm32l4xx_hal_msp.d \
./Src/stm32l4xx_it.d \
./Src/system_stm32l4xx.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32L476xx -I"C:/Users/gyooo/OneDrive/vandyBackup/shooting/hackathon_accel_base/Inc" -I"C:/Users/gyooo/OneDrive/vandyBackup/shooting/hackathon_accel_base/Drivers/STM32L4xx_HAL_Driver/Inc" -I"C:/Users/gyooo/OneDrive/vandyBackup/shooting/hackathon_accel_base/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/gyooo/OneDrive/vandyBackup/shooting/hackathon_accel_base/Drivers/CMSIS/Device/ST/STM32L4xx/Include" -I"C:/Users/gyooo/OneDrive/vandyBackup/shooting/hackathon_accel_base/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


