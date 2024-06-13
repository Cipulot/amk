/**
 * @file amk_apc.c
 * @author astro
 * 
 * @copyright Copyright (c) 2023
 */

#include "amk_apc.h"
#include "amk_dks.h"
#include "amk_store.h"

#include "quantum.h"

#include "amk_gpio.h"
#include "amk_utils.h"
#include "amk_printf.h"

#ifndef APC_DEBUG
#define APC_DEBUG 1
#endif

#if APC_DEBUG
#define apcrt_debug  amk_printf
#else
#define apcrt_debug(...)
#endif

#ifdef EC_MATRIX_ENABLE
#ifndef APC_KEY_MIN
#define APC_KEY_MIN             0
#endif

#ifndef APC_KEY_MAX
#define APC_KEY_MAX             4096
#endif

#ifndef APC_KEY_MIN_PRESET
#define APC_KEY_MIN_PRESET      400
#endif

#ifndef APC_KEY_MAX_PRESET
#define APC_KEY_MAX_PRESET      2800
#endif

#ifndef APC_INTERVAL_PRESET
#define APC_INTERVAL_PRESET     2400 
#endif

#ifndef APC_INTERVAL_INVALID
#define APC_INTERVAL_INVALID    0
#endif

#ifndef AMK_RT_SENS_DEFAULT
#define AMK_RT_SENS_DEFAULT     50
#endif

#ifndef AMK_TOP_SENS_DEFAULT
#define AMK_TOP_SENS_DEFAULT    100
#endif

#ifndef AMK_BTM_SENS_DEFAULT
#define AMK_BTM_SENS_DEFAULT    100
#endif

#ifndef AMK_APC_SENS_DEFAULT
#define AMK_APC_SENS_DEFAULT    100
#endif

#ifndef AMK_APC_NOISE_DEFAULT
#define AMK_APC_NOISE_DEFAULT   100
#endif

#ifndef APC_KEY_COUNT
#define APC_KEY_COUNT           5
#endif

#define TABLE_MAX   4000
static uint32_t apc_rt_table[] //SECTION_CCMRAM 
= {
0,
100,
200,
300,
400,
500,
600,
700,
800,
900,
1000,
1100,
1200,
1300,
1400,
1500,
1600,
1700,
1800,
1900,
2000,
2100,
2200,
2300,
2400,
2500,
2600,
2700,
2800,
2900,
3000,
3100,
3200,
3300,
3400,
3500,
3600,
3700,
3800,
3900,
4000,
};

#else
#ifndef APC_KEY_MIN
#define APC_KEY_MIN             2048
#endif

#ifndef APC_KEY_MAX
#define APC_KEY_MAX             4096
#endif

#ifndef APC_KEY_MIN_PRESET
#define APC_KEY_MIN_PRESET      2200
#endif

#ifndef APC_KEY_MAX_PRESET
#define APC_KEY_MAX_PRESET      3600
#endif

#ifndef APC_INTERVAL_MAX
#define APC_INTERVAL_MAX        2048
#endif

#ifndef APC_INTERVAL_PRESET
#define APC_INTERVAL_PRESET     1300
#endif

#ifndef APC_INTERVAL_INVALID
#define APC_INTERVAL_INVALID    0
#endif

#ifndef AMK_RT_SENS_DEFAULT
#define AMK_RT_SENS_DEFAULT     100
#endif

#ifndef AMK_TOP_SENS_DEFAULT
#define AMK_TOP_SENS_DEFAULT    100
#endif

#ifndef AMK_BTM_SENS_DEFAULT
#define AMK_BTM_SENS_DEFAULT    150
#endif

#ifndef AMK_APC_SENS_DEFAULT
#define AMK_APC_SENS_DEFAULT     80
#endif

#ifndef AMK_APC_NOISE_DEFAULT
#define AMK_APC_NOISE_DEFAULT   50
#endif

#ifndef APC_KEY_COUNT
#define APC_KEY_COUNT           5
#endif

#define TABLE_MAX   4000
static uint32_t apc_rt_table[] //SECTION_CCMRAM
= {
0,
36,
73,
109,
145,
182,
218,
255,
291,
327,
364,
418,
473,
527,
582,
636,
691,
745,
800,
855,
909,
1036,
1164,
1291,
1418,
1545,
1673,
1800,
1927,
2055,
2182,
2364,
2545,
2727,
2909,
3091,
3273,
3455,
3636,
3818,
4000,
};
#endif

#define APC_POLE_MAX            4096

#define APC_KEY_STATE_DEFAULT   APC_KEY_OFF
#define APC_KEY_MIN_DEFAULT     APC_KEY_MAX
#define APC_KEY_MAX_DEFAULT     APC_KEY_MIN
#define APC_KEY_LAST_DEFAULT    APC_KEY_MAX

//#define APC_KEY_TRIGGER_DEFAULT 0
//#define APC_KEY_DOWN_DEFAULT    1024
//#define APC_KEY_UP_DEFAULT      0


uint8_t amk_magnetive_pole = 0;
uint8_t amk_apcrt_profile = 0;
uint8_t amk_dks_disable = 0;

static uint32_t amk_rt_sens = AMK_RT_SENS_DEFAULT;
static uint32_t amk_top_sens = AMK_TOP_SENS_DEFAULT;
static uint32_t amk_btm_sens = AMK_BTM_SENS_DEFAULT;
static uint32_t amk_apc_sens = AMK_APC_SENS_DEFAULT;
static uint32_t amk_noise_sens = AMK_APC_NOISE_DEFAULT;

enum apc_key_state
{
    APC_KEY_OFF,
    APC_KEY_ON,
};

struct apc_key
{
    uint32_t state;
    uint32_t min;
    uint32_t max;
    uint32_t last;
    uint32_t apc;
    uint32_t rt_up;
    uint32_t rt_down;
    bool rt_cont;
    //bool dks_disable;
    uint32_t count;
    uint32_t value;
    bool active;
};


#ifdef MATRIX_NO_KEY_MASK
static matrix_row_t nokey_mask[] = MATRIX_NO_KEY_MASK;
#endif

static uint32_t get_rt_sense(uint32_t row, uint32_t col)
{
    uint32_t rt = amk_rt_sens;
#ifdef APC_SPECIAL_KEY_NUM
    for (int i = 0; i < APC_SPECIAL_KEY_NUM; i++) {
        struct apc_special_key* key = &apc_special_keys[i];
        if (key->row == row && key->col == col) {
            rt = ((amk_rt_sens*100)/key->sens)/1;
            break;
        }
    }
#endif
    return rt;
}

static uint32_t get_apc_sense(uint32_t row, uint32_t col)
{
    uint32_t apc = amk_apc_sens;
#ifdef APC_SPECIAL_KEY_NUM
    for (int i = 0; i < APC_SPECIAL_KEY_NUM; i++) {
        struct apc_special_key* key = &apc_special_keys[i];
        if (key->row == row && key->col == col) {
            apc = ((amk_apc_sens*100)/key->sens)/1;
            break;
        }
    }
#endif
    return apc;
}

static uint32_t get_noise_sense(uint32_t row, uint32_t col)
{
    uint32_t noise = amk_noise_sens;
#ifdef APC_SPECIAL_KEY_NUM
    for (int i = 0; i < APC_SPECIAL_KEY_NUM; i++) {
        struct apc_special_key* key = &apc_special_keys[i];
        if (key->row == row && key->col == col) {
            noise = ((amk_noise_sens*100)/key->sens)/1;
            break;
        }
    }
#endif
    return noise;
}

static void get_top_btm_sense(uint32_t row, uint32_t col, uint32_t* top, uint32_t* btm)
{
    *top = amk_top_sens;
    *btm = amk_btm_sens;
#ifdef APC_SPECIAL_KEY_NUM
    for (int i = 0; i < APC_SPECIAL_KEY_NUM; i++) {
        struct apc_special_key* key = &apc_special_keys[i];
        if (key->row == row && key->col == col) {
            *top = ((amk_top_sens*100)/key->sens)/1;
            *btm = ((amk_btm_sens*100)/key->sens)/1;
            break;
        }
    }
#endif
}

static void parse_ms_config(uint8_t config)
{
    amk_magnetive_pole = (config & AMK_POLE_MASK) >> AMK_POLE_OFFSET;
    amk_apcrt_profile = (config & AMK_PROFILE_MASK ) >> AMK_PROFILE_OFFSET;
    amk_dks_disable = (config & AMK_DKS_MASK) >> AMK_DKS_OFFSET;
}


static struct apc_key apc_matrix[AMK_STORE_APCRT_PROFILE_COUNT][MATRIX_ROWS][MATRIX_COLS];// SECTION_CCMRAM;

static uint32_t get_part_interval_table(uint32_t val, uint32_t max)
{
    uint32_t interval = (max * apc_rt_table[val]) / TABLE_MAX;
    return interval;
}

uint32_t get_part_interval_with_position(uint32_t val, uint32_t max, uint32_t pos, bool down)
{
    uint32_t pos_val = (pos*TABLE_MAX)/max;

    uint32_t index = 0;
    for (; index < ARRAY_SIZE(apc_rt_table)-1; index++) {
        if (pos_val < apc_rt_table[index]) {
            break;
        }
    }

    uint32_t begin = 0;
    uint32_t end = 0;
    if (down) {
        begin = index;
        end = index + val;
        if (end > ARRAY_SIZE(apc_rt_table)-1) {
            end = ARRAY_SIZE(apc_rt_table)-1;
        }
    } else {
        begin = (index > val) ? index - val : 0;
        end = index;
    }

    uint32_t interval = (max * (apc_rt_table[end]-apc_rt_table[begin])) / TABLE_MAX;
    return interval;
}

static void get_preset_params(uint32_t row, uint32_t col, uint32_t* min_preset, uint32_t* max_preset, uint32_t* interval_preset)
{
#ifdef APC_SPECIAL_KEY_NUM
    for (int i = 0; i < APC_SPECIAL_KEY_NUM; i++) {
        struct apc_special_key* key = &apc_special_keys[i];
        if (key->row == row && key->col == col) {
            *min_preset = key->min_preset;
            *max_preset = key->max_preset;
            *interval_preset = key->interval_preset;
            //apcrt_debug("APC: Get preset: row=%d, col=%d, min=%d, max=%d, interval\n", row, col, key->min_preset, key->max_preset, key->interval_preset);
            return;
        }
    }
#endif
    *min_preset = APC_KEY_MIN_PRESET;
    *max_preset = APC_KEY_MAX_PRESET;
    *interval_preset = APC_INTERVAL_PRESET;
}

static uint32_t compute_interval_apc(uint32_t row, uint32_t col, uint32_t value)
{
    if (value == 0) return APC_INTERVAL_INVALID;

    volatile struct apc_key *key = &apc_matrix[amk_apcrt_profile][row][col];
    uint32_t interval = APC_INTERVAL_INVALID;
    uint32_t min_preset = 0;
    uint32_t max_preset = 0;
    uint32_t interval_preset = 0;
    get_preset_params(row, col, &min_preset, &max_preset, &interval_preset);
    uint32_t apc_sens = get_apc_sense(row, col);

    if ((key->min > min_preset) || (key->max < max_preset) || (key->max < (key->min+apc_sens))) {
        interval = apc_sens + get_part_interval_table(value, interval_preset - apc_sens);
    } else {
        interval = apc_sens + get_part_interval_table(value, key->max - key->min - apc_sens);
    }

    return interval;
}

static uint32_t compute_interval_rt(uint32_t row, uint32_t col, struct apc_key* key, bool down)
{
    uint32_t interval_max = 0;
    uint32_t min_preset = 0;
    uint32_t max_preset = 0;
    uint32_t interval_preset = 0;
    get_preset_params(row, col, &min_preset, &max_preset, &interval_preset);
    if ((key->min > min_preset) || (key->max < max_preset) || (key->max < (key->min+amk_apc_sens))) {
        interval_max  = interval_preset;
    } else {
        interval_max = key->max - key->min;
    }
    uint32_t rt_sens = get_rt_sense(row, col);


#if 0
    if (down) {
        uint32_t interval = 0;
        if (key->rt_down) {
            interval = get_part_interval_table(key->rt_down, interval_max);
        } else {
            interval = get_part_interval_table(key->rt_up, interval_max);
        }
        return (interval + rt_sens);
    } else {
        uint32_t interval = get_part_interval_table(40-key->rt_up, interval_max);
        return (interval_max - interval + rt_sens);
    }
#else 
    uint32_t rt;
    if (down && key->rt_down) {
        rt = key->rt_down;
    } else {
        rt = key->rt_up;
    }
    uint32_t pos = key->last > key->min ? key->last - key->min : 0;
    uint32_t interval = get_part_interval_with_position(rt, interval_max, pos, down);
    return interval + rt_sens;
#endif
}

static bool is_adc_value_valid(uint32_t value)
{
    if (value < APC_KEY_MIN || value > APC_KEY_MAX) return false;

    return true;
}


//#define CHECK_MINMAX
uint32_t apc_matrix_calibrate(uint32_t row, uint32_t col, uint32_t adc_value)
{
    if (!is_adc_value_valid(adc_value)) {
        return 0;
    }

    struct apc_key* key = &apc_matrix[amk_apcrt_profile][row][col];
    key->value += adc_value;
    key->count = (key->count+1) % APC_KEY_COUNT;
    if (key->count != 0) {
        return 0;
    }

    uint32_t value = key->value / APC_KEY_COUNT;
    key->value = 0;
    uint32_t top_sens = 0;
    uint32_t btm_sens = 0;
    get_top_btm_sense(row, col, &top_sens, &btm_sens);

#ifdef CHECK_MINMAX
    bool updated = false;
#endif

    if (key->min > value + top_sens) {
        key->min = value + top_sens;
#ifdef CHECK_MINMAX
        updated = true;
#endif
    }

    if ((value > btm_sens) && (key->max < value - btm_sens)) {
        key->max = value - btm_sens;
#ifdef CHECK_MINMAX
        updated = true;
#endif
    }

#ifdef CHECK_MINMAX
    if (updated) {
        apcrt_debug("EC KEY Updated: row=%d, col=%d, min=%d, max=%d\n", row, col, key->min, key->max);
    }
#endif
    return value;
}

void apc_matrix_init(void)
{
    amk_udpate_ms_config();

    apc_matrix_update_sensitivity();

    for (int profile = 0; profile < AMK_STORE_APCRT_PROFILE_COUNT; profile++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                apc_matrix[profile][row][col].state = APC_KEY_STATE_DEFAULT;
                apc_matrix[profile][row][col].min = APC_KEY_MIN_DEFAULT;
                apc_matrix[profile][row][col].max = APC_KEY_MAX_DEFAULT;
                apc_matrix[profile][row][col].last = APC_KEY_LAST_DEFAULT;
                apc_matrix[profile][row][col].count = 0;
                apc_matrix[profile][row][col].value = 0;
                apc_matrix[profile][row][col].active = false;
                //apc_matrix[profile][row][col].dks_disable = false;

                apc_matrix_update_interval_at(profile, row, col);
            }
        }
    }

    dks_matrix_init();
}

bool apc_matrix_update(uint32_t row, uint32_t col, uint32_t org_value, bool* valid)
{
#ifdef MATRIX_NO_KEY_MASK
    if ((1<<col)&nokey_mask[row]) {
        return false;
    }
#endif
    uint32_t adc_value = org_value;
    if (amk_magnetive_pole) {
        adc_value = APC_POLE_MAX - org_value;
        //apcrt_debug("row=%d, col=%d, value=%d\n", row, col, org_value);
    }
    //return false;

    if (!amk_dks_disable /*&& !apc_matrix[amk_apcrt_profile][row][col].dks_disable*/ && dks_matrix_valid(row, col)) {
        apcrt_debug("USE DKS: row=%d, col=%d, value=%d\n", row, col, org_value);
        return dks_matrix_update(row, col, adc_value);
    }

    struct apc_key* key = &apc_matrix[amk_apcrt_profile][row][col];
    uint32_t value = apc_matrix_calibrate(row, col, adc_value);
    if (value == 0) {
        return key->state == APC_KEY_ON;
    }

    *valid = true;
    uint32_t noise_sens = get_noise_sense(row, col);

    uint32_t apc = compute_interval_apc(row, col, key->apc);
    uint32_t cur_max = key->max > APC_KEY_MAX_PRESET ? key->max : APC_KEY_MAX_PRESET;
    if (cur_max > key->min + apc) {
        cur_max = key->min + apc;
    }

    switch(key->state) {
    case APC_KEY_OFF:
        if (key->last > value) {
            key->last = value;
        }

        if (key->rt_up == APC_INTERVAL_INVALID) {
            // no RT
            if (value > cur_max) {
                apcrt_debug("APC from OFF to ON, value=%ld, min=%ld, max=%ld, cur_max=%ld, last=%ld, apc=%ld\n", value, key->min, key->max, cur_max, key->last, apc);
                key->last = value;
                key->state = APC_KEY_ON;
            }
        } else {
            if (value > cur_max || (key->active&&key->rt_cont)) {
                uint32_t rt = compute_interval_rt(row, col, key, true);
                if (value > key->last + rt || 
                    !key->active ||
                    (key->active && (value > key->last && value > key->max))) {
                    // rt turn on
                    apcrt_debug("RT from OFF to ON, value=%ld, min=%ld, max=%ld, cur_max=%ld, last=%ld, apc=%ld, rt=%ld\n", value, key->min, key->max, cur_max, key->last, apc, rt);
                    key->last = value;
                    key->state = APC_KEY_ON;
                }
            }
        }

        if (key->state == APC_KEY_ON) {
            if (key->active == false) {
                key->active = true;
                apcrt_debug("Key Active turned ON\n");
            }
        } else {
            if (key->active == true) {
                if (value < (key->min + noise_sens)) {
                    key->active = false;
                    apcrt_debug("Key Active turned OFF: value=%ld, min=%ld, max=%ld, cur_max=%ld, last=%ld\n", value, key->min, key->max, cur_max, key->last);
                }
            }
        }
        break;
    case APC_KEY_ON:
        if (key->last < value) {
            key->last = value;
        }

        if (key->rt_up == APC_INTERVAL_INVALID) {
            // no RT
            if ((value + noise_sens) < cur_max) {
                apcrt_debug("APC from ON to OFF, value=%ld, min=%ld, max=%ld, cur_max=%ld, last=%ld, apc=%ld\n", value, key->min, key->max, cur_max, key->last, apc);
                key->last = value;
                key->state = APC_KEY_OFF;
            }
        } else {
            if (key->last > key->max) {
                key->last = key->max;
            }
            uint32_t rt = compute_interval_rt(row, col, key, false);
            if ((value + rt) < key->last || 
                (!key->rt_cont && (value + noise_sens) < cur_max) || 
                (key->rt_cont && (value < key->min + noise_sens)))  {
                // rt turn off
                apcrt_debug("RT from ON to OFF, value=%ld, min=%ld, max=%ld, cur_max=%ld, last=%ld, apc=%ld, rt=%ld\n", value, key->min, key->max, cur_max, key->last, apc, rt);
                key->last = value;
                key->state = APC_KEY_OFF;
            }
        }
        break;
    default:
        // never here
        break;
    }

    return key->state == APC_KEY_ON;
}

void apc_matrix_update_interval(void)
{
    for (int profile = 0; profile < AMK_STORE_APCRT_PROFILE_COUNT; profile++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int col = 0; col < MATRIX_COLS; col++) {
                apc_matrix_update_interval_at(profile, row, col);
            }
        }
    }
}

void apc_matrix_update_interval_at(uint8_t profile, uint32_t row, uint32_t col)
{
    apc_matrix[profile][row][col].apc = amk_store_get_apc(profile, row, col);
    uint16_t rt = amk_store_get_rt(profile, row, col);
    apc_matrix[profile][row][col].rt_up = (rt&0x003F);
    apc_matrix[profile][row][col].rt_down = (rt&0x0FC0) >> 6;
    apc_matrix[profile][row][col].rt_cont = (rt&0x8000) > 0 ? true : false;
    //apc_matrix[profile][row][col].dks_disable = (rt&0x4000) > 0 ? true : false;
}

void apc_matrix_update_sensitivity(void)
{
    amk_rt_sens = eeconfig_read_rt_sens();
    if (amk_rt_sens == 0) {
        amk_rt_sens = AMK_RT_SENS_DEFAULT;
        eeconfig_update_rt_sens(amk_rt_sens);
    }
    amk_top_sens = eeconfig_read_top_sens();
    if (amk_top_sens == 0) {
        amk_top_sens = AMK_TOP_SENS_DEFAULT;
        eeconfig_update_top_sens(amk_top_sens);
    }

    amk_btm_sens = eeconfig_read_btm_sens();
    if (amk_btm_sens == 0) {
        amk_btm_sens = AMK_BTM_SENS_DEFAULT;
        eeconfig_update_btm_sens(amk_btm_sens);
    }

    amk_apc_sens = eeconfig_read_apc_sens();
    if (amk_apc_sens == 0) {
        amk_apc_sens = AMK_APC_SENS_DEFAULT;
        eeconfig_update_apc_sens(amk_apc_sens);
    }

    amk_noise_sens = eeconfig_read_noise_sens();
    if (amk_noise_sens == 0) {
        amk_noise_sens = AMK_APC_NOISE_DEFAULT;
        eeconfig_update_noise_sens(amk_noise_sens);
    }
    apcrt_debug("Sensitivity: rt=%d, top=%d, bottom=%d, apc=%d, noise=%d\n", amk_rt_sens, amk_top_sens, amk_btm_sens, amk_apc_sens, amk_noise_sens);
}

void amk_set_ms_config(uint8_t config)
{
    parse_ms_config(config);
    uint8_t old = eeconfig_read_ms_config();
    if (old != config) {
        apcrt_debug("AMK MS: Update config for 0x%x to 0x%x\n", old, config);
        eeconfig_update_ms_config(config);
    }
}

uint8_t amk_get_ms_config(void)
{
    uint8_t config = 0;
    config |= ((amk_magnetive_pole << AMK_POLE_OFFSET) & AMK_POLE_MASK);
    config |= ((amk_apcrt_profile << AMK_PROFILE_OFFSET) & AMK_PROFILE_MASK);
    config |= ((amk_dks_disable << AMK_DKS_OFFSET) & AMK_DKS_MASK);

    return config;
}

void amk_udpate_ms_config(void)
{
    uint8_t config = eeconfig_read_ms_config();
    parse_ms_config(config);
}