
HHKBBLE_DIR := $(TOP_DIR)/keyboards/hhkbble

SRC_FILES += \
	$(HHKBBLE_DIR)/hhkbble.c \
	$(HHKBBLE_DIR)/hhkbble_keymap.c \

INC_FOLDERS += \
	$(HHKBBLE_DIR) \

MCU = NRF52832
BOOTMAGIC_ENABLE = yes
MOUSEKEY_ENABLE = yes
EXTRAKEY_ENABLE = yes