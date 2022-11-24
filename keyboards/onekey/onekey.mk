
#MCU = STM32L072 
#MCU = STM32F446
#MCU = STM32F103
#MCU = STM32F405
#MCU = STM32F411 
#MCU = STM32F722
#MCU = STM32L432
#MCU = ATSAMD21
#MCU = NUC126 
#MCU = HC32F460
#MCU = GD32E103
#MCU = STM32F401
#MCU = STM32G431
#MCU = GD32E505
#MCU = STM32L452
MCU = STM32F412
#MCU = STM32L432
#MCU = M484
#USB_HOST_ENABLE = yes
#USB_DEVICE_ENABLE = no
#EECONFIG_FLASH = yes
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
#SCREEN_ENABLE = yes
#RGB_LINEAR_ENABLE = yes
#RGB_INDICATOR_ENABLE = yes

#RTOS_ENABLE = yes

#LINKER_PATH = $(KEYBOARD_DIR)

MSC_ENABLE = yes
DYNAMIC_CONFIGURATION = yes
QSPI_ENABLE = yes

SRCS += $(KEYBOARD_DIR)/onekey.c
SRCS += $(KEYBOARD_DIR)/onekey_keymap.c

#SRCS += $(KEYBOARD_DIR)/display.c
#SRCS += $(MAIN_DIR)/drivers/gc9107.c

SRCS +=	$(KEYBOARD_DIR)/display_8xv3.c
SRCS += $(MAIN_DIR)/drivers/st7735.c

SRCS += $(MAIN_DIR)/drivers/spi.c
SRCS += $(MAIN_DIR)/drivers/w25q_mem.c