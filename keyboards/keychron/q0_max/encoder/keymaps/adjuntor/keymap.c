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
    BASE,
    NIKKE,
};

enum custom_keycodes {
    ALICEMACRO = SAFE_RANGE,
    ADVISE,
    LMOUSECLICK,
    RMOUSECLICK
};

bool spam_alicemacro  = false;
bool spam_advise = false;
bool spam_lmouseclick = false;
bool spam_rmouseclick = false;
uint32_t spam_timer = 0;

int randy(int max, int min){
   return (rand() % (max + 1 - min)) + min;
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
        if (host_keyboard_led_state().num_lock) {
            rgb_matrix_set_color(4, RGB_WHITE);
        } else {
            rgb_matrix_set_color(4, RGB_BLACK);
        }
        
        if (spam_advise == true) {
            rgb_matrix_set_color(14, RGB_BLUE);
        }
        
        if (spam_lmouseclick == true) {
            int keys[] = {4, 5, 6, 7};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }
        if (spam_rmouseclick == true) {
            int keys[] = {9, 10, 11, 12};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }
        if (spam_advise == true) {
            int keys[] = {18, 19, 20};
            for(uint8_t r = 0; r < sizeof(keys)/sizeof(int); r++) {
                rgb_matrix_set_color(keys[r], RGB_RED);
            }
        }

        switch(get_highest_layer(layer_state|default_layer_state)) {

        case BASE:
            rgb_matrix_set_color(i, 0x00, 0x00, 0x00);  // RGB off
            break;
        case NIKKE:
            rgb_matrix_set_color(i, 0xF0, 0x14, 0x3C);  // RGB Crimson
            break;
        default:
            break;
        }
    }
    return false;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_tenkey_27(
        _______,         BT_HST1, BT_HST2, BT_HST3, P2P4G,
        TG(NIKKE),	 KC_NUM, KC_PSLS,KC_PAST,KC_PMNS,
        _______,	 KC_P7,	 KC_P8,	 KC_P9,	 KC_PPLS,
        _______,	 KC_P4,	 KC_P5,	 KC_P6,
        _______,	 KC_P1,	 KC_P2,	 KC_P3,	 KC_PENT,
        _______,         KC_P0,          KC_PDOT         ),

    [NIKKE] = LAYOUT_tenkey_27(
        KC_Z,  KC_X, KC_C, KC_V, KC_B,
        TO(BASE), KC_A, KC_S, KC_D, KC_F0,
        LMOUSECLICK,  _______, _______, _______, _______,
        RMOUSECLICK,  _______, _______, _______,
        ______,  _______, _______, _______, _______,
        ADVISE,  _______,          ALICEMACRO,          )
};

// clang-format on
//#if defined(ENCODER_MAP_ENABLE)
//const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
//    [BASE] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
//    [FN]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
//    [L2]   = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU)},
//    [L3]   = {ENCODER_CCW_CW(RGB_VAD, RGB_VAI)},
//};
//#endif // ENCODER_MAP_ENABLE

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_keychron_common(keycode, record)) {
        return false;
    }
    return true;
}
