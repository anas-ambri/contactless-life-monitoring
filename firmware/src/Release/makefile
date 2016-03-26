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
all: contactless_monitor.axf

# Tool invocations
contactless_monitor.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\nxp\LPCXpresso_8.0.0_526\lpcxpresso\Examples\LPCOpen\lpcopen_2_12_lpcxpresso_ngx_xplorer_4330\lpc_chip_43xx\Release" -L"C:\nxp\LPCXpresso_8.0.0_526\lpcxpresso\Examples\LPCOpen\lpcopen_2_12_lpcxpresso_ngx_xplorer_4330\lpc_board_ngx_xplorer_4330\Release" -Xlinker -Map="contactless_monitor.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "usbd_rom_hid_generic_Release.ld" -o "contactless_monitor.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) contactless_monitor.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "contactless_monitor.axf"; # arm-none-eabi-objcopy -v -O binary "contactless_monitor.axf" "contactless_monitor.bin" ; # checksum -p LPC4330 -d "contactless_monitor.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
