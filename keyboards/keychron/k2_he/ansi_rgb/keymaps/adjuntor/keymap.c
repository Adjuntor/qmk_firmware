/* Copyright 2024 @ Keychron (https://www.keychron.com)
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
#include "keychron_common.h"

enum layers {
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN,
    MACRO,

};

enum custom_keycodes {
    SNIPERMACRO = SAFE_RANGE,
    SNIPERCOOP,
    ADVISE,
    LMOUSECLICK,
    RMOUSECLICK,
    GOBACK
};

bool spam_snipermacro  = false;
bool spam_snipercoop  = false;
bool spam_advise = false;
bool spam_lmouseclick = false;
bool spam_rmouseclick = false;
uint32_t spam_timer = 0;

int randy(int max, int min){
   return (rand() % (max + 1 - min)) + min;
}

void matrix_scan_user(void) {
  if (spam_snipermacro) {
    if (timer_elapsed32(spam_timer) > randy(50,20)) {
        tap_code_delay(KC_MS_BTN1, randy(250,200));
        spam_timer = timer_read32();
    }
  }
  if (spam_snipercoop) {
    if (timer_elapsed32(spam_timer) > randy(100,40)) {
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

  if (spam_lmouseclick) {
    if (timer_elapsed32(spam_timer) > randy(100,80)) {
        tap_code_delay(KC_MS_BTN1, randy(50,20));
        spam_timer = timer_read32();
    }
  }

  if (spam_rmouseclick) {
    if (timer_elapsed32(spam_timer) > randy(100,80)) {
        tap_code_delay(KC_MS_BTN2, randy(50,20));
        spam_timer = timer_read32();
    }
  }

};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) { 
     for (uint8_t i = led_min; i <= led_max; i++) {
        if (spam_advise == true) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_BLUE);
            }
        }
        
        if (spam_lmouseclick == true) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_BLUE);
            }
        }  
        if (spam_rmouseclick == true) {
            int keys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_BLUE);
            }
        }

        switch(get_highest_layer(layer_state|default_layer_state)) {

        case WIN_BASE:
            rgb_matrix_set_color(i, 0x00, 0x00, 0x00);  // RGB Off
            break;
        case WIN_FN:
            rgb_matrix_set_color(i, 0x15, 0x80, 0x15);  // RGB Green
            break;
        case MACRO:
            rgb_matrix_set_color(i, 0x8B, 0x45, 0x13);  // RGB Saddle Brown
            break;
        default:
            break;
        }
    }
    return false;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[MAC_BASE] = LAYOUT_ansi_84(
     KC_ESC,   KC_BRID,  KC_BRIU,  KC_MCTRL, KC_LNPAD, RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  KC_SNAP,  KC_DEL,   RGB_MOD,
     KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
     KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
     KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,    KC_END,
     KC_LCTL,  KC_LOPTN, KC_LCMMD,                               KC_SPC,                                 KC_RCMMD,MO(MAC_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

[MAC_FN] = LAYOUT_ansi_84(
     _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   _______,  _______,  RGB_TOG,
     _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
     _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
     _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
     _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

[WIN_BASE] = LAYOUT_ansi_84(
     KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_DEL,   TG(MACRO),
     KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,            KC_PGUP,
     KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,            KC_PGDN,
     KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,             KC_HOME,
     KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,  KC_UP,    KC_END,
     KC_LCTL,  KC_LGUI,  KC_LALT,                                KC_SPC,                                 KC_RALT, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

[WIN_FN] = LAYOUT_ansi_84(
     _______,  KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  _______,
     _______,  BT_HST1,  BT_HST2,  BT_HST3,  P2P4G,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
     RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,
     _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,
     _______,            _______,  _______,  _______,  _______,  BAT_LVL,  NK_TOGG,  _______,  _______,  _______,  _______,          _______,  _______,  _______,
     _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______),

[MACRO] = LAYOUT_ansi_84(
     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  GOBACK,
     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            ADVISE,
     _______,  _______,  _______,  _______,  SNIPERMACRO,  SNIPERCOOP,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,     LMOUSECLICK,
     _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            RMOUSECLICK,
     _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,  _______,  _______,
     _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______)
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

    case ADVISE:
        if (record->event.pressed) {
            spam_advise = !spam_advise;
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

    case RMOUSECLICK:
        if (record->event.pressed) {
            spam_rmouseclick = !spam_rmouseclick;
            spam_timer = timer_read32();
        } else {
            //nothing
        }
        break;

    case GOBACK:
        if (record->event.pressed) {
            spam_snipermacro  = false;
	    spam_snipercoop  = false;
	    spam_advise = false;
	    spam_lmouseclick = false;
	    spam_rmouseclick = false;
	    spam_timer = timer_read32();
            //layer_invert(MACRO);
        } else {
            layer_move(WIN_BASE);
            //nothing
        }
        break;

    }

    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }
    return true;
}
