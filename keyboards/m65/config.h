/**
 * config.h
 *
 */

#pragma once

// USB Device descriptor parameter
#define VENDOR_ID  0x4D58   // MX
#define PRODUCT_ID 0x6500   // M65 with screen
#define DEVICE_VER 0x0001
#define MANUFACTURER Matrix Lab
#define PRODUCT Matrix VITA

// key matrix
#define MATRIX_ROWS     5
#define MATRIX_COLS     15

#define MATRIX_ROW_PINS { A1, C2, C1, C4, B10}
#define MATRIX_COL_PINS { C9, A8, C8, C7, C6, B4, D2, C12, C11, C10, A15, B11, B2, B1, B0}

#define CAPS_LED_PIN    A10

#define SCREEN_NUM      1
#define SCREEN_0_PWR    C0
#define SCREEN_0_PWR_EN 0
#define SCREEN_0_CS     A2
#define SCREEN_0_RESET  A3
#define SCREEN_0_DC     A4

#define FLASH_CS        B12
#define W25QXX_SPI_ID   SPI_INSTANCE_2

// custom tusb definitions
#define CFG_TUSB_CONFIG_FILE    "tusb_def.h"
// custom lvgl definitions
#define LV_CONF_PATH            lvgl_def.h

#define VECT_TAB_OFFSET 0x20000

#define RGB_LED_NUM     4
#define WS2812_LED_PIN  A0
#define PWM_TIM_PERIOD  134
#define PWM_TIM htim2
#define PWM_TIM_CHANNEL TIM_CHANNEL_1

//#define SHARED_HID_EP
#define DYNAMIC_CONFIGURATION