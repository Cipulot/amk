
SRCS += $(KEYBOARD_DIR)/m65.c

MCU = STM32F722
#TINYUSB_ENABLE = yes
#TINYUSB_USE_HAL = yes
SCREEN_ENABLE = yes
MSC_ENABLE = yes
EECONFIG_FLASH = yes
#RGB_LINEAR_ENABLE = yes
DATETIME_ENABLE = yes
VIAL_ENABLE = yes
RTOS_ENABLE = yes
DYNAMIC_CONFIGURATION = yes

LINKER_PATH = $(KEYBOARD_DIR)

ifeq (yes, $(strip $(VIAL_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/m65_actionmap.c
else
	SRCS += $(KEYBOARD_DIR)/m65_keymap.c
endif
