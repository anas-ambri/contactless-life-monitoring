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
all: usbd_rom_hid_generic.axf

# Tool invocations
usbd_rom_hid_generic.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\lpc_chip_43xx\Debug" -L"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\lpc_board_ngx_xplorer_4330\Debug" -Xlinker -Map="usbd_rom_hid_generic.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "usbd_rom_hid_generic_Debug.ld" -o "usbd_rom_hid_generic.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) usbd_rom_hid_generic.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "usbd_rom_hid_generic.axf"; # arm-none-eabi-objcopy -v -O binary "usbd_rom_hid_generic.axf" "usbd_rom_hid_generic.bin" ; # checksum -p LPC4330 -d "usbd_rom_hid_generic.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
