################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../queue/circular_queue.c \
../queue/queue.c 

OBJS += \
./queue/circular_queue.o \
./queue/queue.o 

C_DEPS += \
./queue/circular_queue.d \
./queue/queue.d 


# Each subdirectory must supply rules for building sources it contributes
queue/%.o: ../queue/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -DZB_ACK -UBYPASS_MODE -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega1284p -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


