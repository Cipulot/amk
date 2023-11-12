/**
 * @file at32f405.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "amk_hal.h"
#include "cm_misc.h"
#include "wait.h"

#include "tusb.h"

// if want to USE USB_HS, the HEXT MUST be 12000000

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = (hext * pll_ns)/(pll_ms * pll_fp)
  *         system clock source   = pll (hext)
  *         - hext                = HEXT_VALUE
  *         - sclk                = 216000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 216000000
  *         - apb2div             = 1
  *         - apb2clk             = 216000000
  *         - apb1div             = 2
  *         - apb1clk             = 108000000
  *         - pll_ns              = 72
  *         - pll_ms              = 1
  *         - pll_fr              = 4
  *         - flash_wtcyc         = 6 cycle
  * @param  none
  * @retval none
  */
void system_clock_init(void)
{
    /* reset crm */
    crm_reset();

    /* config flash psr register */
    flash_psr_set(FLASH_WAIT_CYCLE_6);

    /* enable pwc periph clock */
    crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

    /* set power ldo output voltage to 1.3v */
    pwc_ldo_output_voltage_set(PWC_LDO_OUTPUT_1V3);

    crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

    /* wait till hext is ready */
    while(crm_hext_stable_wait() == ERROR)
    {
    }

    /* if pll parameter has changed, please use the AT32_New_Clock_Configuration tool for new configuration. */
    crm_pll_config(CRM_PLL_SOURCE_HEXT, 72, 1, CRM_PLL_FP_4);

    /* config pllu div */
    crm_pllu_div_set(CRM_PLL_FU_18);

    /* enable pll */
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

    /* wait till pll is ready */
    while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
    {
    }

    /* config ahbclk */
    crm_ahb_div_set(CRM_AHB_DIV_1);

    /* config apb2clk, the maximum frequency of APB2 clock is 216 MHz */
    crm_apb2_div_set(CRM_APB2_DIV_1);

    /* config apb1clk, the maximum frequency of APB1 clock is 120 MHz */
    crm_apb1_div_set(CRM_APB1_DIV_2);

    /* enable auto step mode */
    crm_auto_step_mode_enable(TRUE);

    /* select pll as system clock source */
    crm_sysclk_switch(CRM_SCLK_PLL);

    /* wait till pll is used as system clock source */
    while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
    {
    }

    /* disable auto step mode */
    crm_auto_step_mode_enable(FALSE);

    /* update system_core_clock global variable */
    system_core_clock_update();
}


static void usb_custom_init(void)
{
    /* enable otg clock */
    crm_periph_clock_enable(CRM_OTGHS_PERIPH_CLOCK, TRUE);

    /* select usb 48m clcok source */
    //usb_clock48m_select(USB_CLK_HEXT);

    /* enable otg irq */
    nvic_irq_enable(OTGHS_IRQn, 0, 0);
}

void custom_board_init(void)
{
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
    usb_custom_init();
}

void custom_board_task(void)
{}

void OTGHS_IRQHandler(void)
{
    tud_int_handler(0);
}