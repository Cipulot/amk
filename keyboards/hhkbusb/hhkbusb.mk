
SRCS += $(wildcard $(KEYBOARD_DIR)/*.c)
#APP_DEFS += -DABELX
#APP_DEFS += -DADD
APP_DEFS += -DFALCON

MCU = STM32F103
#MCU = STM32L072
CUSTOM_MATRIX = yes
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
VIAL_ENABLE = no 
ACTIONMAP_ENABLE = yes

LINKER_PATH = $(KEYBOARD_DIR)