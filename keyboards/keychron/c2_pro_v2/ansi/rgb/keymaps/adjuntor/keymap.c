/* Copyright 2025 @ Keychron (https://www.keychron.com)
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
  MACRO,
  WIN_FN,
};

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

enum custom_keycodes {
    SNIPERMACRO = SAFE_RANGE,
    SNIPERCOOP,
    LMOUSECLICK,
    RMOUSECLICK,
    GOBACK,
};

// RANDOM TIME FUNCTION
int randy(int max, int min){
   return (rand() % (max + 1 - min)) + min;
}

void keyboard_post_init_user(void) {
  // Call the post init code.
    rgb_matrix_mode(RGB_MATRIX_CUSTOM_default_off);
}

bool spam_snipermacro = false;
bool spam_snipercoop  = false;
bool spam_lmouseclick = false;
bool spam_rmouseclick = false;
uint32_t spam_timer = 0;

// MACRO
void matrix_scan_user(void) {
  if (spam_snipermacro) {
    if (timer_elapsed32(spam_timer) > randy(50,20)) {
        tap_code_delay(MS_BTN1, randy(250,200));
        spam_timer = timer_read32();
    }
  }
  if (spam_snipercoop) {
    if (timer_elapsed32(spam_timer) > randy(100,40)) {
        tap_code_delay(MS_BTN1, randy(250,200));
        spam_timer = timer_read32();
    }
  }

  if (spam_lmouseclick) {
    if (timer_elapsed32(spam_timer) > randy(100,80)) {
        tap_code_delay(MS_BTN1, randy(50,20));
        spam_timer = timer_read32();
    }
  }

  if (spam_rmouseclick) {
    if (timer_elapsed32(spam_timer) > randy(100,80)) {
        tap_code_delay(MS_BTN2, randy(50,20));
        spam_timer = timer_read32();
    }
  }

};

// Change LED
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 

    uint8_t layer = get_highest_layer(layer_state);
    for (uint8_t i = led_min; i < led_max; i++) {
        switch(get_highest_layer(layer_state|default_layer_state)) {
            case MACRO:
                for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
                    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                        uint8_t index = g_led_config.matrix_co[row][col];
                        if (index >= led_min && index < led_max && index != NO_LED && keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) {
                            rgb_matrix_set_color(index, 0xDC, 0x14, 0x3C);  // RGB Crimson
                        }
                    }
                }
                break;
            case WIN_FN:
            case MAC_FN:
                for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
                    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
                        uint8_t index = g_led_config.matrix_co[row][col];
                        if (index >= led_min && index < led_max && index != NO_LED && keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS) {
                            rgb_matrix_set_color(index, 0x15, 0x80, 0x15);  // RGB Green
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    if (spam_lmouseclick == true) {
        rgb_matrix_set_color(30, RGB_BLUE);
    }  
    if (spam_rmouseclick == true) {
        rgb_matrix_set_color(45, RGB_BLUE);
    }    
    return false;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_BRID,  KC_BRIU,  _______,  _______,  RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  KC_NO,    KC_NO,    RM_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LOPT,  KC_LCMD,                                KC_SPC,                                 KC_RCMD,  KC_ROPT,  MO(MAC_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [MAC_FN] = LAYOUT_104_ansi(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   _______,  _______,  RM_NEXT,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_BASE] = LAYOUT_104_ansi(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_PSCR,  KC_NO,    TG(MACRO),
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  MO(WIN_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT         ),

    [MACRO] = LAYOUT_104_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  GOBACK,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  LMOUSECLICK,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  SNIPERMACRO,  SNIPERCOOP,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  RMOUSECLICK,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),

    [WIN_FN] = LAYOUT_104_ansi(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FLXP,  RM_VALU,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  _______,  _______,  RM_TOGG,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______,  _______,            _______         ),
};

// clang-format on
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case SNIPERMACRO:
        if (record->event.pressed) {
            spam_snipermacro = true;
            spam_timer = timer_read32();
        } else {
            spam_snipermacro = false;
            spam_timer = timer_read32();
        }
        break;

    case SNIPERCOOP:
        if (record->event.pressed) {
            spam_snipercoop = true;
            spam_timer = timer_read32();
        } else {
            spam_snipercoop = false;
            spam_timer = timer_read32();
        }
        break;
    
    case LMOUSECLICK:
        if (record->event.pressed) {
            spam_lmouseclick = !spam_lmouseclick;
            spam_timer = timer_read32();
        }
        break;

    case RMOUSECLICK:
        if (record->event.pressed) {
            spam_rmouseclick = !spam_rmouseclick;
            spam_timer = timer_read32();
        }
        break;

// DISABLE ALL MACROS AND RETURN TO BASE LAYER
    case GOBACK:
        if (record->event.pressed) {
        spam_snipermacro = false;
	    spam_snipercoop  = false;
	    spam_lmouseclick = false;
	    spam_rmouseclick = false;
	    spam_timer = timer_read32();
        } else {
            layer_off(MACRO);
        }
        break;
    }
    return true;
}

