
MCU = STM32F401
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
EECONFIG_FLASH = yes
VIAL_ENABLE = yes
NKRO_AUTO_ENABLE = yes

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

LINKER_PATH = $(KEYBOARD_DIR)

SRCS += $(KEYBOARD_DIR)/meta.c
SRCS += $(KEYBOARD_DIR)/meta_gcmap.c
SRCS += $(KEYBOARD_DIR)/uart_cmd.c

RGB_LINEAR_ENABLE = yes
RGB_INDICATOR_ENABLE = yes
APP_DEFS += -DMETA_HOTSWAP
