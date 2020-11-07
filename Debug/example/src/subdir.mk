################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/adc.c \
../example/src/cr_startup_lpc15xx.c \
../example/src/hid_desc.c \
../example/src/hid_mouse.c \
../example/src/sysinit.c 

OBJS += \
./example/src/adc.o \
./example/src/cr_startup_lpc15xx.o \
./example/src/hid_desc.o \
./example/src/hid_mouse.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/adc.d \
./example/src/cr_startup_lpc15xx.d \
./example/src/hid_desc.d \
./example/src/hid_mouse.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -D__REDLIB__ -DCORE_M3 -I"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\lpc_chip_15xx\inc" -I"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\lpc_chip_15xx\inc\usbd" -I"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\periph_adc_with_vcom\example\inc" -I"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\lpc_board_nxp_lpcxpresso_1549\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


