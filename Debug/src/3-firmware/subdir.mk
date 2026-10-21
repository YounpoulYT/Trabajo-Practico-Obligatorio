################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/3-firmware/cr_cpp_config.cpp \
../src/3-firmware/cr_startup_lpc84x.cpp \
../src/3-firmware/dr_pll.cpp \
../src/3-firmware/swm.cpp 

C_SRCS += \
../src/3-firmware/crp.c \
../src/3-firmware/mtb.c 

S_SRCS += \
../src/3-firmware/aeabi_romdiv_patch.s 

CPP_DEPS += \
./src/3-firmware/cr_cpp_config.d \
./src/3-firmware/cr_startup_lpc84x.d \
./src/3-firmware/dr_pll.d \
./src/3-firmware/swm.d 

C_DEPS += \
./src/3-firmware/crp.d \
./src/3-firmware/mtb.d 

OBJS += \
./src/3-firmware/aeabi_romdiv_patch.o \
./src/3-firmware/cr_cpp_config.o \
./src/3-firmware/cr_startup_lpc84x.o \
./src/3-firmware/crp.o \
./src/3-firmware/dr_pll.o \
./src/3-firmware/mtb.o \
./src/3-firmware/swm.o 


# Each subdirectory must supply rules for building sources it contributes
src/3-firmware/%.o: ../src/3-firmware/%.s src/3-firmware/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__NEWLIB__ -DDEBUG -D__CODE_RED -DCORE_M0PLUS -D__USE_ROMDIVIDE -D__LPC84X__ -g3 -gdwarf-4 -mcpu=cortex-m0 -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/3-firmware/%.o: ../src/3-firmware/%.cpp src/3-firmware/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\01-pin\in" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\01-pin\out" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\02-gpio" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\04-PerifericosTemporizados" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\05-timer" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\06-EntradasDigitales" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\07-SalidasDigitales" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\2-aplicacion" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\3-firmware" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\3-firmware\systick" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\08-display_de_segmentos\09A-barrido\barrido-interfaz" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\08-display_de_segmentos\09A-barrido" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\08-display_de_segmentos\09B-segmentos\segmentos-interfaz" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\08-display_de_segmentos\09B-segmentos" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\08-display_de_segmentos\09C-display" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\09-teclado" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\10-adc" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\13-dac" -I"C:\Users\Juanp\OneDrive\Documentos\GitHub\Trabajo-Practico-Obligatorio\src\1-modulos\12-USART" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/3-firmware/%.o: ../src/3-firmware/%.c src/3-firmware/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M0PLUS -D__MTB_BUFFER_SIZE=256 -D__USE_ROMDIVIDE -DCPP_USE_HEAP -D__LPC84X__ -O0 -fno-common -g3 -gdwarf-4 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-3-2d-firmware

clean-src-2f-3-2d-firmware:
	-$(RM) ./src/3-firmware/aeabi_romdiv_patch.o ./src/3-firmware/cr_cpp_config.d ./src/3-firmware/cr_cpp_config.o ./src/3-firmware/cr_startup_lpc84x.d ./src/3-firmware/cr_startup_lpc84x.o ./src/3-firmware/crp.d ./src/3-firmware/crp.o ./src/3-firmware/dr_pll.d ./src/3-firmware/dr_pll.o ./src/3-firmware/mtb.d ./src/3-firmware/mtb.o ./src/3-firmware/swm.d ./src/3-firmware/swm.o

.PHONY: clean-src-2f-3-2d-firmware

