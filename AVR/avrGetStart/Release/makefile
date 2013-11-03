################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
avrGetStart.lss \

FLASH_IMAGE += \
avrGetStart.hex \

EEPROM_IMAGE += \
avrGetStart.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: avrGetStart.elf secondary-outputs

# Tool invocations
avrGetStart.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,avrGetStart.map -mmcu=atmega48p -o "avrGetStart.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

avrGetStart.lss: avrGetStart.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S avrGetStart.elf  >"avrGetStart.lss"
	@echo 'Finished building: $@'
	@echo ' '

avrGetStart.hex: avrGetStart.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex avrGetStart.elf  "avrGetStart.hex"
	@echo 'Finished building: $@'
	@echo ' '

avrGetStart.eep: avrGetStart.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex avrGetStart.elf  "avrGetStart.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: avrGetStart.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega48p avrGetStart.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) avrGetStart.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
