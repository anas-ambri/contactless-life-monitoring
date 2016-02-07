################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc43xx.c \
../example/src/hid_desc.c \
../example/src/hid_generic.c \
../example/src/hid_main.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cr_startup_lpc43xx.o \
./example/src/hid_desc.o \
./example/src/hid_generic.o \
./example/src/hid_main.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cr_startup_lpc43xx.d \
./example/src/hid_desc.d \
./example/src/hid_generic.d \
./example/src/hid_main.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M4 -D__MULTICORE_NONE -I"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\lpc_chip_43xx\inc" -I"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\lpc_board_ngx_xplorer_4330\inc" -I"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\usbd_rom_hid_generic\example\inc" -I"C:\Users\Team04\Documents\LPCXpresso_8.0.0_526\workspace\lpc_chip_43xx\inc\usbd" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


