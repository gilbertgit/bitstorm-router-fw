################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../btle/btle.c \
../btle/btle_driver.c 

OBJS += \
./btle/btle.o \
./btle/btle_driver.o 

C_DEPS += \
./btle/btle.d \
./btle/btle_driver.d 


# Each subdirectory must supply rules for building sources it contributes
btle/%.o: ../btle/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega1284p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


