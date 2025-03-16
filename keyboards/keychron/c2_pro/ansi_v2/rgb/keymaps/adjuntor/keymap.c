/* Copyright 2023 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum layers{
  MAC_BASE,
  MAC_FN,
  WIN_BASE,
  WIN_FN,
  WIN_NIKKE,
  WIN_MACRO,
  WIN_FORTNITE,
};


enum custom_keycodes {
    ALICEMACRO = SAFE_RANGE,
    ADVISE,
    BURST,
    STAB,
    ASSASSINATE,
    JAM,
    LMOUSECLICK,
  
};

bool spam_alicemacro  = false;
bool spam_advise = false;
bool spam_burst = false;
bool spam_assassinate  = false;
bool spam_stab  = false;
bool spam_jam = false;
bool spam_lmouseclick = false;
uint32_t spam_timer = 0;

int randy(int max, int min){
   return (rand() % (max + 1 - min)) + min;
}

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

bool win_mode;   // declare global variable

bool dip_switch_update_user(uint8_t index, bool active) { 
    if (index == 0 && active) { 
        win_mode = true;
    } else {
        win_mode = false;
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
    case ALICEMACRO:
        if (record->event.pressed) {
            spam_alicemacro = true;
            spam_timer = timer_read32();
        } else {
            spam_alicemacro = false;
            spam_timer = timer_read32();
        }
        break;

    case ADVISE:
        if (record->event.pressed) {
            spam_advise = !spam_advise;
            spam_timer = timer_read32();
        } else {
            //nothing
        }
        break;

    case BURST:
        if (record->event.pressed) {
            spam_burst = !spam_burst;
            spam_timer = timer_read32();
        } else {
            //nothing
        }
        break;

    case ASSASSINATE:
        if (record->event.pressed) {
            spam_assassinate = true;
        } else {
            spam_assassinate = false;
        }
        break;

    case STAB:
        if (record->event.pressed) {
            spam_stab = true;
        } else {
            spam_stab = false;
        }
        break;

    case JAM:
        if (record->event.pressed) {
            spam_jam = !spam_jam;
            spam_timer = timer_read32();
        } else {
            //nothing
        }
        break;
    
    case LMOUSECLICK:
        if (record->event.pressed) {
            spam_lmouseclick = !spam_lmouseclick;
            spam_timer = timer_read32();
        } else {
            //nothing
        }
        break;
    }
    return true;
};


void matrix_scan_user(void) {
  if (spam_alicemacro) {
    if (timer_elapsed32(spam_timer) > randy(50,20)) {
        tap_code_delay(KC_MS_BTN1, randy(250,200));
        spam_timer = timer_read32();
    }
  }
  if (spam_advise) {
    if (timer_elapsed32(spam_timer) > randy(50,20)) {
        tap_code_delay(KC_1, randy(80,60));
        tap_code_delay(KC_SPACE, randy(80,60));
        tap_code_delay(KC_RIGHT_BRACKET, randy(80,60));
        spam_timer = timer_read32();
    }
  }
  if (spam_burst) {
    if (timer_elapsed32(spam_timer) > randy(50,20)) {
        tap_code_delay(KC_A, randy(80,60));
        tap_code_delay(KC_S, randy(80,60));
        tap_code_delay(KC_D, randy(80,60));
        tap_code_delay(KC_F, randy(80,60));
        spam_timer = timer_read32();
    }
  }

  if (spam_assassinate) {
    SEND_STRING( SS_TAP(X_LEFT_CTRL) SS_TAP(X_Y));
  }
  if (spam_stab) {
    SEND_STRING( SS_TAP(X_LEFT_SHIFT) SS_TAP(X_Y) SS_TAP(X_U));
  }
  if (spam_jam) {
    if (timer_elapsed32(spam_timer) > randy(150000,60000)) {
        SEND_STRING(SS_DOWN(X_B) SS_DELAY(200) SS_TAP(X_SPACE) SS_DELAY(200) SS_UP(X_B));
        spam_timer = timer_read32();
    }
  }
  if (spam_lmouseclick) {
    if (timer_elapsed32(spam_timer) > randy(100,80)) {
        tap_code_delay(KC_MS_BTN1, randy(50,20));
        spam_timer = timer_read32();
    }
  }
};

/*
*  RGB ID_KEY
*  **********
*  0_ESC,              1_F1,     2_F2,     3_F3,     4_F4,     5_F5,     6_F6,     7_F7,     8_F8,     9_F9,     10_F10,   11_F11,     12_F12,   13_PSCR,  14_NO,    15_MOD,
*  16_GRV,   17_1,     18_2,     19_3,     20_4,     21_5,     22_6,     23_7,     24_8,     25_9,     26_0,     27_MINS,  28_EQL,     29_BSPC,  30_INS,   31_HOME,  32_PGUP,    33_NUM,   34_PSLS,  35_PAST,  36_PMNS,
*  37_TAB,   38_Q,     39_W,     40_E,     41_R,     42_T,     43_Y,     44_U,     45_I,     46_O,     47_P,     48_LBRC,  49_RBRC,    50_BSLS,  51_DEL,   52_END,   53_PGDN,    54_P7,    55_P8,    56_P9,    57_PPLS,
*  58_CAPS,  59_A,     60_S,     61_D,     62_F,     63_G,     64_H,     65_J,     66_K,     67_L,     68_SCLN,  69_QUOT,              70_ENT,                                   71_P4,    72_P5,    73_P6,
*  74_LSFT,            75_Z,     76_X,     77_C,     78_V,     79_B,     80_N,     81_M,     82_COMM,  83_DOT,   84_SLSH,              85_RSFT,            86_UP,                87_P1,    88_P2,    89_P3,    90_PENT,
*  91_LCTL,  92_LWIN,  93_LALT,                                94_SPC,                                 95_RALT,  96_RWIN,  97_FN),     98_RCTL,  99_LEFT,  100_DOWN,  101_RGHT,  102_P0,             103_PDOT         ),
*/

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 
     for (uint8_t i = led_min; i <= led_max; i++) {
        if (host_keyboard_led_state().caps_lock) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 37, 74, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 86, 101, 58, 30, 31, 32, 51, 52, 53};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }
        if (host_keyboard_led_state().num_lock) {
            int keys[] = {33, 34, 35, 36, 54, 55, 56, 57, 71, 72, 73, 87, 88, 89, 90, 102, 103};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_WHITE);
            } 
        } else {
            int keys[] = {33, 34, 35, 36, 54, 55, 56, 57, 71, 72, 73, 87, 88, 89, 90, 102, 103};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_BLACK);
            }
        }
        
        if (spam_advise == true) {
            rgb_matrix_set_color(14, RGB_BLUE);
        }
        
        if (spam_burst == true) {
            rgb_matrix_set_color(13, RGB_BLUE);
        }

        if (spam_jam == true) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 65};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }

        if (spam_lmouseclick == true) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 81};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }

        switch(get_highest_layer(layer_state|default_layer_state)) {

        case MAC_BASE:
        case WIN_BASE:
            if (win_mode) {
                rgb_matrix_set_color(i, 0x2C, 0x52, 0x8C);  // RGB YInMn Blue
            } else {
                rgb_matrix_set_color(i, 0x9E, 0x9E, 0x9E);  // RGB white
            }
            break;
        case WIN_FN:
            rgb_matrix_set_color(i, 0x00, 0x9E, 0x00);  // RGB green
            break;
        case MAC_FN:
            rgb_matrix_set_color(i, 0x51, 0xA0, 0xD5);  // RGB Carolina Blue
            break;
        case WIN_NIKKE:
            rgb_matrix_set_color(i, 0xF0, 0x14, 0x3C);  // RGB Crimson
            break;
        case WIN_MACRO:
            rgb_matrix_set_color(i, 0xFF, 0x22, 0x00);  // RGB orange
            break;
        case WIN_FORTNITE:
            rgb_matrix_set_color(i, 0x00,0x00, 0xFF); // RGB blue
            break;
        default:
            break;
        }
    }
    return false;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_MCTL,  KC_LPAD,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  KC_NO,    KC_NO,    RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LOPT,  KC_LCMD,                                KC_SPC,                                 KC_RCMD,  KC_ROPT,  MO(MAC_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [MAC_FN] = LAYOUT_104_ansi(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   _______,  _______,  RGB_TOG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_PSCR,  RGB_TOG,    RGB_MOD,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  MO(WIN_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [WIN_FN] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  TO(WIN_BASE),
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,    _______, _______,  _______,  TG(WIN_FORTNITE),  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,     _______,  _______,  _______,  _______,  _______,  TG(WIN_NIKKE),  TG(WIN_MACRO),  _______,  _______,  _______,  _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_NIKKE] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  BURST,  ADVISE,  TO(WIN_BASE),
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  ALICEMACRO,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,    _______, _______,  _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_MACRO] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  TO(WIN_BASE),
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,    _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),
    
    [WIN_FORTNITE] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  TO(WIN_BASE),
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,   _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,    _______,  _______,  _______,  _______,  _______,  _______,  JAM,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,     _______,  _______,  _______,  _______,  _______,  _______,  LMOUSECLICK,  _______,  _______,  _______,  _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

};
