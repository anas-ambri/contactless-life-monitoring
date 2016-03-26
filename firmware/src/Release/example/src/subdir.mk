################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc43xx.c \
../example/src/libusbdev.c \
../example/src/libusbdev_desc.c \
../example/src/main.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cr_startup_lpc43xx.o \
./example/src/libusbdev.o \
./example/src/libusbdev_desc.o \
./example/src/main.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cr_startup_lpc43xx.d \
./example/src/libusbdev.d \
./example/src/libusbdev_desc.d \
./example/src/main.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DNDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M4 -D__MULTICORE_NONE -I"C:\nxp\LPCXpresso_8.0.0_526\lpcxpresso\Examples\LPCOpen\lpcopen_2_12_lpcxpresso_ngx_xplorer_4330\lpc_chip_43xx\inc" -I"C:\nxp\LPCXpresso_8.0.0_526\lpcxpresso\Examples\LPCOpen\lpcopen_2_12_lpcxpresso_ngx_xplorer_4330\lpc_board_ngx_xplorer_4330\inc" -I"C:\Users\Team04\Documents\contactless-life-monitoring\firmware\src\example\inc" -I"C:\nxp\LPCXpresso_8.0.0_526\lpcxpresso\Examples\LPCOpen\lpcopen_2_12_lpcxpresso_ngx_xplorer_4330\lpc_chip_43xx\inc\usbd" -Os -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


