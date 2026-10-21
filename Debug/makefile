################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/3-firmware/systick/subdir.mk
-include src/3-firmware/subdir.mk
-include src/2-aplicacion/subdir.mk
-include src/1-modulos/14-SPI/subdir.mk
-include src/1-modulos/13-dac/subdir.mk
-include src/1-modulos/12-USART/subdir.mk
-include src/1-modulos/10-adc/subdir.mk
-include src/1-modulos/09-teclado/subdir.mk
-include src/1-modulos/08-display_de_segmentos/09C-display/subdir.mk
-include src/1-modulos/08-display_de_segmentos/09B-segmentos/subdir.mk
-include src/1-modulos/08-display_de_segmentos/09A-barrido/subdir.mk
-include src/1-modulos/07-SalidasDigitales/subdir.mk
-include src/1-modulos/06-EntradasDigitales/subdir.mk
-include src/1-modulos/05-timer/subdir.mk
-include src/1-modulos/04-PerifericosTemporizados/subdir.mk
-include src/1-modulos/03-intext/subdir.mk
-include src/1-modulos/02-gpio/subdir.mk
-include src/1-modulos/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Trabajo-Practico-Obligatorio
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: Trabajo-Practico-Obligatorio.axf

# Tool invocations
Trabajo-Practico-Obligatorio.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -Xlinker -Map="Trabajo-Practico-Obligatorio.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m0 -mthumb -T R2002_Debug.ld -o "Trabajo-Practico-Obligatorio.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Trabajo-Practico-Obligatorio.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Trabajo-Practico-Obligatorio.axf"; # arm-none-eabi-objcopy -v -O binary "Trabajo-Practico-Obligatorio.axf" "Trabajo-Practico-Obligatorio.bin" ; # checksum -p LPC845 -d "Trabajo-Practico-Obligatorio.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
