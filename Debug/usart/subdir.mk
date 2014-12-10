################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usart/usart_btle.c \
../usart/usart_wan.c 

OBJS += \
./usart/usart_btle.o \
./usart/usart_wan.o 

C_DEPS += \
./usart/usart_btle.d \
./usart/usart_wan.d 


# Each subdirectory must supply rules for building sources it contributes
usart/%.o: ../usart/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega1284p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


