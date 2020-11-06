################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: periph_adc_with_vcom.axf

# Tool invocations
periph_adc_with_vcom.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\lpc_chip_15xx\Debug" -L"C:\Users\Bobasek\Documents\LPCXpresso_8.2.2_650\workspace\lpc_board_nxp_lpcxpresso_1549\Debug" -Xlinker -Map="periph_adc_with_vcom.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "periph_adc_Debug.ld" -o "periph_adc_with_vcom.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) periph_adc_with_vcom.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "periph_adc_with_vcom.axf"; # arm-none-eabi-objcopy -v -O binary "periph_adc_with_vcom.axf" "periph_adc_with_vcom.bin" ; # checksum -p LPC1549 -d "periph_adc_with_vcom.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
