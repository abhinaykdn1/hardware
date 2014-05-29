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
pomodoro_clock.lss \

FLASH_IMAGE += \
pomodoro_clock.hex \

EEPROM_IMAGE += \
pomodoro_clock.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: pomodoro_clock.elf secondary-outputs

# Tool invocations
pomodoro_clock.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,pomodoro_clock.map -mmcu=atmega328p -o "pomodoro_clock.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

pomodoro_clock.lss: pomodoro_clock.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S pomodoro_clock.elf  >"pomodoro_clock.lss"
	@echo 'Finished building: $@'
	@echo ' '

pomodoro_clock.hex: pomodoro_clock.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex pomodoro_clock.elf  "pomodoro_clock.hex"
	@echo 'Finished building: $@'
	@echo ' '

pomodoro_clock.eep: pomodoro_clock.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex pomodoro_clock.elf  "pomodoro_clock.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: pomodoro_clock.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t pomodoro_clock.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) pomodoro_clock.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
